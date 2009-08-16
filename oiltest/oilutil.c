/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <time.h>
#include <errno.h>
#include <oilcore.h>
#include "oilutil.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/**
 * SECTION:randomize_functions
 * @short_description: randomize functions
 * 
 * Randomize functions, used to generate samples for testing.
 */

/**
 * oil_randize_array:
 * 
 * Function called by #oil_checker_default, to randomize the testing array.
 * You can overload it with your own function.
 */
OilRandizeArray oil_randize_array = oil_randize_array_default;

/**
 * oil_rand_u8:
 * @r: a #GRand, representing a random seed
 * 
 * Returns: an 8-bit unsigned random integer 
 */
guint8 oil_rand_u8 (GRand *r)
{
    /* g_rand_int_range return gint32 in [begin..end-1]
     */
    return (guint8) g_rand_int_range (r, G_MININT8, G_MAXINT8 + 1);
}

/**
 * oil_rand_s8:
 * @r: a #GRand, representing a random seed
 * 
 * Returns: an 8-bit signed random integer
 */
gint8 oil_rand_s8 (GRand *r)
{
    return (gint8) g_rand_int_range (r, G_MININT8, G_MAXINT8 + 1);
}

/**
 * oil_rand_u16:
 * @r: a #GRand, representing a random seed
 * 
 * Returns: a 16-bit unsigned random integer
 */
guint16 oil_rand_u16 (GRand *r)
{
    return (guint16) g_rand_int_range (r, G_MININT16, G_MAXINT16 + 1);
}

/**
 * oil_rand_s16:
 * @r: a #GRand, representing a random seed
 * 
 * Returns: a 16-bit signed random integer
 */
gint16 oil_rand_s16 (GRand *r)
{
    return (gint16) g_rand_int_range (r, G_MININT16, G_MAXINT16 + 1);
}

/**
 * oil_rand_u32
 * @r: a #GRand, representing a random seed
 * 
 * Returns: a 32-bit unsigned random integer
 */
guint32 oil_rand_u32 (GRand *r)
{
    return (guint32) g_rand_int (r);
}

/**
 * oil_rand_s32:
 * @r: a #GRand, representing a random seed
 * 
 * Returns: a 32-bit signed random integer
 */
gint32 oil_rand_s32 (GRand *r)
{
    return (gint32) g_rand_int (r);
}

/**
 * oil_rand_u64:
 * @r: a #GRand, representing a random seed
 * 
 * Returns: a 64-bit unsigned random integer
 */
guint64 oil_rand_u64 (GRand *r)
{
    guint64 ret = (guint32) g_rand_int (r);
    gint32 shift = g_rand_int_range (r, 0, 32);
    
    ret <<= shift;
    ret += (guint32) g_rand_int (r);
    
    return ret;
}

/**
 * oil_rand_s64:
 * @r: a #GRand, representing a random seed
 *
 * Returns: a 64-bit signed random integer
 */
gint64 oil_rand_s64 (GRand *r)
{
    return (gint64) oil_rand_u64 (r);
}

/**
 * oil_rand_f32:
 * @r: a #GRand, representing a random seed
 * 
 * Returns: a float, in range [0, 1)
 */
gfloat oil_rand_f32 (GRand *r)
{
    return (gfloat) g_rand_double (r);
}

/**
 * oil_rand_f64:
 * @r: a #GRand, representing a random seed
 *
 * Returns: a double, in range [0, 1)
 */
gdouble oil_rand_f64 (GRand *r)
{
    return g_rand_double (r);
}

/**
 * oil_randize_array_default:
 * @r: a #GRand, representing a random seed
 * @array: the array that will be filled with random data
 * @type: the #OilType of the @array's element
 * @pre_n: how many elements in a stride?
 * @stride: the stride
 * @post_n: length of the array equals to @stride * @post_n
 *
 * Fills @array with random data. It fills @pre_n elements(of the @type) 
 * at the starting, then jump to @stride + starting as the new starting, 
 * fills next @pre_n elements ...
 * This is the default randomize-array function, you can replace it with your
 * randomize-array function by changing #oil_randize_array
 */
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


