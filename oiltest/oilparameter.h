/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * not MT safe
 * internal functions
 */

#ifndef __OIL_PARAMETER_H__
#define __OIL_PARAMETER_H__
#include <glib.h>

G_BEGIN_DECLS

typedef struct _OilParameter OilParameter; 

/**
 * OilArgClassType:
 * @OIL_ARG_T_SCALE: related to scale
 * @OIL_ARG_T_SRC: related to source operation
 * @OIL_ARG_T_DEST: related to destination operation
 * @OIL_ARG_T_INPLACE: related to inplace operation
 *
 * Currently, oil classifies arguments to four categories: source, destination, inplace
 * and scale
 */
typedef enum {
    OIL_ARG_T_SCALE = 0,
    OIL_ARG_T_SRC,
    OIL_ARG_T_DEST,
    OIL_ARG_T_INPLACE,
} OilArgClassType;

typedef enum {
    OIL_TYPE_UNKNOWN = 0,
    OIL_TYPE_INT,
    OIL_TYPE_s8,
    OIL_TYPE_u8,
    OIL_TYPE_s16,
    OIL_TYPE_u16,
    OIL_TYPE_s32,
    OIL_TYPE_u32,
    OIL_TYPE_s64,
    OIL_TYPE_u64,
    OIL_TYPE_f32,
    OIL_TYPE_f64,
    OIL_TYPE_s8p,
    OIL_TYPE_u8p,
    OIL_TYPE_s16p,
    OIL_TYPE_u16p,
    OIL_TYPE_s32p,
    OIL_TYPE_u32p,
    OIL_TYPE_s64p,
    OIL_TYPE_u64p,
    OIL_TYPE_f32p,
    OIL_TYPE_f64p,
} OilType;

/**
 * OilGenericType:
 *
 * Generic type, represents all C primary types.
 * <note>
 * <para>
 * Currently oil only accesses #OilGenericType as a pointer, it may change in the future, 
 * since we have defined OIL_TYPE_s64 OIL_TYPE_u64 and OIL_TYPE_f64.
 * </para>
 * </note>
 * <variablelist role="struct">
 * <varlistentry>
 * <term><link linkend="gpointer">gpointer</link>&nbsp;<structfield>p</structfield>;</term>
 * <listitem><simpara>accesses as an pointer (or 8-32bit integers, in most platforms)</simpara></listitem>
 * </varlistentry>
 * <varlistentry>
 * <term><link linkend="guint64">guint64</link>&nbsp;<structfield>i64</structfield>;</term>
 * <listitem><simpara>accesses as a 64-bit integer</simpara></listitem>
 * </varlistentry>
 * <varlistentry>
 * <term><link linkend="gdouble">gdouble</link>&nbsp;<structfield>f64</structfield>;</term>
 * <listitem><simpara>accesses as a 32-bit float or a 64-bit double</simpara></listitem>
 * </varlistentry>
 * </variablelist>
 */
typedef union {
    gpointer p;
    guint64 i64;
    gdouble f64;
} OilGenericType;

typedef enum {
    OIL_ARG_UNKNOWN = 0,
    OIL_ARG_N,
    OIL_ARG_M,
    OIL_ARG_DEST1,
    OIL_ARG_DSTR1,
    OIL_ARG_DEST2,
    OIL_ARG_DSTR2,
    OIL_ARG_DEST3,
    OIL_ARG_DSTR3,
    OIL_ARG_SRC1,
    OIL_ARG_SSTR1,
    OIL_ARG_SRC2,
    OIL_ARG_SSTR2,
    OIL_ARG_SRC3,
    OIL_ARG_SSTR3,
    OIL_ARG_SRC4,
    OIL_ARG_SSTR4,
    OIL_ARG_SRC5,
    OIL_ARG_SSTR5,
    OIL_ARG_INPLACE1,
    OIL_ARG_ISTR1,
    OIL_ARG_INPLACE2,
    OIL_ARG_ISTR2,

    OIL_ARG_LAST
} OilArgType;

/**
 * OilParameter:
 * @parameter: the string representation of the parameter
 * @c_type: the #OilType corresponding to C type
 * @arg_class: which class dose the argument belong to? see #OilArgClassType
 * @is_pointer: is the argument a pointer?
 * @is_stride: is the argument a stride?
 * @index: 
 * @prestride_var:
 * @poststride_var:
 * @prestride_length:
 * @poststride_length:
 * @value: only has meaning for stride and scale type argument
 * @header: only has meaning for pointer, pad in the start
 * @footer: only has meaning for pointer, pad in the end
 * @pre_n: only has meaning for pointer
 * @post_n: only has meaning for pointer
 * @stride: only has meaning for pointer
 * @size: only has meaning for pointer, the actual size of the array
 * @guard: only has meaning for pointer, memory guard for overflow detection
 */
struct _OilParameter {
    gchar *parameter;
    OilType c_type;
    OilArgClassType arg_class;
    OilArgType arg_type;
    
    gboolean is_pointer;
    gboolean is_stride;
    
    gint index;
    gint prestride_var;
    gint poststride_var;
    glong prestride_length;
    glong poststride_length;
    
    /* for stride & scale parameter */
    OilGenericType value;
    
    /* for array (pointer) */
    gint header;
    gint footer;
    gint pre_n;
    gint post_n;
    gint stride;
    gint size;
    gint8 guard;
};

gint oil_type_sizeof (OilType type);
gboolean oil_prototype_parse (
        gchar *prototype, 
        guint *n_params,
        OilParameter **params);

G_END_DECLS

#endif /* __OIL_PARAMETER_H__ */
