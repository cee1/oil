/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Modified from glib's hash table
 * Not MT safe
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_MIN_SHIFT 3  /* 1 << 3 == 8 buckets */

typedef struct {
    void *key;
    void *value;
    /*
     * key_hash == 0 -> node is not in use
     * key_hash == 1 -> node is a tombstone
     * key_hash == 2 -> node contains data
     */
    unsigned int key_hash;
}HashNode;

struct _HashTable {
    unsigned int size;
    unsigned int mod;
    unsigned int mask;
    
    unsigned int nnodes;
    unsigned int noccupied;  /* nnodes + tombstones */
    
    HashNode *nodes;
    
    HashFunc hash_func;
    EqualFunc key_equal_func;
    DestroyNotify key_destroy_func;
    DestroyNotify value_destroy_func;
};


static unsigned int _hash_table_find_closest_shift (unsigned int n);

static void _hash_table_set_shift_from_size (
        HashTable *ht,
        unsigned int size);

static void _hash_table_set_shift (HashTable *ht, unsigned int shift);

static inline unsigned int
_hash_table_lookup_node (HashTable *ht, const void *key);

static inline unsigned int
_hash_table_lookup_node_for_insertion (
        HashTable *ht,
        const void *key,
        unsigned int *hash_return);

static inline void _hash_table_maybe_resize (HashTable *ht);

static void _hash_table_resize (HashTable *ht);

static void _hash_table_remove_node (HashTable *ht, HashNode *node);

static void _hash_table_remove_all_nodes (HashTable *ht);

/* 
 * Each table size has an associated prime modulo (the first prime
 * lower than the table size) used to find the initial bucket. Probing
 * then works modulo 2^n. The prime modulo is necessary to get a
 * good distribution with poor hash functions.
 */
static const int prime_mod[] = {
    1,  /* For 1 << 0 */
    2,
    3,
    7,
    13,
    31,
    61,
    127,
    251,
    509,
    1021,
    2039,
    4093,
    8191,
    16381,
    32749,
    65521,  /* For 1 << 16 */
    131071,
    262139,
    524287,
    1048573,
    2097143,
    4194301,
    8388593,
    16777213,
    33554393,
    67108859,
    134217689,
    268435399,
    536870909,
    1073741789,
    2147483647  /* For 1 << 31 */
};


HashTable *hash_table_new (
        HashFunc hash_func, 
        EqualFunc key_equal_func,
        DestroyNotify key_destroy_func,
        DestroyNotify value_destroy_func)
{
    HashTable *ht = NULL;
    
    if (hash_func == NULL)
        return NULL;
    
    ht = malloc (sizeof (HashTable));
    
    _hash_table_set_shift (ht, HASH_TABLE_MIN_SHIFT);
    ht->nnodes = 0;
    ht->noccupied = 0;
    
    ht->hash_func = hash_func;
    ht->key_equal_func = key_equal_func;
    ht->key_destroy_func = key_destroy_func;
    ht->value_destroy_func = value_destroy_func;
    
    ht->nodes = calloc (ht->size, sizeof (HashTable));
    
    return ht;
}

void hash_table_insert (HashTable *ht, void *key, void *val)
{
    HashNode *node;
    unsigned int node_index;
    unsigned int key_hash;
    unsigned int old_hash;
    
    if (ht == NULL || key == NULL)
        return;

    node_index = _hash_table_lookup_node_for_insertion (ht, key, &key_hash);
    node = &ht->nodes[node_index];

    old_hash = node->key_hash;

    if (old_hash > 1) {
        /* exists, replace the value & destroy the provided key */
        if (ht->key_destroy_func)
            ht->key_destroy_func (key);
        
        if (ht->value_destroy_func)
            ht->value_destroy_func (node->value);
        node->value = val;
    } else {
        /* empty or tombstones */
        node->key = key;
        node->value = val;
        node->key_hash = key_hash;

        ht->nnodes++;

        if (old_hash == 0) {
            /* We replaced an empty node, and not a tombstone */
            ht->noccupied++;
            _hash_table_maybe_resize (ht);
        }
    }
}

void *hash_table_lookup (HashTable *ht, const void *key)
{
    HashNode *node;
    unsigned int node_index;

    if (ht == NULL || key == NULL)
        return NULL;

    node_index = _hash_table_lookup_node (ht, key);
    node = &ht->nodes[node_index];

    return node->key_hash ? node->value : NULL;
}

void hash_table_foreach (
        HashTable *ht,
        HashTableVisitor  visitor,
        void *user_data)
{
    int i;
    
    if (ht == NULL || visitor == NULL)
        return;

    for (i = 0; i < ht->size; i++) {
        HashNode *node = &ht->nodes[i];

        if (node->key_hash > 1)
            (* visitor) (node->key, node->value, user_data);
    }
}

int hash_table_remove (HashTable *ht, const void *key)
{
    if (ht == NULL || key == NULL)
        return 0;
    
    unsigned int node_index = _hash_table_lookup_node (ht, key);
    HashNode *node = ht->nodes + node_index;

    /* 
     * _hash_table_lookup_node() never returns a tombstone, so this is safe 
     */
    if (!node->key_hash)
        return 0;

    _hash_table_remove_node (ht, node);
    _hash_table_maybe_resize (ht);
    
    return 1;
}

void hash_table_remove_all (HashTable *ht)
{
    if (ht == NULL)
        return;

    _hash_table_remove_all_nodes (ht);
    _hash_table_maybe_resize (ht);
}

