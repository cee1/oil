/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include "../oilclass.c"
#include "test_common.h"

#define CLASS_DATA_SIZE 64
#define NR_CLASSES 10000
#define NR_IMPL_PRE_CLASS 10000
#define NR_IMPL_MORE 1000

static int cls_data_alloc;
static void *public_funcs[NR_CLASSES];

void destroy_cls_data (void *cls_data)
{
    test_assert (cls_data, "cls_data is NULL");
    cls_data_alloc--;
    free (cls_data);
}

static unsigned int _get_order_base10 (unsigned int n)
{
    unsigned int ord, tmp;
    
    for (ord = 0, tmp = 1; tmp <= NR_CLASSES; ord++)
        tmp *= 10;
    
    return ord - 1;
}

static void fill_classes ()
{
    char cls_prefix[] = "class";
    unsigned int i, ord;
    
    ord = _get_order_base10 (NR_CLASSES) + 1;
    
    for (i = 0; i < NR_CLASSES; i++) {
        OilClass *cls;
        char cls_name[5 + ord + 1];
        char *data = malloc (sizeof ("Hello world") + ord);

        snprintf (cls_name, 5 + ord + 1, "Class%d", i + 1);
        cls = oil_class_register (cls_name, (void *) (i + 1), public_funcs + i);
        
        snprintf (data, sizeof ("Hello world") + ord, "Hello world%d", i + 1);
        oil_class_add_data (cls, data);
        cls_data_alloc++;
    }
}

static void verify_each_implement (
        const char *impl_name,
        void *func,
        unsigned int flags,
        unsigned int *counter)
{
    unsigned int ord = _get_order_base10 (flags) + 1;
    char in[4 + ord + 1];
    
    *counter += 1;
    
    test_assert (
            impl_name && func,
            "(%d)impl_foreach, one of passed parameters is NULL (impl_name: \"%s\", func: %p)",
            *counter, impl_name, func);

    snprintf (in, 4 + ord + 1, "Impl%d", flags);
    test_assert (
            strcmp (in, impl_name) == 0,
            "(%d)impl_foreach, composed name(\"%s\") not equal to impl_name(\"%s\")",
            *counter, in, impl_name);
    
    test_assert (
            flags == (unsigned int) func,
            "(%d \"%s\")impl_foreach, unexpected result flags(%d) != func(%d)",
            *counter, impl_name, flags, (unsigned int) func);
}

static void verify_each_class (
        const char *cls_name,
        OilClass *cls,
        unsigned int *counter_p)
{
    OilClass *c;
    
    *counter_p += 1;
    
    test_assert (
            cls_name && cls,
            "(%d)class_foreach, one of passed parameters is NULL (cls_name: \"%s\", cls: %p)",
            *counter_p, cls_name, cls);
    
    c = oil_class_get ((char *) cls_name);
    test_assert (
            c == cls,
            "(%d)get class by name(\"%s\", %p) not the same to that passed in (%p)",
            *counter_p, cls_name, c, cls);

    {
        unsigned int f = (unsigned int) oil_class_get_reference (cls);
        unsigned int ord = _get_order_base10 (NR_CLASSES) + 1;
        char hello[sizeof ("Hello world") + ord];
        char cn[5 + ord + 1];
        char *data;
        
        test_assert (
                test_in_range (f, 0, NR_CLASSES + 1),
                "(%d \"%s\")corrupted ref field(%d)",
                *counter_p, cls_name, f);
        
        snprintf (cn, 5 + ord + 1, "Class%d", f);
        snprintf (hello, sizeof ("Hello world") + ord, "Hello world%d", f);
        
        test_assert (
                strcmp (cn, cls_name) == 0,
                "(%d)unexpected: re-composed class_name %s != %s",
                *counter_p, cn, cls_name);
        
        data = oil_class_get_data (cls);
        test_assert (
                strcmp (data, hello) == 0,
                "(%d)unexpected class data(\"%s\", expected \"%s\")",
                *counter_p, data, hello);
       
       if (f % 3) {
            /* delete attached data */
            int alloc_count = cls_data_alloc;

            oil_class_add_data (cls, NULL);
            test_assert (
                    alloc_count == cls_data_alloc + 1,
                    "(%d)unexpected: cls_data_alloc number(%d) should decrease by 1",
                    *counter_p, cls_data_alloc);
            
            test_assert (
                oil_class_get_data (cls) == NULL,
                "(%d \"%s\")Attached class_data is not as expected(should be NULL)",
                *counter_p, cls_name);
        }
    }
    
    test_assert (
                oil_class_get_active_implement (cls) == NULL,
                "(%d \"%s\")Actived implement is not as expected(should be NULL)",
                *counter_p, cls_name);
    
    {
        unsigned int counter = 0;
        oil_class_implements_foreach (
                cls, 
                (OilImplementVisitor) verify_each_implement,
                (void *) &counter);
        test_assert (
                counter == 0,
                "(%d \"%s\")unexpected nr_impls(%d), should be zero",
                *counter_p, cls_name, counter);
    }
}

