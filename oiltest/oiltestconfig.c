/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "oiltestconfig.h"
#include "oilutil.h"


typedef struct _OilTestConfig OilTestConfig;

struct _OilTestConfig {
    GTrashStack frame;
    
    gint log_fd;
    gchar *seed_str;
    GRand *rand;
    gint test_header;
    gint test_footer;
    gint test_m;
    gint test_n;
    gint iterations;
    gboolean silent;
    gdouble tolerance_high;
    gdouble tolerance_low;

};

static GTrashStack *oil_test_config = NULL;

static OilTestConfig *_dup_top_frame ();
static void _free_frame (OilTestConfig *_f);
static gchar *_gen_seed (const gchar *seed_str, GRand **r);
static gchar *_gen_seed_auto (GRand **rand_);
static gchar *_gen_seed_from_string (
        const gchar *seed_str,
        GRand **rand_);
static gboolean _may_init ();

/**
 * SECTION:test_config
 * @short_description: configure testing context in a stack style
 *
 * oil_test_config_* functions compose a stack-based interface
 * to control how the testing is performed.
 * <note>
 * <para>
 * oiltest only reads the configuration variables before performing the actual test,
 * so you should call these functions before calling oil_test_optimize_class*
 * </para>
 * </note>
 */

/**
 * oil_test_config_height:
 *
 * The height of the stack, only for debugging.
 *
 * Returns: the height of the stack.
 */
guint oil_test_config_height ()
{
    return g_trash_stack_height (&oil_test_config);
}

/**
 * oil_test_config_push:
 * @n_params: the number of remaining arguments or -1, 
 * which means the variable-length argument list is NULL-terminated.
 *
 * Push a frame, and modifies some variables, e.g.
 * |[
 * oil_test_config_push (3, 
 *                       OIL_CONF_OPT_LOG_FD, 0,
 *                       OIL_CONF_OPT_SEED, "R02S939fc71d669f7c0c88a7eb85b190df83",
 *                       OIL_CONF_OPT_SILENT, FALSE);
 *
 *
 * oil_test_config_push (-1,
 *                       OIL_CONF_OPT_HEADER_LEN, 128,
 *                       OIL_CONF_OPT_TOLERANCE_HIGH, 0.001,
 *                       OIL_CONF_OPT_TOLERANCE_LOW, 0,
 *                       NULL);
 * ]|
 */
void oil_test_config_push (gint n_params, ...)
{
    va_list args;
    
    if (!_may_init ())
        g_trash_stack_push (
                &oil_test_config,
                (gpointer) _dup_top_frame ());
    
    va_start (args, n_params);
    oil_test_config_vset (n_params, args);
    va_end (args);
}

/**
 * oil_test_config_set:
 * @n_params: the number of remaining arguments or -1, 
 * which means the variable-length argument list ends with NULL
 *
 * Like oil_test_config_push(), but not push any frame.
 */
void oil_test_config_set (gint n_params, ...)
{
    va_list args;
    
    va_start (args, n_params);
    oil_test_config_vset (n_params, args);
    va_end (args);
}

/**
 * oil_test_config_get:
 * @n_params: the number of remaining arguments or -1, 
 * which means the variable-length argument list ends with NULL
 *
 * Get the values of the variables of the top frame, e. g.
 * |[
 * gchar *seed;
 * gint header_len;
 * 
 * oil_test_config_get (-1,
 *                      /&ast; returns internal string, don't free &ast;/
 *                      OIL_CONF_OPT_SEED, &seed, 
 *                      OIL_CONF_OPT_HEADER_LEN, &header_len,
 *                      NULL);
 * ]|
 */
void oil_test_config_get (gint n_params, ...)
{
    va_list args;
    
    va_start (args, n_params);
    oil_test_config_vget (n_params, args);
    va_end (args);
}

/**
 * oil_test_config_vset:
 * @n_params: the number of arguments in @va_args or -1, 
 * which means the @va_args is NULL-terminated
 * @va_args: variable-length argument list
 *
 * va_list version of oil_test_config_set().
 */
