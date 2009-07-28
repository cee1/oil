/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
 
#include "oilchecker.h"
#include "oilutil.h"
#include "oiltestconfig.h"
#include <inttypes.h>

OilChecker oil_checker_default = {
    .init = oil_checker_init,
    .generate_sample = oil_checker_sample_generator,
    .check_result = oil_checker_check_result,
    .uninit = oil_checker_uninit,
    .data = NULL,
};

static double _check_array (
        guint8 *data,
        guint8 *ref,
        OilType type,
        gint pre_n,
        gint stride,
        gint post_n);

struct _checker_data {
    guint n_params;
    OilParameter *params;
    gdouble tolerance;
};

void oil_checker_init (
        OilChecker *checker, 
        guint n_params, 
        OilParameter *params)
{
    struct _checker_data *data = g_try_new0 (struct _checker_data, 1);
    
    data->n_params = n_params;
    data->params = params;
    
    checker->data = data;
}

void oil_checker_uninit (OilChecker *checker)
{
    struct _checker_data *data = checker->data;
    
    checker->data = NULL;
    g_free (data);
}

void oil_checker_sample_generator (
        OilChecker *checker,
        OilGenericType *src)
{
    struct _checker_data *data = checker->data;
    
    guint n_params = data->n_params;
    OilParameter *params = data->params;

    guint i;
    gchar *seed;
    gboolean low_tolerance = TRUE;
    
    /* emit log about the seed */
    oil_test_config_get (1, OIL_CONF_OPT_SEED, &seed);
    oil_test_log_printf ("Seed: %s", seed);
    
    for (i = 0; i < n_params; i++) {
        OilParameter *p = params + i;
        
        if (p->is_pointer) {
            guint8 *buffer = (guint8 *) (src + i)->p;
            
            if (p->c_type == OIL_TYPE_f64p && 
                p->c_type == OIL_TYPE_f32p)
                low_tolerance = FALSE;
            
            if (p->arg_class == OIL_ARG_T_SRC ||
                    p->arg_class == OIL_ARG_T_INPLACE) {
                oil_randize_array (
                        oil_test_config_rand (),
                        buffer,
                        p->c_type,
                        p->pre_n,
                        p->stride,
                        p->post_n);
            }
            oil_test_log_printf ("Sample_array: 0x%x", buffer);
        }
        /* emit log about the parameter */
        oil_test_log_parameter (p);
    }

    if (low_tolerance)
        data->tolerance = oil_test_config_tolerance_low ();
    else
        data->tolerance = oil_test_config_tolerance_high ();
}

gboolean oil_checker_check_result (
        OilChecker *checker,
        const OilGenericType *ref,
        OilGenericType *tdata)
{
    struct _checker_data *data = checker->data;
    
    guint n_params = data->n_params;
    OilParameter *params = data->params;
    
    gint i, n = 0;
    gdouble diff = 0.0;
    
    for (i = 0;  i < n_params; i++) {
        OilParameter *p = params + i;

        if (p->is_pointer) {
            if (p->arg_class == OIL_ARG_T_DEST ||
                    p->arg_class == OIL_ARG_T_INPLACE) {
                diff += _check_array (
                        (guint8 *) (ref + i)->p,
                        (guint8 *) (tdata + i)->p,
                        p->c_type,
                        p->pre_n,
                        p->stride,
                        p->post_n);
            }
            n += p->pre_n * p->post_n;
        }
    }
    oil_test_log_printf ("Deviation: %g for %d samples", diff, n);
    
    if (diff > data->tolerance * n) {
        oil_test_log_printf (
                "Critical: Deviation %g > %g * %d",
                diff,
                data->tolerance,
                n);
                
        return FALSE;
    }
    
    return TRUE;
}

static double _check_array (
        guint8 *data,
        guint8 *ref,
        OilType c_type,
        gint pre_n,
        gint stride,
        gint post_n)
{
    gdouble diff = 0.0;

#define OFFSET_GET(ptr, offset, type) (*(type *) ((guint8 *) (ptr) + (offset)))
#define CHECK(type) G_STMT_START {                                      \
    gint i, j;                                                          \
    gint s = oil_type_sizeof (c_type);                                  \
    for (i = 0; i < post_n; i++) {                                      \
        for (j = 0; j < pre_n; j++) {                                   \
            gint offset = i * stride + j * s;                           \
            diff += ABS ((gdouble) OFFSET_GET (data, offset, type) -    \
                    (gdouble) OFFSET_GET (ref, offset, type));          \
        }                                                               \
    }                                                                   \
} G_STMT_END

    switch (c_type) {
        case OIL_TYPE_s8p:
            CHECK (int8_t);
            break;
        case OIL_TYPE_u8p:
            CHECK (uint8_t);
            break;
        case OIL_TYPE_s16p:
            CHECK (int16_t);
            break;
        case OIL_TYPE_u16p:
            CHECK (uint16_t);
            break;
        case OIL_TYPE_s32p:
            CHECK (int32_t);
            break;
        case OIL_TYPE_u32p:
            CHECK (uint32_t);
            break;
        case OIL_TYPE_s64p:
            CHECK (int64_t);
            break;
        case OIL_TYPE_u64p:
            CHECK (uint64_t);
            break;
        case OIL_TYPE_f32p:
            CHECK (float);
            break;
        case OIL_TYPE_f64p:
            CHECK (double);
            break;
        default:
            g_error ("Unknow OilType %d", c_type);
            /* should abort*/
            break;
    }

#undef CHECK
#undef OFFSET_GET

    return diff;
}