static void verify_class1 (OilClass *cls)
{
    int i, j;
    unsigned int ord = _get_order_base10 (NR_IMPL_PRE_CLASS) + 1;
    char impl_name[4 + ord + 1];
    unsigned int counter = 0;
    
    for (i = 0; i < NR_IMPL_PRE_CLASS; i++) {
        snprintf (impl_name, 4 + ord + 1, "Impl%d", i + 1);
        oil_class_add_implement (cls, impl_name, (void *) i + 1, i + 1);
        
        test_assert (
            cls->n_impls == cls->n_occupied,
            "(%d)Add implements linearly, but n_impls(%d) != n_occupied(%d)",
            i, cls->n_impls, cls->n_occupied);
        
        test_assert (
            cls->n_alloc >= cls->n_impls && (cls->n_alloc - cls->n_impls) < OIL_IMPL_EXTEND_CHUNK,
            "(%d)A n_alloc(%d) is not in the range [n_impls(%d), n_impls + OIL_IMPL_EXTEND_CHUNK)",
            i, cls->n_alloc, cls->n_impls);
        
        test_assert (
            cls->n_alloc % OIL_IMPL_EXTEND_CHUNK == 0,
            "(%d)A n_alloc(%d) can't divide \"%d\" exactly ",
            i, cls->n_alloc, OIL_IMPL_EXTEND_CHUNK);
    }
    
    test_assert (
            cls->n_impls == cls->n_occupied,
            "Have added %d implements linearly, but n_impls(%d) != n_occupied(%d)",
            NR_IMPL_PRE_CLASS, cls->n_impls, cls->n_occupied);
        
    test_assert (
            cls->n_impls == NR_IMPL_PRE_CLASS,
            "Have added %d implements linearly, but n_impls is %d",
            NR_IMPL_PRE_CLASS, cls->n_impls);
    
    test_assert (
            cls->n_alloc >= cls->n_impls && (cls->n_alloc - cls->n_impls) < OIL_IMPL_EXTEND_CHUNK,
            "Have added %d implements linearly, n_alloc(%d) is not in the range [n_impls(%d), n_impls + OIL_IMPL_EXTEND_CHUNK)",
            NR_IMPL_PRE_CLASS, cls->n_alloc, cls->n_impls);
    
    test_assert (
            cls->n_alloc % OIL_IMPL_EXTEND_CHUNK == 0,
            "Have added %d implements linearly, n_alloc(%d) can't divide \"%d\" exactly ",
            NR_IMPL_PRE_CLASS, cls->n_alloc, OIL_IMPL_EXTEND_CHUNK);
    
    for (i = 0; i < NR_IMPL_PRE_CLASS; i++) {
        if (!(i % 4) || (!(i % 5))) {
            snprintf (impl_name, 4 + ord + 1, "Impl%d", i + 1);
            oil_class_remove_implement (cls, impl_name);
        } else if (!(i % 7)) {
            /* i >= 7, i = 0 won't reach here */
            j = i / 4 * 4 ;
            snprintf (impl_name, 4 + ord + 1, "Impl%d", j + 1);
            oil_class_add_implement (cls, impl_name, (void *) j + 1, j + 1);
        }else {
            char *tmp;
            
            snprintf (impl_name, 4 + ord + 1, "Impl%d", i + 1);
            oil_class_active_implement (cls, impl_name);
            
            tmp = oil_class_get_active_implement (cls);
            test_assert (
                    tmp && strcmp (tmp, impl_name) == 0,
                    "(%d)active impl \"%s\", but told \"%s\" is actived",
                    i, impl_name, tmp);
        }
        test_assert (
                cls->n_alloc >= cls->n_occupied && cls->n_occupied >= cls->n_impls,
                "(%d)Not meet: n_alloc(%d) > n_occupied(%d) > n_impls(%d)",
                i, cls->n_alloc, cls->n_occupied, cls->n_impls);
        test_assert (
                (cls->n_alloc - cls->n_impls) < OIL_IMPL_EXTEND_CHUNK,
                "(%d)B n_alloc(%d) is not in range [n_impls(%d), n_impls + OIL_IMPL_EXTEND_CHUNK)",
                i, cls->n_alloc, cls->n_impls);
        test_assert (
                cls->n_alloc % OIL_IMPL_EXTEND_CHUNK == 0,
                "(%d)B n_alloc(%d) can't divide \"%d\" exactly ",
                i, cls->n_alloc, OIL_IMPL_EXTEND_CHUNK);
    }
    
    oil_class_implements_foreach (
                cls, 
                (OilImplementVisitor) verify_each_implement,
                &counter);
}

int main ()
{
    unsigned int counter = 0;
    
    printf ("> oil_class_init and fill with testing classes.\n");
    oil_class_init (*destroy_cls_data);
    fill_classes ();
    test_assert (
            cls_data_alloc == NR_CLASSES,
            "The number of alloced cls_data is %d, expect %d",
            cls_data_alloc, NR_CLASSES);
    
    oil_class_foreach ((OilClassVisitor) verify_each_class, &counter);
    test_assert (
            counter == NR_CLASSES,
            "The number of visited classes is %d, expect %d",
            counter, NR_CLASSES);
    
    printf (">> testing implement adding & removing.\n");
    verify_class1 (oil_class_get ("Class1"));
    
    printf (">>> oil_class_uninit.\n");
    oil_class_uninit ();
    test_assert (cls_data_alloc == 0, "cls_data_alloc(%d) is not zero", cls_data_alloc);
    
    return 0;
}