void oil_test_config_vset (gint n_params, va_list va_args)
{
    OilTestConfig *_f;
    
    _may_init ();
    _f = (OilTestConfig *) g_trash_stack_peek (&oil_test_config);
    
    if (n_params) {
        guint i, n = (guint) n_params;
        
        for (i = 0; i < n; i++) {
            gint opt = va_arg (va_args, gint);
            
            if (n_params == -1 && opt == 0)
                break;
        
            switch (opt) {
                case OIL_CONF_OPT_LOG_FD:
                    {
                        gint fd = va_arg (va_args, gint);
                        oil_act_if_fail (
                                fd >= 0,
                                g_warning ("set log_fd to invaild value: %d", fd);
                                continue);
                        _f->log_fd = fd;
                    }
                    break;
                case OIL_CONF_OPT_SEED:
                    {
                        gchar *seed_str;
                        GRand *rand_ = NULL;

                        seed_str = _gen_seed (va_arg (va_args, const gchar *), &rand_);
                        if (seed_str) {
                            if (_f->rand) {
                                g_free (_f->seed_str);
                                g_rand_free (_f->rand);
                            }
                            _f->rand = rand_;
                            _f->seed_str = seed_str;
                        }
                    }
                    break;
                case OIL_CONF_OPT_HEADER_LEN:
                    {
                        gint header_len = va_arg (va_args, gint);
                        oil_act_if_fail (
                                header_len >= 0,
                                g_warning ("set header_len to invaild value: %d", header_len);
                                continue);
                        _f->test_header = header_len;
                    }
                    break;
                case OIL_CONF_OPT_FOOTER_LEN:
                    {
                        gint footer_len = va_arg (va_args, gint);
                        oil_act_if_fail (
                                footer_len >= 0,
                                g_warning ("set footer_len to invaild value: %d", footer_len);
                                continue);
                        _f->test_footer = footer_len;
                    }
                    break;
                case OIL_CONF_OPT_TEST_M:
                    {
                        gint m = va_arg (va_args, gint);
                        oil_act_if_fail (
                                m >= 0,
                                g_warning ("set test_m to invaild value: %d", m);
                                continue);
                        _f->test_m = m;
                    }
                    break;
                case OIL_CONF_OPT_TEST_N:
                    {
                        gint n = va_arg (va_args, gint);
                        oil_act_if_fail (
                                n >= 0,
                                g_warning ("set test_n to invaild value: %d", n);
                                continue);
                        _f->test_n = n;
                    }
                    break;
                case OIL_CONF_OPT_SILENT:
                    _f->silent = va_arg (va_args, gboolean);
                    break;
                case OIL_CONF_OPT_ITERATIONS:
                    {
                        gint iterations = va_arg (va_args, gint);
                        oil_act_if_fail (
                            iterations > 0,
                            g_warning ("set iterations to invaild value: %d", iterations);
                            continue);
                        _f->iterations = iterations;
                    }
                    break;
                case OIL_CONF_OPT_TOLERANCE_HIGH:
                    {
                        gdouble tolerance_high = va_arg (va_args, gdouble);
                        oil_act_if_fail (
                            tolerance_high >= 0,
                            g_warning ("set tolerance_high to invaild value: %g", tolerance_high);
                            continue);
                        _f->tolerance_high = tolerance_high;
                    }
                    break;
                case OIL_CONF_OPT_TOLERANCE_LOW:
                    {
                        gdouble tolerance_low = va_arg (va_args, gdouble);
                        oil_act_if_fail (
                            tolerance_low >= 0,
                            g_warning ("set tolerance_low to invaild value: %g", tolerance_low);
                            continue);
                        _f->tolerance_low = tolerance_low;
                    }
                    break;
                default:
                    g_warning ("set testconfig: not supported option_id %d", opt);
                    return;
            }
        }
    }
}

/**
 * oil_test_config_vget:
 * @n_params: the number of arguments in @va_args or -1, 
 * which means the @va_args is NULL-terminated
 * @va_args: variable-length argument list
 * 
 * va_list version of oil_test_config_get().
 */
