/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Not MT safe
 */

/**
 * SECTION:function_class
 * @short_description: manipulates function classes of their implements
 *
 * Function Class:
 * <itemizedlist>
 *   <listitem>
 *     <para>Represents functions that have the same prototype & function</para>
 *   </listitem>
 *   <listitem>
 *     <para>Must have a reference implement</para>
 *   </listitem>
 *   <listitem>
 *     <para>Can add zero or more implement</para>
 *   </listitem>
 *   <listitem>
 *     <para>Has only one active implement, which can be used by the exported function pointer</para>
 *   </listitem>
 *   <listitem>
 *     <para>Can have an attachment, which will be used by oiltest</para>
 *   </listitem>
 * </itemizedlist>
 */
#include "hashtable.h"
#include "oilclass.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

typedef struct _OilImplement OilImplement;

struct _OilImplement {
    char *name;
    void *impl;
    unsigned int flags;
};

#define OIL_IMPL_EXTEND_CHUNK 3
#define OIL_ROOF(n) ((((n) + OIL_IMPL_EXTEND_CHUNK - 1) / OIL_IMPL_EXTEND_CHUNK) * OIL_IMPL_EXTEND_CHUNK)

struct _OilClass {
    char *name;
    void **pChosen;
    void *ref;

    int n_occupied;
    int n_alloc;
    int n_impls;
    OilImplement *active_impl;
    OilImplement *impls;

    void *data;
};

static OilImplement *_class_implements_foreach_internal (
        OilClass *cls,
        OilImplementVisitor visitor,
        void *user_data);
static int _cmp_impl_name (
        const char *name,
        const void *impl,
        unsigned int flag,
        void *user_data);
static void _class_clear_implements_internal (OilClass *cls);
static void _destroy_class (OilClass *cls);
static int _str_equal (const void *a, const void *b);
static uint32_t _str_hash (const void *v);
static void _class_vacumm_impl_array (OilClass *cls);
static void _class_resize_impl_array (OilClass *cls, int new_len);

static HashTable *oil_classes = NULL;
static DestroyNotify oil_class_data_destructor = NULL;

/**
 * oil_class_init:
 * @class_data_destructor: destructor of classes' attachments, can be NULL
 * 
 * Low-level function, initializes the oilcore module, @class_data_destructor is
 * a function pointer which tells oilcore how to destroy the attachment of a class. 
 */
void oil_class_init (void *class_data_destructor)
{
    oil_classes = hash_table_new (
            _str_hash,
            _str_equal,
            (DestroyNotify) free,
            (DestroyNotify) _destroy_class);

    if (!oil_classes) {
        fprintf (stderr, "oil_class_init: create internal hash table failed\n");
        abort ();
    }
    if (class_data_destructor)
        oil_class_data_destructor = (DestroyNotify) class_data_destructor;
}

/**
 * oil_class_uninit:
 *
 * Low-level function, free dynamtic alloced memory
 */
void oil_class_uninit ()
{
    hash_table_destroy (oil_classes);

    oil_class_data_destructor = NULL;
    oil_classes = NULL;
}

/**
 * oil_class_register:
 * @cls_name: shouldn't be NULL or empty string
 * @ref_func: a pointer, which point to the the reference implement, shouldn't be NULL
 * @export_func: address of a pointer that point to the active implement of a function class
 * 
 * Register a function class named @cls_name, with a reference implement @ref_func, and 
 * export the active function through @export_func. Its typical usage:
 * |[
 *   /&ast; declare a global function pointer variable &ast;/
 *   void (*foo) (...);
 *
 *   oil_class_register ("class_foo", foo_ref, &foo);
 * ]|
 * If a former function class with the same @cls_name exists, it will be replaced.
 *
 * Returns: #OilClass, an opaque C structure represents a function class
 */
OilClass *oil_class_register (
        char *cls_name,
        void *ref_func,
        void **export_func)
{
    OilClass *cls = NULL;
    char *dup_cls_name = NULL;

    if (!ref_func || !export_func || !cls_name || *cls_name == '\0')
        return NULL;

    dup_cls_name = strdup (cls_name);
    if (!dup_cls_name) {
        fprintf (stderr, "oil_cls_reg_name: duplicate cls_name failed\n");
        abort ();
    }
    cls = (OilClass *) malloc (sizeof (OilClass));

    *export_func = cls->ref = ref_func;
    cls->name = dup_cls_name;
    cls->pChosen = export_func;

    cls->n_alloc = 0;
    cls->n_impls = 0;
    cls->n_occupied = 0;
    cls->active_impl = NULL;
    cls->impls = NULL;

    cls->data = NULL;

    hash_table_insert (oil_classes, dup_cls_name, cls);

    return cls;
}

/**
 * oil_class_get:
 * @cls_name: an #OilClass
 *
 * Get C structure representation of a function class from its name
 *
 * Returns: #OilClass or NULL
 */
OilClass *oil_class_get (char *cls_name)
{
    OilClass *cls = NULL;

    if (cls_name)
        cls = hash_table_lookup (oil_classes, cls_name);

    return cls;
}

