/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#ifndef __OIL_UTIL_H__
#define __OIL_UTIL_H__

#include <glib.h>
#include <oilparameter.h>

G_BEGIN_DECLS

#define oil_act_if_fail(expr, fail_stmts)                               \
G_STMT_START {                                                          \
    if (!(expr)) { fail_stmts; }                                        \
} G_STMT_END

#define oil_error_gerror(gerror)                                        \
G_STMT_START {                                                          \
    const gchar *domain = g_quark_to_string ((gerror)->domain);         \
    g_error ("%s: Error code: %d, %s",                                  \
                        domain ? domain : "",                           \
                        (gerror)->code,                                 \
                        (gerror)->message);                             \
    /* should abort */                                                  \
} G_STMT_END

#define oil_warning_gerror(gerror) G_STMT_START {                       \
    const gchar *domain = g_quark_to_string ((gerror)->domain);         \
    g_warning ("%s: Error code: %d, %s",                                \
                        domain ? domain : "",                           \
                        (gerror)->code,                                 \
                        (gerror)->message);                             \
} G_STMT_END

#define oil_timestamp(timestamp)                                        \
G_STMT_START {                                                          \
        GTimeVal _tv;                                                   \
        g_get_current_time (&_tv);                                      \
        *timestamp = (_tv.tv_sec * (1.0 * G_USEC_PER_SEC) +             \
                      _tv.tv_usec) / G_USEC_PER_SEC;                    \
} G_STMT_END

#define oil_log_detail(g_logfunc, fmt, args...)                         \
G_STMT_START {                                                          \
        gdouble _timestamp;                                             \
        oil_timestamp (&_timestamp);                                    \
        g_logfunc ("[%f] %s(): " fmt "\n", _timestamp,                  \
                 G_STRFUNC, ##args);                                    \
} G_STMT_END

#define oil_oops(fmt, args...) oil_log_detail(g_error, fmt, ##args)

typedef gpointer (*OilTimerNew) ();
typedef void (*OilTimerStamp) (gpointer timer);
typedef gdouble (*OilTimerElapsed) (gpointer timer, gulong *ret2);
typedef void (*OilTimerDestroy) (gpointer timer);
typedef struct _OilTimerClass OilTimerClass;

struct _OilTimerClass {
    OilTimerNew new;
    OilTimerStamp start;
    OilTimerStamp stop;
    OilTimerElapsed elapsed;
    OilTimerDestroy destroy;
    gdouble precise;
}; 

typedef void (*OilRandizeArray) (
        GRand *r,
        guint8 *array, 
        OilType type, 
        gint pre_n,
        gint stride, 
        gint post_n);

extern OilRandizeArray oil_randize_array;
extern OilTimerClass oil_timer_class;

guint8 oil_rand_u8 (GRand *r);
gint8 oil_rand_s8 (GRand *r);
guint16 oil_rand_u16 (GRand *r);
gint16 oil_rand_s16 (GRand *r);
guint32 oil_rand_u32 (GRand *r);
gint32 oil_rand_s32 (GRand *r);
guint64 oil_rand_u64 (GRand *r);
gint64 oil_rand_s64 (GRand *r);
gfloat oil_rand_f32 (GRand *r);
void oil_randize_array_default (
        GRand *r,
        guint8 *array, 
        OilType type, 
        gint pre_n,
        gint stride, 
        gint post_n);

gchar *oil_flags_to_string (guint flag);

gpointer oil_timer_new_default ();
void oil_timer_start_default (gpointer timer);
void oil_timer_stop_default (gpointer timer);
gdouble oil_timer_elapsed_default (gpointer timer, gulong *ret2);
void oil_timer_destroy_default (gpointer timer);

G_END_DECLS

#endif /* __OIL_UTIL_H__ */