void oil_test_config_vget (gint n_params, va_list va_args)
{
    OilTestConfig *_f;
    
    _may_init ();
    _f = (OilTestConfig *) g_trash_stack_peek (&oil_test_config);
    
    if (n_params) {
        guint i, n = (guint) n_params;
        gint *i_ptr;
        gchar **s_ptr;
        gdouble *f_ptr;
        gboolean *b_ptr;
        
        for (i = 0; i < n; i++) {
            gint opt = va_arg (va_args, gint);
            
            if (n_params == -1 && opt == 0)
                break;
        
            switch (opt) {
                case OIL_CONF_OPT_LOG_FD:
                    i_ptr = va_arg (va_args, gint *);
                    *i_ptr = _f->log_fd;
                    break;
                case OIL_CONF_OPT_SEED:
                    /* internal string: don't free */
                    s_ptr = va_arg (va_args, gchar **);
                    *s_ptr =_f->seed_str;
                    break;
                case OIL_CONF_OPT_HEADER_LEN:
                    i_ptr = va_arg (va_args, gint *);
                    *i_ptr = _f->test_header;
                    break;
                case OIL_CONF_OPT_FOOTER_LEN:
                    i_ptr = va_arg (va_args, gint *);
                    *i_ptr = _f->test_footer;
                    break;
                case OIL_CONF_OPT_TEST_M:
                    i_ptr = va_arg (va_args, gint *);
                    *i_ptr = _f->test_m;
                    break;
                case  OIL_CONF_OPT_TEST_N:
                    i_ptr = va_arg (va_args, gint *);
                    *i_ptr = _f->test_n;
                    break;
                case OIL_CONF_OPT_SILENT:
                    b_ptr = va_arg (va_args, gint *);
                    *b_ptr = _f->silent;
                    break;
                case OIL_CONF_OPT_ITERATIONS:
                    i_ptr = va_arg (va_args, gint *);
                    *i_ptr = _f->iterations;
                    break;
                case OIL_CONF_OPT_TOLERANCE_HIGH:
                    f_ptr = va_arg (va_args, gdouble *);
                    *f_ptr = _f->tolerance_high;
                    break;
                case OIL_CONF_OPT_TOLERANCE_LOW:
                    f_ptr = va_arg (va_args, gdouble *);
                    *f_ptr = _f->tolerance_low;
                    break;
                default:
                    g_warning ("set test_config: not supported option_id %d", opt);
                    return;
            }
        }
    }
}

/**
 * oil_test_config_pop:
 * 
 * Pop up the top frame.
 */
void oil_test_config_pop ()
{
    OilTestConfig *_f = g_trash_stack_pop (&oil_test_config);
    
    if (_f)
        _free_frame (_f);
}

/**
 * oil_test_config_pop_all:
 *
 * Empty the stack.
 */
void oil_test_config_pop_all ()
{
    OilTestConfig *_f = g_trash_stack_pop (&oil_test_config);
    
    while (_f) {
        _free_frame (_f);
        _f = g_trash_stack_pop (&oil_test_config);
    }
}

/**
 * oil_test_config_silent:
 * 
 * A shortcut of getting whether the silent mode is enabled
 * 
 * Returns: a #gboolean
 */
gboolean oil_test_config_silent ()
{
    OilTestConfig *_f;
    _may_init ();
    
    _f = (OilTestConfig *) g_trash_stack_peek (&oil_test_config);
    
    return _f->silent;
}

/**
 * oil_test_config_iterations:
 * 
 * A shortcut of getting the times of the iteration.
 */
gint oil_test_config_iterations ()
{
    OilTestConfig *_f;
    _may_init ();
    
    _f = (OilTestConfig *) g_trash_stack_peek (&oil_test_config);
    
    return _f->iterations;
}

/**
 * oil_test_config_tolerance_high:
 *
 * A shortcut of getting the extreme deviation from the reference could be accepted.
 */
gdouble oil_test_config_tolerance_high ()
{
    OilTestConfig *_f;
    _may_init ();
    
    _f = (OilTestConfig *) g_trash_stack_peek (&oil_test_config);
    return _f->tolerance_high;
}

/**
 * oil_test_config_tolerance_low:
 *
 * A shortcut of getting the minimal deviation from the reference could be accepted.
 */
gdouble oil_test_config_tolerance_low ()
{
    OilTestConfig *_f;
    _may_init ();
    
    _f = (OilTestConfig *) g_trash_stack_peek (&oil_test_config);
    return _f->tolerance_low;
}

/**
 * oil_test_config_rand:
 * A shortcut of getting the random seed.
 * 
 * Returns: the internal #GRand, so don't free
 */