/**
 * oil_class_get_name:
 * @cls: an #OilClass
 *
 * Get the name of a function class
 *
 * Returns: a string. The string is used internally, so don't free.
 *          or NULL if @cls is NULL or corrupt
 */
char *oil_class_get_name (OilClass *cls)
{
    if (!cls)
        return NULL;

    return cls->name;
}

/**
 * oil_class_add_data:
 * @cls: an #OilClass
 * @cls_data: the data attached to @cls
 *
 * Attach data @cls_data to a function class @cls. If the @cls already has attachment,
 * the attachment will be freed by class_data_destructor (registered in oil_class_init())
 * and replaced by @cls_data.
 */
void oil_class_add_data (OilClass *cls, void *cls_data)
{
    if (!cls)
        return;
    
    if (oil_class_data_destructor)
        if (cls->data)
            oil_class_data_destructor (cls->data);
    
    cls->data = cls_data;
}

/**
 * oil_class_get_data:
 * @cls: an #OilClass
 * 
 * Get the attachment of the @cls.
 *
 * Returns: the attachment of the @cls,
 *          or NULL if @cls has no attachment or is NULL or corrupt
 */
void *oil_class_get_data (OilClass *cls)
{
    if (!cls)
        return NULL;

    return cls->data;
}

/**
 * oil_class_get_reference:
 * @cls: an #OilClass
 *
 * Get the reference implement of function class @cls
 *
 * Returns: a pointer point to the reference implement, or NULL, if @cls is NULL or corrupt
 */
void *oil_class_get_reference (OilClass *cls)
{
    if (!cls)
        return NULL;

    return cls->ref;
}

/**
 * oil_class_add_implement:
 * @cls: an #OilClass
 * @impl_name: name of the implement, should not be NULL
 * @func: the actual function of implement, should not be NULL
 * @flags: flags tells what features CPU must have to run the implement, see #OilCPUFlagBits
 *
 * Add an implement to function class @cls, with the name @impl_name. 
 * If an implement of the @cls with the same @impl_name already exists, it will be replaced.
 */
void oil_class_add_implement (
        OilClass *cls,
        char *impl_name,
        void *func,
        unsigned int flags)
{
    OilImplement *impl = NULL;

    if (!cls || !impl_name || !func)
        return;

    int i;
    for (i = 0; i < cls->n_occupied; i++) {
        OilImplement *j = cls->impls + i;
        
        if (j->name) {
            if (strcmp (impl_name, j->name) == 0) {
                impl = j;
                break;
            }
        } else if (!impl)
            impl = j;
    }

    if (impl) {
        if (i == cls->n_occupied)
            /* reuse a hole */
            cls->n_impls++;
        else
            /* replace an impl */
            free (impl->name);
        
    } else { /* no reuse, no replace (n_impls == n_occupied) */
        _class_resize_impl_array (cls, OIL_ROOF (cls->n_impls + 1));
        
        cls->n_impls++;
        cls->n_occupied++;
        
        impl = cls->impls + cls->n_impls - 1;
    }

    impl->name = strdup (impl_name);
    impl->impl = func;
    impl->flags = flags;
}

/**
 * oil_class_remove_implement:
 * @cls: #OilClass
 * @impl_name: name of the implement, should not be NULL
 *
 * remove implement @impl_name from function class @cls, rarely use
 * 
 * Returns: 1 on success, 0 on fail
 */
int oil_class_remove_implement (
        OilClass *cls,
        char *impl_name)
{
    OilImplement *impl = NULL;

    if (!cls || !impl_name)
        return 0;

    impl = _class_implements_foreach_internal (
            cls,
            (OilImplementVisitor) _cmp_impl_name,
            impl_name);

    if (impl) {
        free (impl->name);
        impl->name = NULL;
        cls->n_impls--;
        
        _class_resize_impl_array (cls, OIL_ROOF (cls->n_impls));
        
        return 1;
    }

    return 0;
}

/**
 * oil_class_foreach:
 * @visitor: the function to be called with each class
 * @user_data: data for @visitor
 *
 * For each class, calls @visitor, passes name of class, class itself and @user_data 
 * as the parameters
 */
void oil_class_foreach (OilClassVisitor visitor, void *user_data)
{
    if (!visitor) return;
    hash_table_foreach (oil_classes, (HashTableVisitor) visitor, user_data);
}

/**
 * oil_class_implements_foreach:
 * @cls: #OilClass
 * @visitor: the function to be called with each implement of @cls
 * @user_data: data for @visitor
 *
 * For each implement of function class @cls, call @visitor
 *
 * Returns: NULL or the name of implement that the visitor returns 1 and stops further visit
 */
char *oil_class_implements_foreach (
        OilClass *cls,
        OilImplementVisitor visitor,
        void *user_data)
{
    OilImplement *impl = NULL;

    if (!cls || !visitor)
        return NULL;

    impl = _class_implements_foreach_internal (cls, visitor, user_data);

    return impl ?  impl->name : NULL;
}

/**
 * oil_class_clear_implements:
 * @cls: #OilClass
 *
 * remove all implements of function class @cls
 *
 * Returns: 1 on success, 0 on fail
 */
