/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <time.h>
#include <errno.h>
#include <oilcore.h>
#include "oilutil.h"

OilRandizeArray oil_randize_array = oil_randize_array_default;
OilTimerClass oil_timer_class = {
    .new = oil_timer_new_default,
    .start = oil_timer_start_default,
    .stop = oil_timer_stop_default,
    .elapsed = oil_timer_elapsed_default,
    .destroy = oil_timer_destroy_default,
    .precise = 1e-8,
};

guint8 oil_rand_u8 (GRand *r)
{
    /* g_rand_int_range return gint32 in [begin..end-1]
     */
    return (guint8) g_rand_int_range (r, G_MININT8, G_MAXINT8 + 1);
}

gint8 oil_rand_s8 (GRand *r)
{
    return (gint8) g_rand_int_range (r, G_MININT8, G_MAXINT8 + 1);
}

guint16 oil_rand_u16 (GRand *r)
{
    return (guint16) g_rand_int_range (r, G_MININT16, G_MAXINT16 + 1);
}

gint16 oil_rand_s16 (GRand *r)
{
    return (gint16) g_rand_int_range (r, G_MININT16, G_MAXINT16 + 1);
}

guint32 oil_rand_u32 (GRand *r)
{
    return (guint32) g_rand_int (r);
}

gint32 oil_rand_s32 (GRand *r)
{
    return (gint32) g_rand_int (r);
}

guint64 oil_rand_u64 (GRand *r)
{
    guint64 ret = (guint32) g_rand_int (r);
    gint32 shift = g_rand_int_range (r, 0, 32);
    
    ret <<= shift;
    ret += (guint32) g_rand_int (r);
    
    return ret;
}

gint64 oil_rand_s64 (GRand *r)
{
    return (gint64) oil_rand_u64 (r);
}

/* range: [0, 1) */
gfloat oil_rand_f32 (GRand *r)
{
    return (gfloat) g_rand_double (r);
}
gdouble oil_rand_f64 (GRand *r)
{
    return g_rand_double (r);
}

void oil_randize_array_default (
        GRand *r,
        guint8 *array, 
        OilType type, 
        gint pre_n,
        gint stride, 
        gint post_n)
{
#define _FILL(c_type, rfunc)                            \
G_STMT_START {                                          \
    gint i;                                             \
    for (i = 0; i < post_n; i++) {                      \
        gint j;                                         \
        guint8 *ptr = array + i * stride;               \
                                                        \
        for (j = 0; j < pre_n; j++) {                   \
            c_type *_e = (c_type *) ptr + j;            \
            *_e = (c_type) rfunc (r);                   \
        }                                               \
    }                                                   \
} G_STMT_END

    switch (type) {
        case OIL_TYPE_s8p:
            _FILL (gint8, oil_rand_s8);
            break;
        case OIL_TYPE_u8p:
            _FILL (guint8, oil_rand_u8);
            break;
        case OIL_TYPE_s16p:
            _FILL (gint16, oil_rand_s16);
            break;
        case OIL_TYPE_u16p:
            _FILL (guint16, oil_rand_u16);
            break;
        case OIL_TYPE_s32p:
            _FILL (gint32, oil_rand_s32);
            break;
        case OIL_TYPE_u32p:
            _FILL (guint32, oil_rand_u32);
            break;
        case OIL_TYPE_s64p:
            _FILL (gint64, oil_rand_s64);
            break;
        case OIL_TYPE_u64p:
            _FILL (guint64, oil_rand_u64);
            break;
        case OIL_TYPE_f32p:
            _FILL (gfloat, oil_rand_f32);
            break;
        case OIL_TYPE_f64p:
            _FILL (gdouble, oil_rand_f64);
            break;
        default:
            g_error ("Unknow OilType %d", type);
            /* should abort*/
            break;
};

#undef _FILL 
}

gchar *oil_flags_to_string (guint flag)
{
    gchar *flags[OIL_FLAGBIT_NR + 1] = {0, };
    gchar *content = "no flag";
    gchar *ret;

    guint i, j;
    for (i = 0, j = 0; i < OIL_FLAGBIT_NR; i++) {
        if (flag & OIL_FLAG(i)) {
            flags[j] = oil_flags_descriptions[i];
            j++;
        }
    }

    if (j) content = g_strjoinv (", ", flags);
    ret = g_strdup_printf ("[%s]", content);
    if (j) g_free (content);

    return ret;
}

gpointer oil_timer_new_default ()
{
    return g_try_new0 (struct timespec, 2);
}

void oil_timer_start_default (gpointer timer)
{
    struct timespec *start = (struct timespec *) timer;

    if (clock_gettime (CLOCK_THREAD_CPUTIME_ID, start))
        g_error ("%s\n", g_strerror (errno));
}

void oil_timer_stop_default (gpointer timer)
{
    struct timespec *stop = (struct timespec *) timer + 1;
    
    if (clock_gettime (CLOCK_THREAD_CPUTIME_ID, stop))
        g_error ("%s\n", g_strerror (errno));
}

gdouble oil_timer_elapsed_default (gpointer timer, gulong *ret2)
{
    
    struct timespec *start = (struct timespec *) timer;
    struct timespec *stop = (struct timespec *) timer + 1;
    
    return (stop->tv_sec - start->tv_sec) * 1.0 * 1e9 + (stop->tv_nsec - start->tv_nsec) / 1e9;
}

void oil_timer_destroy_default (gpointer timer)
{
    g_free (timer);
}
