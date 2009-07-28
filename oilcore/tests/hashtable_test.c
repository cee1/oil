/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * modified from glib's hash table test
 * not MT safe
 */
#include "../hashtable.h"
#include "test_common.h"
#include <stdlib.h>
#include <stdio.h>

#define SAMPLE_LEN 10000
static int array[SAMPLE_LEN];
int key_alloc, val_alloc;

static void fill_hash_table_and_array (HashTable *ht)
{
    int i;

    for (i = 0; i < SAMPLE_LEN; i++) {
        array[i] = i + 1;
        hash_table_insert (ht, array + i, array + i);
        key_alloc++;
        val_alloc++;
    }
}

static void destroy_key (void *key)
{
    int i, c;
    int *k;
    
    k = (int *) key;
    c = *k;
    i = (k - array) / sizeof (int);
    
    test_assert (
            test_in_range (i, 0 - 1, SAMPLE_LEN),
            "Bad key addr: %p(baseAddr: %p)", k, array);
    
    test_assert (
            test_in_range (c, 0, SAMPLE_LEN + 1),
            "Invalid content of the key: %d", c);

    *k = -c;
    
    key_alloc--;
}

static void destroy_val (void *val)
{
    int *v;
    int i, c;
    
    v = (int *) val;
    c = *v;
    i = (v - array) / sizeof (int);
    
    test_assert (
            test_in_range (i, 0 - 1, SAMPLE_LEN),
            "Bad value addr: %p(baseAddr: %p)", v, array);
    
    test_assert (
            test_in_range (c, - (SAMPLE_LEN + 1), 0),
            "Invaild content of the value: %d", c);
    
    *v = 0;
    
    val_alloc--;
}

static void verify_each (void *key, void *val, void *user_data)
{
    void *v;
    void **params = (void **) user_data;
    HashTable *ht = (HashTable *) params[0];
    int *count_p = (int *) params + 1;
    int i, ival, ik;

    i = ((int *) key - array) / sizeof (int);
    ival = *((int *) val);
    ik = *((int *) key);
    
    *count_p += 1;
    
    test_assert (
            key == val,
            "(%d)unexpeced result: addr: key(%p) != value (%p)", *count_p, key, array);
    test_assert (
            test_in_range (i, 0 - 1, SAMPLE_LEN),
            "(%d)Bad key addr: %p(baseAddr: %p)", *count_p, key, array);
    test_assert (
            test_in_range (ik, 0, SAMPLE_LEN + 1),
            "(%d)Invalid content of the key: %d", *count_p, ik);
    
    v = hash_table_lookup  (ht, (const void *) key);
    
    test_assert (v == val, "(%d)lookuped value(%p) not the same to foreached value(%p)", *count_p, v, val);
}

static unsigned int my_hash (const void *key)
{
    return (unsigned int) *((const int *) key);
}

static int my_hash_equal (const void *a, const void *b)
{
    return *((const int *) a) == *((const int *) b);
}

int main (int   argc, char *argv[])
{
    HashTable *ht;
    void **params[2];
    int i, expected_nr_node;
    
    printf ("> hash_table_new and fill with testing data\n");
    ht = hash_table_new (my_hash, my_hash_equal, destroy_key, destroy_val);
    fill_hash_table_and_array (ht);
    test_assert (
            key_alloc == SAMPLE_LEN && val_alloc == SAMPLE_LEN,
            "A: unexpected alloc count(expected %d): key(%d), val(%d)", SAMPLE_LEN, key_alloc, val_alloc);
    
    printf (">> hash_table_foreach. verify each node\n");
    params[0] = (void *) ht;
    params[1] = (void *) 0;
    hash_table_foreach (ht, (HashTableVisitor) verify_each, params);
    test_assert (
            (int ) params[1] == SAMPLE_LEN,
            "unexpected visit count: %d (expect %d)", (int ) params[1], SAMPLE_LEN);
    
    printf (">>> hash_table_remove_all.\n");
    hash_table_remove_all (ht);
    test_assert (
            key_alloc == 0 && val_alloc == 0,
            "B: unexpected alloc count(expected %d): key(%d), val(%d)", 0, key_alloc, val_alloc);
    
    printf (">>>> fill with testing data and verify each node again.\n");
    fill_hash_table_and_array (ht);
    hash_table_foreach (ht, (HashTableVisitor) verify_each, params);
    
    test_assert (
            key_alloc ==  SAMPLE_LEN && val_alloc == SAMPLE_LEN,
            "C: unexpected alloc count(expected %d): key(%d), val(%d)", SAMPLE_LEN, key_alloc, val_alloc);
    
    printf (">>>>> remove each node.\n");
    expected_nr_node = key_alloc;
    for (i = 0; i < SAMPLE_LEN; i++) {
        if (!(i % 2)) {
            hash_table_remove (ht, array + i);
            expected_nr_node--;
            test_assert (
                    key_alloc == expected_nr_node && val_alloc == expected_nr_node,
                    "D: unexpected alloc count(expected %d): key(%d), val(%d)", expected_nr_node, key_alloc, val_alloc);
        }
    }
    
    printf (">>>>>> hash_table_destroy.\n");
    hash_table_destroy (ht);
    test_assert (
            key_alloc == 0 && val_alloc == 0,
            "E: unexpected alloc count(expected %d): key(%d), val(%d)", 0, key_alloc, val_alloc);
    
    return 0;

}