int oil_class_clear_implements (OilClass *cls)
{
    if (!cls)
        return 0;

    _class_clear_implements_internal (cls);

    return 1;
}

/**
 * oil_class_activate_implement:
 * @cls: #OilClass
 * @impl_name: the name of the implement, NULL means active the reference implement
 *
 * Make @impl_name as the active implement of function class @cls.
 * If @impl_name is NULL, the reference implement is activated.
 * If no implement with the name @impl_name exists in function class @cls, do nothing
 */
void oil_class_activate_implement (OilClass *cls, char *impl_name)
{
    OilImplement *impl = NULL;

    if (!cls)
        return;

    if (!impl_name) {
        *cls->pChosen = cls->ref;
        cls->active_impl = NULL;
    } else {
        impl = _class_implements_foreach_internal (cls, (OilImplementVisitor) _cmp_impl_name, impl_name);

        if (impl) {
            *cls->pChosen = impl->impl;
            cls->active_impl = impl;
        }
    }
}

/**
 * oil_class_get_active_implement:
 * @cls: #OilClass
 *
 * Retrieve the active implement of function class @cls
 *
 * Returns: an internal string holds the name of the current active implement, 
 * or NULL, which means the reference implement is activated, or @cls is NULL or corrupt
 */
char *oil_class_get_active_implement (OilClass *cls)
{
    if (!cls)
        return NULL;

    return cls->active_impl ? cls->active_impl->name : NULL;
}

/**
 * oil_class_remove:
 * @cls_name: the name of the function class
 * 
 * Unregister the function class named @cls_name
 *
 * Returns: 1 on success, 0 on fail
 */
int oil_class_remove (char *cls_name)
{
    return hash_table_remove (oil_classes, cls_name);
}

/**
 * oil_class_remove_all:
 *
 * Unregister all function classes
 */
void oil_class_remove_all ()
{
    hash_table_remove_all (oil_classes);
}

static OilImplement *_class_implements_foreach_internal (
        OilClass *cls,
        OilImplementVisitor visitor,
        void *user_data)
{
    OilImplement *ret = NULL;
    int i;

    for (i = 0; i < cls->n_occupied; i++) {
        OilImplement *impl = cls->impls + i;

        if (impl->name)
            if (visitor (impl->name, impl->impl, impl->flags, user_data)) {
                ret = impl;
                break;
            }
    }

    return ret;
}

static void _class_clear_implements_internal (OilClass *cls)
{
    int i;

    for (i = 0; i < cls->n_occupied; i++) {
        OilImplement *impl = cls->impls + i;
        free (impl->name);
    }

    free (cls->impls);
    cls->impls = NULL;
    cls->n_alloc = 0;
    cls->n_occupied = 0;
    cls->n_impls = 0;
    cls->active_impl = NULL;
}

static void _destroy_class (OilClass *cls)
{
    _class_clear_implements_internal (cls);

    if (oil_class_data_destructor)
        if (cls->data)
            oil_class_data_destructor (cls->data);

    free (cls);
}

static int _cmp_impl_name (
        const char *name,
        const void *impl,
        unsigned int flag,
        void *user_data)
{
    char *to_cmp = (char *) user_data;

    if (!to_cmp) return 0;

    return strcmp (name, to_cmp) == 0;
}

static int _str_equal (const void *a, const void *b)
{
    const char *str1 = a;
    const char *str2 = b;

    return strcmp (str1, str2) == 0;
}

static uint32_t _str_hash (const void *v)
{
    /* 31 bit hash function */
    const signed char *p = v;
    uint32_t h = *p;

    if (h)
        for (p += 1; *p != '\0'; p++)
            h = (h << 5) - h + *p;

    return h;
}

static void _class_vacumm_impl_array (OilClass *cls)
{
    /* cls->n_occupied > cls->n_impls:
     * first try to remove the empty implements in the tail */
    int j;
    for (j = cls->n_occupied - 1; j >= 0; j--)
        if ((cls->impls + j)->name)
            break;

    if (j + 1 != cls->n_impls) {
        /* j + 1 > cls->n_impls: has holes, vacuum it */
        int i, w_idx;
        for (i = 0, w_idx = -1; i <= j; i++) {
            OilImplement *impl = cls->impls + i;

            if (!impl->name) {
                if (w_idx == -1)
                    /* first encounter a hole */
                    w_idx = i;
            } else if (w_idx != -1) {
                /* Behind a hole: move an non-empty element */
                cls->impls[w_idx] = *impl;
                w_idx++;
            }
        }
    }
    
    /* Here: all empty implements cleaned out */
    cls->n_occupied = cls->n_impls;
}

static void _class_resize_impl_array (OilClass *cls, int new_len)
{
    if (cls->n_alloc != new_len) {
        if (cls->n_occupied != cls->n_impls)
            _class_vacumm_impl_array (cls);
        
        cls->impls = realloc (cls->impls, new_len * sizeof (OilImplement));
        if (!cls->impls) {
            fprintf (stderr, "_class_resize_impl_array: realloc failed\n");
            abort ();
        }
        cls->n_alloc = new_len;
    }
}

