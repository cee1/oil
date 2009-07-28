/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Modified from glib's hash table, 
 * Internal use, not MT safe
 */

#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _HashTable HashTable;
typedef void (*HashTableVisitor) (const void *key, void *val, void *user_data);
typedef unsigned int (*HashFunc) (const void *key);
typedef int (*EqualFunc) (const void *a, const void *b);
typedef void (*DestroyNotify) (void *data);

HashTable *hash_table_new (
        HashFunc hash_func, 
        EqualFunc key_equal_func,
        DestroyNotify key_destroy_func,
        DestroyNotify value_destroy_func);
void hash_table_insert (HashTable *ht, void *key, void *val);
void *hash_table_lookup (HashTable *ht, const void *key);
int hash_table_remove (HashTable *ht, const void *key);
void hash_table_remove_all (HashTable *ht);
void hash_table_destroy (HashTable *ht);
void hash_table_foreach (HashTable *ht, HashTableVisitor visitor, void * user_data);

#ifdef __cplusplus
}
#endif

#endif /* __HASH_TABLE_H__ */

