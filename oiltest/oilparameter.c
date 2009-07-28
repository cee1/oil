/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * not MT safe
 * functions for parse parameter spec string
 */

/**
 * SECTION:oilparameter
 * @short_description: parses parameter specification string, generates #OilParameter
 * @include: oil/oilparameter.h
 * @stability: Internal
 */
#include "oilparameter.h"
#include "oilutil.h"

#define MAX_PARAMS (OIL_ARG_LAST - 1)

static gboolean oil_param_parse (gchar *param_str, OilParameter *a_param);
static int _oilType_parse (gchar *c_type, gboolean is_pointer);
static int _arg_dir_parse (gchar *dir);
static void _lenspec_parse (gchar *lenspec, gint *var, glong *len);
static int _argType_parse (int dir, gboolean is_stride, gint index);
static gboolean _prototype_check_sanity (
        guint n_params,
        OilParameter *params);

/**
 * oil_type_sizeof:
 * @stability: Internal
 * @type: a #OilType
 *
 * Returns: size of @type, in bytes
 */
gint oil_type_sizeof (OilType type)
{
    switch (type) {
        case OIL_TYPE_UNKNOWN:
        case OIL_TYPE_INT:
            return 0;

        case OIL_TYPE_s8:
        case OIL_TYPE_u8:
        case OIL_TYPE_s8p:
        case OIL_TYPE_u8p:
            return 1;
    
        case OIL_TYPE_s16:
        case OIL_TYPE_u16:
        case OIL_TYPE_s16p:
        case OIL_TYPE_u16p:
            return 2;
    
        case OIL_TYPE_s32:
        case OIL_TYPE_u32:
        case OIL_TYPE_f32:
        case OIL_TYPE_s32p:
        case OIL_TYPE_u32p:
        case OIL_TYPE_f32p:
            return 4;
    
        case OIL_TYPE_s64:
        case OIL_TYPE_u64:
        case OIL_TYPE_f64:
        case OIL_TYPE_s64p:
        case OIL_TYPE_u64p:
        case OIL_TYPE_f64p:
            return 8;
    }
    
    return 0;
}

/**
 * oil_prototype_parse:
 * @prototype: a string describe the constrain of the prototype
 * @n_params: out argument, length of the @params
 * @params: out argument, an array of #OilParameter
 *
 * Parses a comma separate parameters, convert them to #OilParameter array representation.
 *
 * Returns: TRUE on success, FALSE on fail
 */
gboolean oil_prototype_parse (
        gchar *prototype, 
        guint *n_params,
        OilParameter **params)
{
    GArray *tmp_params = NULL;
    gchar **param_strv = NULL;
    gint i;
    
    g_return_val_if_fail (prototype && *prototype && 
                    params && n_params, FALSE);

    param_strv = g_strsplit (prototype, ",", MAX_PARAMS);
    tmp_params = g_array_new (FALSE, TRUE, sizeof (OilParameter));
    
    for (i = 0; ;i++) {
        OilParameter tmp_param = {0,};
        gchar *param_str;
        
        if (!param_strv[i])
            break;
        param_str = g_strstrip (param_strv[i]);

        if (oil_param_parse (param_str, &tmp_param))
            g_array_append_val (tmp_params, tmp_param);
        else
            goto bad_quit;
    }
    
    if (!_prototype_check_sanity (tmp_params->len, (OilParameter *) tmp_params->data)) {
        g_warning ("prototype failed sanity check");
        goto bad_quit;
    }
    
    /* parse all parameters successfully */
    g_strfreev (param_strv);
    *params = (OilParameter *) tmp_params->data;
    *n_params = tmp_params->len;
    g_array_free (tmp_params, FALSE);
    
    return TRUE;

bad_quit:
    g_array_free (tmp_params, TRUE);
    g_strfreev (param_strv);
    
    return FALSE;
}

/* 
 * regular expression for parameter 
 * support:
 *   1) pointer at most 1 level
 *   2) no qualified Type, e.g. unsigned int, the only exception is const
 */

#define SEP "\\s+"
#define PAD "\\s*+"
#define C_WORD "[_a-zA-Z][_a-zA-Z0-9]*+"

#define TYPE "(?:(?P<type>"C_WORD ")|(?:const" SEP ")(?P<type>" C_WORD ")|(?P<type>" C_WORD ")(?:" SEP "const))"
#define STARSEP "(?:" PAD "(?P<star>\\*)" PAD "|" SEP ")"

#define ARG_NM "(?P<nm>[nm])"
#define ARG_STRIDE "(?P<stride>(?P<arg_class>[isd])s(?P<index>\\d?))"
#define LEN_SPEC "\\d+|[nm](p\\d+)?"
#define ARG_VECTOR "(?P<array>(?P<arg_class>[isd])(?P<index>\\d?)"\
"(_(?P<len1>"LEN_SPEC")(x(?P<len2>"LEN_SPEC"))?)?)"

#define ARG "("ARG_NM"|"ARG_STRIDE"|"ARG_VECTOR")"
#define PARAM "^" PAD TYPE STARSEP ARG PAD "$"