GRand *oil_test_config_rand ()
{
    OilTestConfig *_f;
    _may_init ();
    
    _f = (OilTestConfig *) g_trash_stack_peek (&oil_test_config);
    
    return _f->rand;
}

/**
 * oil_test_config_logfd:
 * A shortcut of getting the log file descriptor.
 * 
 * Returns: the fd of the log file
 */
gint oil_test_config_logfd ()
{
    OilTestConfig *_f;
    _may_init ();
    
    _f = (OilTestConfig *) g_trash_stack_peek (&oil_test_config);
    
    return _f->log_fd;
}

static gboolean _may_init ()
{
    OilTestConfig *_f;
    
    if (g_trash_stack_peek (&oil_test_config))
        return FALSE;
    
    _f = g_slice_new0 (OilTestConfig);
    oil_act_if_fail (_f, oil_oops ("memory alloc failed"));

    _f->log_fd = -1;
    _f->test_header = 256;
    _f->test_footer = 256;
    _f->test_m = 100;
    _f->test_n = 100;
    _f->iterations = 10;
    _f->seed_str = _gen_seed_auto (&_f->rand);
    _f->silent = TRUE;
    _f->tolerance_high = 0.001;
    _f->tolerance_low = 0.0;
    
    g_trash_stack_push (&oil_test_config, (gpointer) _f);
    
    return TRUE;
}

#define SEED_ARRAY_LEN 4
static gchar *_gen_seed_auto (GRand **rand_)
{
    guint32 seed_array[SEED_ARRAY_LEN];
    gchar seed_str[4 + SEED_ARRAY_LEN * 8 + 1] = "R02S";
    gchar *s = seed_str + 4;
    guint i;
    
    for (i = 0; i < SEED_ARRAY_LEN; i++) {
        guint32 r = g_random_int ();
        
        seed_array[i] = r;
        snprintf (s + 8 * i, 9, "%x", r);
    }
    *rand_ = g_rand_new_with_seed_array (seed_array, SEED_ARRAY_LEN);
    
    return g_strndup (seed_str, 4 + SEED_ARRAY_LEN * 8);
}

static gchar *_gen_seed_from_string (
        const gchar *seed_str,
        GRand **rand_)
{
    /* eat continuous blank in the beginning */
    while (strchr (" \t\v\r\n\f", *seed_str))
        seed_str++;

    /* seed for random generator 02 (GRand-2.2) */
    if (strncmp (seed_str, "R02S", 4) == 0) {
        const gchar *s = seed_str + 4;
        
        if (strlen (s) >= SEED_ARRAY_LEN * 8) { /* require SEED_ARRAY_LEN * 8 chars */
            guint32 seed_array[SEED_ARRAY_LEN];
            gchar *p, hexbuf[9] = { 0, };
            gint i;
            
            for (i = 0; i < SEED_ARRAY_LEN; i++) {
                memcpy (hexbuf, s + 8 * i, 8);
            
                seed_array[0] = g_ascii_strtoull (hexbuf, &p, 16);
            }
            
            *rand_ = g_rand_new_with_seed_array (seed_array, SEED_ARRAY_LEN);
            
            return g_strndup (seed_str, 4 + SEED_ARRAY_LEN * 8);
        }
    }

    g_error ("Unknown or invalid random seed: %s", seed_str);
    return NULL;
}
#undef SEED_ARRAY_LEN

static gchar *_gen_seed (const gchar *seed_str, GRand **r)
{
    if (seed_str && *seed_str) 
        return _gen_seed_from_string (seed_str, r);
    else
        return _gen_seed_auto (r);
}

static OilTestConfig *_dup_top_frame ()
{
    OilTestConfig *ret;
    OilTestConfig *tp = (OilTestConfig *) g_trash_stack_peek (&oil_test_config);

    ret = g_slice_dup (OilTestConfig, (gpointer) tp);
    
    oil_act_if_fail (ret, oil_oops ("memory alloc failed"));
    ret->seed_str = g_strdup (tp->seed_str);
    ret->rand = g_rand_copy (tp->rand);
    
    return ret;
}

static void _free_frame (OilTestConfig *_f)
{
    g_rand_free (_f->rand);
    g_free (_f->seed_str);
    g_slice_free (OilTestConfig, _f);
}