/**
 * SECTION:timer
 * @short_description: the timer
 * 
 * The timer used by the default #OilPorfiler. Following the interface of timer,
 * you can also write your own timer. The timer will be internally used like this:
 * |[
 * /&ast; creates a timer &ast;/
 * gpointer timer = oil_timer_class.new ();
 * 
 * /&ast; starts the timer &ast;/
 * oil_timer_class.start (timer);
 * 
 * /&ast; stops the timer &ast;/
 * oil_timer_class.stop (timer);
 *
 * /&ast; get the eclapsed timer &ast;/
 * oil_timer_class.eclapsed (timer);
 * 
 * /&ast; destroy the timer &ast;/
 * oil_timer_class.destroy (timer);
 * ]|
 */

#if defined HAVE_CLOCK_THREAD_CPUTIME_ID || defined HAVE_CLOCK_MONOTONIC
static gpointer oil_timer_new_default ()
{
    return g_try_new0 (struct timespec, 2);
}

static void oil_timer_start_default (gpointer timer)
{
    struct timespec *start = (struct timespec *) timer;

#ifdef HAVE_CLOCK_THREAD_CPUTIME_ID
# define _TIMER CLOCK_THREAD_CPUTIME_ID
#else /* defined HAVE_CLOCK_MONOTONIC */
# define _TIMER CLOCK_MONOTONIC
#endif

    if (clock_gettime (_TIMER, start))
        g_error ("%s\n", g_strerror (errno));

#undef _TIMER
}

static void oil_timer_stop_default (gpointer timer)
{
    struct timespec *stop = (struct timespec *) timer + 1;

#ifdef HAVE_CLOCK_THREAD_CPUTIME_ID
# define _TIMER CLOCK_THREAD_CPUTIME_ID
#else /* defined HAVE_CLOCK_MONOTONIC */
# define _TIMER CLOCK_MONOTONIC
#endif

    if (clock_gettime (_TIMER, stop))
        g_error ("%s\n", g_strerror (errno));

#undef _TIMER
}

static gdouble oil_timer_elapsed_default (gpointer timer, gulong *ret2)
{
    
    struct timespec *start = (struct timespec *) timer;
    struct timespec *stop = (struct timespec *) timer + 1;
    
    return (stop->tv_sec - start->tv_sec) * 1.0 * 1e9 + (stop->tv_nsec - start->tv_nsec) / 1e9;
}

void oil_timer_destroy_default (gpointer timer)
{
    g_free (timer);
}
/**
 * oil_timer_default:
 * 
 * You can activate this timer by "oil_timer = &oil_timer_default"
 */
OilTimerClass oil_timer_default = {
    .new = oil_timer_new_default,
    .start = oil_timer_start_default,
    .stop = oil_timer_stop_default,
    .elapsed = oil_timer_elapsed_default,
    .destroy = oil_timer_destroy_default,
    .precise = 1e-8,
};
#else

OilTimerClass oil_timer_default = {
    .new = (OilTimerNew) g_timer_new,
    .start = (OilTimerStart) g_timer_start,
    .stop = (OilTimerStop) g_timer_stop,
    .elapsed = (OilTimerElapsed) g_timer_elapsed,
    .destroy =  (OilTimerDestroy) g_timer_destroy,
    .precise = 1e-8,
};

#endif /* HAVE_CLOCK_THREAD_CPUTIME_ID || defined HAVE_CLOCK_MONOTONIC */

/**
 * oil_timer:
 * 
 * The timer used by #oil_profiler_default.
 * You can change to your own timer, the default is #oil_timer_default
 */
OilTimerClass *oil_timer = &oil_timer_default;

/**
 * SECTION:util_misc
 * @short_description: misc functions
 */
/**
 * oil_flags_to_string:
 * @flags: the flags, see #OilCPUFlagBits
 *
 * Get the string description of the @flags
 * 
 * Returns: a string description of the @flags
 */
gchar *oil_flags_to_string (guint flags)
{
    gchar *flagv[OIL_FLAGBIT_NR + 1] = {0, };
    gchar *content = "no flag";
    gchar *ret;

    guint i, j;
    for (i = 0, j = 0; i < OIL_FLAGBIT_NR; i++) {
        if (flags & OIL_FLAG(i)) {
            flagv[j] = oil_flags_descriptions[i];
            j++;
        }
    }

    if (j) content = g_strjoinv (", ", flagv);
    ret = g_strdup_printf ("[%s]", content);
    if (j) g_free (content);

    return ret;
}
