/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#ifndef __OIL_CLASS_H__
#define __OIL_CLASS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * OilClass:
 *
 * an opaque type represents a function class
 */
typedef struct _OilClass OilClass;

/**
 * OilImplementVisitor:
 * @impl_name: name of the implement
 * @func: the actual function of the implement
 * @flags: flags of the implement, see #OilCPUFlagBits
 * @user_data: user provides data
 *
 * see oil_class_implements_foreach()
 *
 * Returns: 0 for continuing visiting next implement, 1 for no further visit
 */
typedef int (*OilImplementVisitor) (
        const char *impl_name,
        const void *func,
        unsigned int flags,
        void *user_data);

/**
 * OilClassVisitor:
 * @cls_name: name of the function class
 * @cls: an #OilClass
 * @user_data: user provides data
 *
 * see oil_class_foreach()
 */
typedef void (*OilClassVisitor) (
        const char *cls_name,
        OilClass *cls,
        void *user_data);

void oil_class_init (void *class_data_destructor);
void oil_class_uninit ();

OilClass *oil_class_register (
        char *cls_name,
        void *ref_func,
        void **slot);
OilClass *oil_class_get (char *cls_name);
int oil_class_remove (char *cls_name);
void oil_class_remove_all ();

void oil_class_add_implement (
        OilClass *cls,
        char *impl_name,
        void *func,
        unsigned int flags);

void oil_class_foreach (OilClassVisitor visitor, void *user_data);

int oil_class_remove_implement (
        OilClass *cls,
        char *impl_name);
int oil_class_clear_implements (OilClass *cls);

char *oil_class_implements_foreach (
        OilClass *cls,
        OilImplementVisitor visitor,
        void *user_data);
void *oil_class_implements_get (
        OilClass *cls, 
        char *impl_name, 
        unsigned int *flags);

void oil_class_activate_implement (OilClass *cls, char *impl_name);
char *oil_class_get_active_implement (OilClass *cls);

void oil_class_add_data (OilClass *cls, void *cls_data);
char *oil_class_get_name (OilClass *cls);
void *oil_class_get_data (OilClass *cls);
void *oil_class_get_reference (OilClass *cls);

#ifdef __cplusplus
}
#endif

#endif /* __OIL_CLASS_H__ */