static gboolean oil_param_parse (gchar *param_str, OilParameter *a_param)
{
    static GRegex *param_pattern = NULL;
    GMatchInfo *param_info = NULL;
    gboolean ret = FALSE;
    gchar *ptr = NULL, *c_type = NULL, *is_pointer = NULL;
    
    /* init: compile regular expression */
    if (!param_pattern) {
        GError *err = NULL;
        param_pattern = g_regex_new (PARAM, 
                        G_REGEX_NO_AUTO_CAPTURE | G_REGEX_OPTIMIZE | G_REGEX_DUPNAMES,
                        0, &err);
        if (err) oil_error_gerror (err);
    }
    
    if (!g_regex_match (param_pattern, param_str, 0, &param_info)) {
            g_warning ("Bad parameter: \"%s\"", param_str);
            goto out;
    }
    
    c_type = g_match_info_fetch_named (param_info, "type");
    is_pointer = g_match_info_fetch_named (param_info, "star");
    a_param->c_type = _oilType_parse (c_type, *is_pointer != '\0');
    if (a_param->c_type == OIL_TYPE_UNKNOWN) {
        g_warning ("When parsing parameter \"%s\", not support C type \"%s\"", param_str, c_type);
        goto out;
    }
    
    ptr = g_match_info_fetch_named (param_info, "nm");
    if (*ptr != '\0') {
        /* scale parameter: n、m */
        /* a_param->arg_class = OIL_ARG_T_SCALE; */
        a_param->index = 0;
        a_param->is_pointer = FALSE;
        a_param->is_stride = FALSE;
        
        g_free (ptr);
    } else {
        g_free (ptr);
        ptr = g_match_info_fetch_named (param_info, "stride");
        
        if (*ptr != '\0') {
            /* stride */
            gchar *arg_class, *index;
            g_free (ptr);
            
            a_param->is_stride = TRUE;
            a_param->is_pointer = FALSE;
            
            arg_class = g_match_info_fetch_named (param_info, "arg_class");
            index = g_match_info_fetch_named (param_info, "index");
            
            a_param->arg_class = _arg_dir_parse (arg_class);
            a_param->index = (*index == '\0') ? 1 : (*index - '0');
            
            g_free (arg_class);
            g_free (index);
        } else {
            /* array */
            gchar *arg_class, *index, *len1, *len2;
            g_free (ptr);
            
            a_param->is_stride = FALSE;
            a_param->is_pointer = TRUE;
            
            arg_class = g_match_info_fetch_named (param_info, "arg_class");
            index = g_match_info_fetch_named (param_info, "index");
            len1 = g_match_info_fetch_named (param_info, "len1");
            len2 = g_match_info_fetch_named (param_info, "len2");

            a_param->arg_class = _arg_dir_parse (arg_class);
            a_param->index = (*index == '\0') ? 1 : (*index - '0');
            
            /* 
             * g_match_info_fetch_named may return NULL, which should be a bug 
             */
            if (!len1 || *len1 == '\0') {
                /* no len spec */
                a_param->prestride_var = 0;
                a_param->poststride_var = 1;
                a_param->prestride_length = 1;
                a_param->poststride_length = 0;
            } else if (!len2 || *len2 == '\0') {
                /* only len spec1 */
                a_param->prestride_var = 0;
                a_param->prestride_length = 1;
                
                _lenspec_parse (len1, 
                    &a_param->poststride_var, 
                    &a_param->poststride_length);
            } else {
                _lenspec_parse (len1, 
                    &a_param->prestride_var, 
                    &a_param->prestride_length);
                
                _lenspec_parse (len2, 
                    &a_param->poststride_var, 
                    &a_param->poststride_length);
            }
            
            g_free (arg_class);
            g_free (index);
            g_free (len1);
            g_free (len2);
        }
    }
    a_param->arg_type = _argType_parse (
            a_param->arg_class, 
            a_param->is_stride, 
            a_param->index);
    
    if (a_param->arg_type == OIL_ARG_UNKNOWN) {
        g_warning ("When parsing parameter \"%s\", unknown OilArgType", param_str);
        goto out;
    }
    
    /* here, parse successfully */
    a_param->parameter = g_strdup (param_str);
    ret = TRUE;

out:
    if (param_info) g_match_info_free (param_info);
    g_free (c_type);
    g_free (is_pointer);
    
    return ret;
}

static int _oilType_parse (gchar *c_type, gboolean is_pointer)
{
    /* the same order to enum in OilType */
    static const gchar *oil_c_type_names[] = {
        "int8_t",
        "uint8_t",
        "int16_t",
        "uint16_t",
        "int32_t",
        "uint32_t",
        "int64_t",
        "uint64_t",
        "float",
        "double"
    };
    int i;
    
    if (is_pointer) {
        for (i = 0; i < sizeof (oil_c_type_names) / sizeof (gchar *); i++) {
            if (strcmp (c_type, oil_c_type_names[i]) == 0)
                return OIL_TYPE_s8p + i;
        }
    }else {
        if (strcmp (c_type, "int") == 0)
            return OIL_TYPE_INT;

        for (i = 0; i < sizeof (oil_c_type_names) / sizeof (gchar *); i++) {
                if (strcmp (c_type, oil_c_type_names[i]) == 0)
                    return OIL_TYPE_s8 + i;
        }
    }
    return OIL_TYPE_UNKNOWN;
}