void hash_table_destroy (HashTable *ht)
{
    if (ht == NULL)
        return;
    _hash_table_remove_all_nodes (ht);
    
    free (ht->nodes);
    free (ht);
}


static unsigned int _hash_table_find_closest_shift (unsigned int n)
{
    int i;

    for (i = 0; n; i++)
        n >>= 1;

    return i;
}

static void _hash_table_set_shift_from_size (
        HashTable *ht,
        unsigned int size)
{
    unsigned int shift;
    
    shift = _hash_table_find_closest_shift (size);
    
    if (shift < HASH_TABLE_MIN_SHIFT)
        shift = HASH_TABLE_MIN_SHIFT;

    _hash_table_set_shift (ht, shift);
}

static void _hash_table_set_shift (HashTable *ht, unsigned int shift)
{
    unsigned int mask = 0;
    
    ht->size = 1 << shift;
    ht->mod = prime_mod[shift];
    ht->mask = (1 << shift) - 1;
}

static inline unsigned int
_hash_table_lookup_node (HashTable *ht, const void *key)
{
    HashNode *node;
    unsigned int node_index;
    unsigned int hash_value;
    unsigned int step = 0;

    /* 
     * Empty buckets have hash_value set to 0, and for tombstones, it's 1.
     * We need to make sure our hash value is not one of these.
     */

    hash_value = (* ht->hash_func) (key);
    if (hash_value <= 1)
        hash_value = 2;

    node_index = hash_value % ht->mod;
    node = &ht->nodes[node_index];

    while (node->key_hash) {
        if (node->key_hash == hash_value) {
            if (ht->key_equal_func) {
                if (ht->key_equal_func (node->key, key))
                    break;
            } else if (node->key == key)
                break;
        }

        step++;
        node_index += step;
        node_index &= ht->mask;
        node = ht->nodes + node_index;
    }

    return node_index;
}

static inline unsigned int
_hash_table_lookup_node_for_insertion (
        HashTable *ht,
        const void *key,
        unsigned int *hash_return)
{
    HashNode *node;
    unsigned int node_index;
    unsigned int hash_value;
    unsigned int first_tombstone;
    int have_tombstone = 0;
    unsigned int step = 0;

    /* 
     * Empty buckets have hash_value set to 0, and for tombstones, it's 1.
     * We need to make sure our hash value is not one of these.
     */
    hash_value = (* ht->hash_func) (key);
    if (hash_value <= 1)
        hash_value = 2;

    *hash_return = hash_value;

    node_index = hash_value % ht->mod;
    node = &ht->nodes[node_index];

    while (node->key_hash) {
        /*  
         * We first check if our full hash values
         * are equal so we can avoid calling the full-blown
         * key equality function in most cases.
         */
        if (node->key_hash == hash_value) {
            if (ht->key_equal_func) {
                if (ht->key_equal_func (node->key, key))
                    return node_index;
            } else if (node->key == key)
                return node_index;
        } else if (node->key_hash == 1 && !have_tombstone) {
            first_tombstone = node_index;
            have_tombstone = 1;
        }
        
        step++;
        node_index += step;
        node_index &= ht->mask;
        node = &ht->nodes[node_index];
    }

    if (have_tombstone)
        return first_tombstone;

    return node_index;
}

static inline void _hash_table_maybe_resize (HashTable *ht)
{
    unsigned int noccupied = ht->noccupied;
    int size = ht->size;

    if ((size > ht->nnodes * 4 && size > 1 << HASH_TABLE_MIN_SHIFT) ||
        (size <= noccupied + (noccupied / 16)))
        _hash_table_resize (ht);
}

static void _hash_table_resize (HashTable *ht)
{
    HashNode *new_nodes;
    unsigned int old_size;
    int i;

    old_size = ht->size;
    _hash_table_set_shift_from_size (ht, ht->nnodes * 2);

    new_nodes = calloc  (ht->size, sizeof (HashNode));

    for (i = 0; i < old_size; i++) {
        HashNode *node = &ht->nodes[i];
        HashNode *new_node;
        unsigned int hash_val;
        unsigned int step = 0;

        if (node->key_hash <= 1)
            continue;

        hash_val = node->key_hash % ht->mod;
        new_node = &new_nodes[hash_val];

        while (new_node->key_hash) {
            step++;
            hash_val += step;
            hash_val &= ht->mask;
            new_node = &new_nodes[hash_val];
        }

        *new_node = *node;
    }

    free (ht->nodes);
    ht->nodes = new_nodes;
    ht->noccupied = ht->nnodes;
}

static void _hash_table_remove_node (HashTable *ht, HashNode *node)
{
    if (ht->key_destroy_func)
        ht->key_destroy_func (node->key);

    if (ht->value_destroy_func)
        ht->value_destroy_func (node->value);

    /* Erect tombstone */
    node->key_hash = 1;

    /* Be GC friendly */
    node->key = NULL;
    node->value = NULL;

    ht->nnodes--;
}

static void _hash_table_remove_all_nodes (HashTable *ht)
{
    int i;

    for (i = 0; i < ht->size; i++) {
        HashNode *node = &ht->nodes [i];

        if (node->key_hash > 1) {
            if (ht->key_destroy_func)
                ht->key_destroy_func (node->key);

            if (ht->value_destroy_func)
                ht->value_destroy_func (node->value);
        }
    }

    /* 
     * We need to set node->key_hash = 0 for all nodes - might as well be GC
   * friendly and clear everything
   */
    memset (ht->nodes, 0, ht->size * sizeof (HashNode));

    ht->nnodes = 0;
    ht->noccupied = 0;
}