static int _arg_dir_parse (gchar *dir)
{
    int ret = -1;
    
    switch (*dir) {
        case 'i':
            ret = OIL_ARG_T_INPLACE;
            break;
        
        case 's':
            ret = OIL_ARG_T_SRC;
            break;
        
        case 'd':
            ret = OIL_ARG_T_DEST;
            break;
        
        default:
            g_error ("Unknown Arg arg_class, the regular pattern of parameter is bad");
            /* should abort */
            break;
    }
    
    return ret;
}

static void _lenspec_parse (gchar *lenspec, gint *var, glong *len)
{
    switch (*lenspec) {
        case 'n':
            *var = 1;
            *len = (*(lenspec + 1) == '\0') ? 0 : g_ascii_strtoll (lenspec + 2, NULL, 0);
            break;
        
        case 'm':
            *var = 2;
            *len = (*(lenspec + 1) == '\0') ? 0 : g_ascii_strtoll (lenspec + 2, NULL, 0);
            break;
        
        default:
            *var = 0;
            *len = g_ascii_strtoll (lenspec, NULL, 0);
            break;
    }
}

static int _argType_parse (int dir, gboolean is_stride, gint index)
{
    struct argTypeTriple {
        OilArgType type;
        int arg_class;
        gboolean is_stride;
        int index;
    };
    static const struct argTypeTriple _map[] = {
        { OIL_ARG_N,        OIL_ARG_T_SCALE,    FALSE, 0 },
        { OIL_ARG_M,        OIL_ARG_T_SCALE,    FALSE, 0 },
        { OIL_ARG_DEST1,    OIL_ARG_T_DEST,     FALSE, 1 },
        { OIL_ARG_DSTR1,    OIL_ARG_T_DEST,     TRUE,  1 },
        { OIL_ARG_DEST2,    OIL_ARG_T_DEST,     FALSE, 2 },
        { OIL_ARG_DSTR2,    OIL_ARG_T_DEST,     TRUE,  2 },
        { OIL_ARG_DEST3,    OIL_ARG_T_DEST,     FALSE, 3 },
        { OIL_ARG_DSTR3,    OIL_ARG_T_DEST,     TRUE,  3 },
        { OIL_ARG_SRC1,     OIL_ARG_T_SRC,      FALSE, 1 },
        { OIL_ARG_SSTR1,    OIL_ARG_T_SRC,      TRUE,  1 },
        { OIL_ARG_SRC2,     OIL_ARG_T_SRC,      FALSE, 2 },
        { OIL_ARG_SSTR2,    OIL_ARG_T_SRC,      TRUE,  2 },
        { OIL_ARG_SRC3,     OIL_ARG_T_SRC,      FALSE, 3 },
        { OIL_ARG_SSTR3,    OIL_ARG_T_SRC,      TRUE,  3 },
        { OIL_ARG_SRC4,     OIL_ARG_T_SRC,      FALSE, 4 },
        { OIL_ARG_SSTR4,    OIL_ARG_T_SRC,      TRUE,  4 },
        { OIL_ARG_SRC5,     OIL_ARG_T_SRC,      FALSE, 5 },
        { OIL_ARG_SSTR5,    OIL_ARG_T_SRC,      TRUE,  5 },
        { OIL_ARG_INPLACE1, OIL_ARG_T_INPLACE,  FALSE, 1 },
        { OIL_ARG_ISTR1,    OIL_ARG_T_INPLACE,  TRUE,  1 },
        { OIL_ARG_INPLACE2, OIL_ARG_T_INPLACE,  FALSE, 2 },
        { OIL_ARG_ISTR2,    OIL_ARG_T_INPLACE,  TRUE,  2 },
    };
    int i, ret = OIL_ARG_UNKNOWN;
    
    for ( i = 0; i < sizeof (_map) / sizeof (struct argTypeTriple); i++) {
        if (dir == _map[i].arg_class &&
                is_stride == _map[i].is_stride &&
                index == _map[i].index) {
            ret = _map[i].type;
            break;
        }
    }
    return ret;
}

static gboolean _prototype_check_sanity (
        guint n_params,
        OilParameter *params)
{
    int i;
    gboolean has_n;

    for (i = 0, has_n = FALSE; i<n_params; i++) {
        OilParameter *p = params + i;
        if (p->arg_type == OIL_ARG_N) {
            has_n =  TRUE;
            break;
        }
    }

  if (!has_n) {
        /* check that no parameter depends on n */
        for(i = 0; i < n_params; i++) {
            OilParameter *p = params + i;
            
            if (p->is_stride) continue;
            if (p->arg_type == OIL_ARG_N) continue;
            if (p->arg_type == OIL_ARG_M) continue;

            if (p->prestride_var == 1) return FALSE;
            if (p->poststride_var == 1) return FALSE;
        }
    }
    return TRUE;
}

