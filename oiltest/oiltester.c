/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * not MT safe
 */

#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "oiltest.h"
#include "oilutil.h"
#include "oiltestconfig.h"

static jmp_buf jump_env;
static gint in_try_block;
static gint enable_level;

static struct sigaction action, oldaction;

/* Define data struct */
typedef struct _OilClassData {
    gchar *prototype;
    OilParameter *parameters;
    guint n_params;
    
    OilProfCall call;
    OilChecker *checker;
    OilProfiler *profiler;
} OilClassData;

static inline void _optimize_class_visitor_wrap (
        const gchar *cls_name,
        OilClass *cls,
        gpointer user_data);
static void _optimize_class (
        OilClass *cls,
        OilClassData *cls_data,
        void *ref);
static void _calc_sample_layout (guint n_params, OilParameter *params);
static void _calc_layout_of_array (
        OilParameter *array,
        OilParameter *stride);
static void _may_alloc_mem_of_params (
        guint n_params,
        OilParameter *params,
        OilGenericType *args,
        OilGenericType *real_args);
static void _may_free_mem_of_params (
        guint n_params,
        OilParameter *params,
        OilGenericType* args);
static void _sigill_handler (int num);
static void _fault_check_enable (void);
static void _fault_check_disable (void);
static void _arm_test_data (
        guint n_params,
        OilParameter *params,
        OilGenericType *tdata,
        OilGenericType *src_sample);

struct _fixture {
    OilClassData *class_data;
    
    guint n_params;
    OilParameter *params;
    
    OilGenericType *src_sample;
    OilGenericType *ref_sample;
    OilGenericType *tdata;
    
    OilGenericType *real_src_sample;
    OilGenericType *real_ref_sample;
    OilGenericType *real_tdata;
    
    OilChecker *checker;
    OilProfiler *profiler;
    
    gdouble min_val;
    gchar *chosen_impl;
};
static gboolean _check_reference (void *ref, struct _fixture *fixture);
static int _check_implement (
        char *impl_name,
        void *func,
        unsigned int flags,
        void *user_data);
static gboolean _check_violation (
        guint n_params,
        OilParameter *params,
        const OilGenericType *src_data,
        OilGenericType *data);
static gboolean _check_guard (guint8 *p, gint size, gint8 guard);

/**
 * SECTION:test_log
 * @short_description: logging function that used in the test
 * 
 * These logging functions output log in "header:body\n" text formation.
 * To enable logging, you need set silent_mode FALSE and indicate a logfd by oil_test_config_set()
 */

/**
 * oil_test_log:
 * @msg: the log message
 *
 * Appends a "\n" to @msg, and outputs it.
 */
void oil_test_log (gchar *msg)
{
    if (!oil_test_config_silent ()) {
    
        gint log_fd = oil_test_config_logfd ();
        
        if (log_fd >= 0 && msg && *msg) {
            guint len = strlen (msg);
            gchar buf[len + 2];
            
            memcpy (buf, msg, len);
            buf[len] = '\n';
            buf[len + 1] = '\0';
            
            int r;
            do {
                r = write (log_fd, buf, len + 2);
            } while (r < 0 && errno == EINTR);
        }
    }
}

/**
 * oil_test_log_printf:
 * @format: the format just like that used in printf
 * 
 * A printf-like logging function.
 */
void oil_test_log_printf (gchar *format, ...)
{
    if (!oil_test_config_silent ()) {
        gchar *msg;
        va_list args;
        
        va_start (args, format);
        
        msg = g_strdup_vprintf (format, args);
        oil_test_log (msg);
        g_free (msg);
        
        va_end (args);
    }
}

/**
 * oil_test_log_binary:
 * @ptr: point the binary data
 * @n: length of the binary data
 * 
 * Outputs the binary data in hex.
 */
void oil_test_log_binary (guint8 *ptr, guint n)
{
    guint i;
    GString *gstr;
    
    if (oil_test_config_silent ()) return;
    g_return_if_fail (ptr && n > 0);
    
    gstr = g_string_new (NULL);
    
    for (i = 0; i < n; i++) {
        guint byte = ptr[i];
        g_string_append_printf (gstr, "%02x", byte);
    }
    oil_test_log (gstr->str);
    g_string_free (gstr, TRUE);
}

/**
 * oil_test_log_parameter:
 * @param: an #OilParameter
 * 
 * Outputs an #OilParameter.
 */
void oil_test_log_parameter (OilParameter *param)
{
    GString *gstr;
    
    if (oil_test_config_silent ()) return;
    g_return_if_fail (param);
    
    gstr = g_string_new (NULL);
    g_string_append_printf  (gstr, "Parameter: %s", param->parameter);
    if (param->is_pointer) {
        gchar *dirs[]={"SCALE", "SRC", "DEST", "INPLACE"};
        switch (param->arg_class) {
            case OIL_ARG_T_SRC:
            case OIL_ARG_T_DEST:
            case OIL_ARG_T_INPLACE:
                g_string_append_printf  (
                        gstr,
                        "\nDirection: %s", 
                        dirs[param->arg_class]);
                break;
            default:
                g_string_append  (gstr, "\nDirection: BAD_DIRECTION");
                break;
        }
        
        g_string_append_printf (
                gstr,
                "\nsize: %d\nheader: %d\npre_n: %d\nstride: %d\npost_n: %d\nfooter: %d\nguard: %u", 
                param->size,
                param->header,
                param->pre_n,
                param->stride,
                param->post_n,
                param->footer,
                (guint8) param->guard);
        
    } else {
        switch (param->c_type) {
            case OIL_TYPE_INT:
            case OIL_TYPE_s8:
            case OIL_TYPE_s16:
            case OIL_TYPE_s32:
                g_string_append_printf (
                        gstr,
                        "\nvalue: %d",
                        GPOINTER_TO_INT (param->value.p));
                break;
            case OIL_TYPE_s64:
                g_string_append_printf (
                        gstr,
                        "\nvalue: %qd",
                        (gint64) param->value.i64);
                break;
            case OIL_TYPE_u8:
            case OIL_TYPE_u16:
            case OIL_TYPE_u32:
                g_string_append_printf (
                        gstr,
                        "\nvalue: %u",
                        GPOINTER_TO_UINT (param->value.p));
                break;
            case OIL_TYPE_u64:
                g_string_append_printf (
                        gstr,
                        "\nvalue: %qu",
                        (guint64) param->value.i64);
                break;
            case OIL_TYPE_f32:
            case OIL_TYPE_f64:
                g_string_append_printf (
                        gstr,
                        "\nvalue: %g",
                        param->value.f64);
                break;
            default:
                g_string_append_printf (
                        gstr,
                        "\nvalue: %p",
                        param->value.p);
                break;
        }
    }
    oil_test_log (gstr->str);
    g_string_free (gstr, TRUE);
}

/**
 * SECTION:tester
 * @short_description: functions that perform the actual test.
 * 
 * First you need attach some testing information to function class, then run
 * oil_test_optimize_class_all() or oil_test_optimize_class(), e.g.
 * |[
 * /&ast; initialize the oilcore environment &ast;/
 * oil_class_init (oil_test_destroy_class_data);
 * 
 * /&ast; register some function classes & add some implements to each class &ast;/
 * ...
 * 
 * /&ast; attach testing context to each class &ast;/
 * oil_test_attach_class_full (
 *     "oil_copy8x8_u8", /&ast; name of the function class &ast;/
 *     "uint8_t * d_8x8, int ds, uint8_t * s_8x8, int ss", /&ast; prototype string &ast;/
 *     OIL_MARSHAL__UINT8_Tp_INT_UINT8_Tp_INT, /&ast; the marshal function &ast;/
 *     &oil_checker_default, /&ast; the default checker &ast;/
 *     &oil_profiler_default); /&ast; the default profiler &ast;/
 *
 * /&ast; perform the test, optimizing all classes &ast;/
 * oil_test_optimize_class_all ();
 *
 * /&ast; un-initialize the oilcore environment, if not need it anymore &ast;/
 * oil_class_uninit (oil_test_destroy_class_data);
 * ]|
 */
/**
 * oil_test_attach_class_full:
 * @cls_name: the name of the function class
 * @prototype: the prototype string
 * @call: the marshal function
 * @checker: the checker, whether you provide one or oil_checker_default
 * @profiler: the profiler, whether you provide one or oil_profiler_default
 *
 * Attach testing context to @cls_name, see the description for detail.
 * This function is typically not directly used by user, 
 * the script #oil-regimpls will generate code of attaching.
 */
void oil_test_attach_class_full (
        gchar *cls_name, 
        gchar *prototype,
        OilProfCall call,
        OilChecker *checker,
        OilProfiler *profiler)
{
    OilClass *cls;
    OilClassData *data = NULL;
    OilParameter *params = NULL;
    guint n_params;
    
    g_return_if_fail (cls_name && *cls_name);
    g_return_if_fail (prototype && *prototype);
    g_return_if_fail (call);
    g_return_if_fail (checker && 
            checker->init && checker->uninit &&
            checker->generate_sample && checker->check_result);
    g_return_if_fail (profiler && 
            profiler->begin && profiler->end &&
            profiler->start && profiler->stop);
    
    cls = oil_class_get (cls_name);
    oil_act_if_fail (cls, g_warning ("Fail to attach test context to class \"%s\": no such class", cls_name); return);

    data = oil_class_get_data (cls);
    oil_act_if_fail (!data, g_warning ("Fail to attach test context to class \"%s\": already attached", cls_name); return);
    
    if (oil_prototype_parse (prototype, &n_params, &params)) {
        data = g_slice_new (OilClassData);
        oil_act_if_fail (data, oil_oops ("memory alloc failed"));
        
        /* chain to cls & free check checker & profiler */
        data->prototype = g_strdup (prototype);
        data->n_params = n_params;
        data->parameters = params;
        data->call = call;
        data->checker = checker;
        data->profiler = profiler;
        
        oil_class_add_data (cls, data);
    }
}

/**
 * oil_test_destroy_class_data:
 * 
 * The only usage of this function is passed itself to oil_class_init.
 * Then it will release the testing context when un-initialize the oilcore environment.
 */
void oil_test_destroy_class_data (void *data)
{
    if (data) {
        gint i;
        
        OilClassData *cls_data = (OilClassData *) data;
        
        g_free (cls_data->prototype);
        
        for (i = 0; i < cls_data->n_params; i++)
            g_free ((cls_data->parameters + i)->parameter);
        
        g_free (cls_data->parameters);
        g_slice_free (OilClassData, data);
    }
}

/**
 * oil_test_optimize_class:
 * @cls_name: the name of the function class
 * 
 * Performs the test on @cls_name, select & activate the best implement for @cls_name.
 */
void oil_test_optimize_class (char *cls_name)
{
    OilClass *cls = NULL;
    void *ref = NULL;
    OilClassData *cls_data = NULL;
    
    cls = oil_class_get (cls_name);
    oil_act_if_fail (cls, g_warning ("Fail to Optimize function class \"%s\": no such class", cls_name); return);
    
    cls_data = (OilClassData *) oil_class_get_data (cls);
    oil_act_if_fail (cls_data, g_warning ("Fail to Optimize function class \"%s\": testing context not attached", cls_name); return);
    
    ref = oil_class_get_reference (cls);
    oil_act_if_fail (ref, g_warning ("Fail to Optimize function class \"%s\": reference function not defined", cls_name); return);
    
    _optimize_class (cls, cls_data, ref);
}

/**
 * oil_test_optimize_class_all:
 *
 * Like oil_test_optimize_class(), but performs test on all classs.
 */
void oil_test_optimize_class_all ()
{
    oil_class_foreach (_optimize_class_visitor_wrap, NULL);
}

static inline void _optimize_class_visitor_wrap (
        const gchar *cls_name,
        OilClass *cls,
        gpointer user_data)
{
    OilClassData *cls_data = NULL;
    void *ref = NULL;
    
    cls_data = (OilClassData *) oil_class_get_data (cls);
    oil_act_if_fail (cls_data, g_warning ("Fail to Optimize function class \"%s\": testing context not attached", cls_name); return);
    
    ref = oil_class_get_reference (cls);
    oil_act_if_fail (ref, g_warning ("Fail to Optimize function class \"%s\": reference function not defined", cls_name); return);
    
    _optimize_class (cls, cls_data, ref);
}

static void _optimize_class (
        OilClass *cls,
        OilClassData *cls_data,
        void *ref)
{
    guint n_params = cls_data->n_params;
    OilParameter *params = cls_data->parameters;
    OilChecker *checker = cls_data->checker;
    OilProfiler *profiler = cls_data->profiler;
    
    OilGenericType src_sample[n_params];
    OilGenericType ref_sample[n_params];
    OilGenericType tdata[n_params];

    OilGenericType real_src_sample[n_params];
    OilGenericType real_ref_sample[n_params];
    OilGenericType real_tdata[n_params];
    
    struct _fixture fixture = {
        .class_data = cls_data,
        
        .n_params = n_params,
        .params = params,
        
        .src_sample = src_sample,
        .ref_sample = ref_sample,
        .tdata = tdata,
        
        .real_src_sample = real_src_sample,
        .real_ref_sample = real_ref_sample,
        .real_tdata = real_tdata,
        
        .checker = checker,
        .profiler = profiler,
    };
    
    /* setup */
    _calc_sample_layout (n_params, params);
    _may_alloc_mem_of_params (n_params, params, src_sample, real_src_sample);
    _may_alloc_mem_of_params (n_params, params, ref_sample, real_ref_sample);
    _may_alloc_mem_of_params (n_params, params, tdata, real_tdata);
    
    checker->init (checker, n_params, params);
    checker->generate_sample (checker, real_src_sample);

    _fault_check_enable ();
    
    /* profile reference function */
    _check_reference (ref, &fixture);
    
    /* check each implement  */
    oil_class_implements_foreach (
            cls,
            (OilImplementVisitor) _check_implement,
            (void *) &fixture);
    
    _fault_check_disable ();

    /* teardown */
    checker->uninit (checker);

    _may_free_mem_of_params (n_params, params, src_sample);
    _may_free_mem_of_params (n_params, params, ref_sample);
    _may_free_mem_of_params (n_params, params, tdata);
    
    /* choose best */
    oil_test_log_printf (
            "Choose: %s",
            fixture.chosen_impl ? fixture.chosen_impl : "reference");
    oil_class_active_implement (cls, fixture.chosen_impl);
}

static void _calc_sample_layout (guint n_params, OilParameter *params)
{
    OilParameter *rearrange_params[OIL_ARG_LAST] = {0,};
    guint i;
    
    for (i = 0; i < n_params; i++) {
        OilParameter *p = params + i;
        rearrange_params[p->arg_type] = p;
    }
    
    _calc_layout_of_array (
            rearrange_params[OIL_ARG_DEST1],
            rearrange_params[OIL_ARG_DSTR1]);
    _calc_layout_of_array (
            rearrange_params[OIL_ARG_DEST2],
            rearrange_params[OIL_ARG_DSTR2]);
    _calc_layout_of_array (
            rearrange_params[OIL_ARG_DEST3],
            rearrange_params[OIL_ARG_DSTR3]);

    _calc_layout_of_array (
            rearrange_params[OIL_ARG_SRC1],
            rearrange_params[OIL_ARG_SSTR1]);
    _calc_layout_of_array (
            rearrange_params[OIL_ARG_SRC2],
            rearrange_params[OIL_ARG_SSTR2]);
    _calc_layout_of_array (
            rearrange_params[OIL_ARG_SRC3],
            rearrange_params[OIL_ARG_SSTR3]);
    _calc_layout_of_array (
            rearrange_params[OIL_ARG_SRC4],
            rearrange_params[OIL_ARG_SSTR4]);
    _calc_layout_of_array (
            rearrange_params[OIL_ARG_SRC5],
            rearrange_params[OIL_ARG_SSTR5]);
    
    _calc_layout_of_array (
            rearrange_params[OIL_ARG_INPLACE1],
            rearrange_params[OIL_ARG_ISTR1]);
    _calc_layout_of_array (
            rearrange_params[OIL_ARG_INPLACE2],
            rearrange_params[OIL_ARG_ISTR2]);
    
    if (rearrange_params[OIL_ARG_N]) {
        gint n;
        
        oil_test_config_get (1, OIL_CONF_OPT_TEST_N, &n);
        rearrange_params[OIL_ARG_N]->value.p = GINT_TO_POINTER (n);
    }
}

static void _calc_layout_of_array (
        OilParameter *array,
        OilParameter *stride)
{
    gint m, n, header_len, footer_len;
    if (!(array && stride && array->arg_type))
        return;
    
    oil_test_config_get (
            -1,
            OIL_CONF_OPT_HEADER_LEN, &header_len,
            OIL_CONF_OPT_FOOTER_LEN, &footer_len,
            OIL_CONF_OPT_TEST_M, &m,
            OIL_CONF_OPT_TEST_N, &n,
            NULL);
    
    array->pre_n = array->prestride_length;
    if (array->prestride_var == 1)
        array->pre_n += n;
    else if (array->prestride_var == 2)
        array->pre_n += m;
    
    if (stride->value.p)
        array->stride = GPOINTER_TO_INT (stride->value.p);
    else {
        array->stride = oil_type_sizeof (array->c_type) * array->pre_n;
        stride->value.p = GINT_TO_POINTER (array->stride);
    }
    
    array->post_n = array->poststride_length;
    if (array->poststride_var == 1)
        array->post_n += n;
    else if (array->poststride_var == 2)
        array->post_n += m;
    
    array->header = header_len;
    array->footer = footer_len;
    array->size = array->stride * array->post_n + 
            array->header + array->footer;
    
    /* mark for detection of overflow */
    array->guard = (gint8) g_random_int_range (G_MININT8, G_MAXINT8 + 1);
}

static void _may_alloc_mem_of_params (
        guint n_params,
        OilParameter *params,
        OilGenericType *args,
        OilGenericType *real_args)
{
    guint i;
    
    for (i = 0; i < n_params; i++) {
        OilParameter *p = params + i;
        
        if (p->is_pointer) {
            gint8 guard = p->guard;
            gpointer a = g_try_malloc (p->size);
            
            (args + i)->p = a;
            (real_args + i)->p = (guint8 *) a + p->header;
            
            memset (a, guard, p->size);
        } else
            real_args[i] = args[i] = p->value;
    }
}

static void _may_free_mem_of_params (
        guint n_params,
        OilParameter *params,
        OilGenericType* args)
{
    guint i;
    
    for (i = 0; i < n_params; i++) {
        OilParameter *p = params + i;
        
        if (p->is_pointer)
            free ((args + i)->p);
    }
}

static void _sigill_handler (int num)
{
    if (in_try_block) {
        sigset_t set;
        
        sigemptyset (&set);
        sigaddset (&set, SIGILL);
        sigprocmask (SIG_UNBLOCK, &set, NULL);

        longjmp (jump_env, 1);
    } else {
        g_error ("illegal instruction (not in try block)");
        /* should abort */
    }
}

static void _fault_check_enable (void)
{
    if (enable_level == 0) {
        memset (&action, 0, sizeof (action));
        action.sa_handler = &_sigill_handler;
        sigaction (SIGILL, &action, &oldaction);
        
        in_try_block = 0;
    }
    enable_level++;
}

static void _fault_check_disable (void)
{
    enable_level--;
    if (enable_level == 0)
        sigaction (SIGILL, &oldaction, NULL);
}

static void _arm_test_data (
        guint n_params,
        OilParameter *params,
        OilGenericType *tdata,
        OilGenericType *src_sample)
{
    gint j;
    
    for (j = 0; j < n_params; j++) {
        OilParameter *p = params + j;
        if (p->is_pointer)
            memcpy ((tdata + j)->p, (src_sample + j)->p, p->size);
        else
            tdata[j] = src_sample[j];
    }
}

#define OIL_TRY(stmts,catch_stmts) G_STMT_START {               \
    gint ret;                                                   \
    in_try_block = 1;                                           \
    ret = setjmp (jump_env);                                    \
    if (!ret) { stmts; }                                        \
    in_try_block = 0;                                           \
                                                                \
    if (ret) { catch_stmts; }                                   \
} G_STMT_END

static gboolean _check_reference (void *ref, struct _fixture *fixture)
{
    OilClassData *cls_data = fixture->class_data;
    
    guint n_params = fixture->n_params;
    OilParameter *params = fixture->params;
    gint iterations;
    
    const OilGenericType *src_sample = \
            (const OilGenericType *) fixture->src_sample;
    OilGenericType *ref_sample = fixture->ref_sample;
    OilGenericType *real_ref_sample = fixture->real_ref_sample;

    OilChecker *checker = fixture->checker;
    OilProfiler *profiler = fixture->profiler;
    gdouble val;

    oil_test_log ("Check: reference");
    
    iterations = oil_test_config_iterations ();
    
    profiler->begin (profiler, iterations, ref);
    
    OIL_TRY (
        gint i;
        for (i = 0; i <= iterations; i++) {
            _arm_test_data (n_params, params, (OilGenericType *) src_sample, ref_sample);
            cls_data->call (ref, real_ref_sample, profiler);
        }
        ,
        /* handle ill instruction exception*/
        oil_test_log ("Critical: illegal instruction detected");
        return FALSE;
    );
    
    val = profiler->end (profiler);
    
    if (!_check_violation (n_params, params, src_sample, ref_sample))
        return FALSE;
    
    fixture->min_val = val;
    fixture->chosen_impl = NULL;
    
    return TRUE;
}

static int _check_implement (
        char *impl_name,
        void *func,
        unsigned int flags,
        void *user_data)
{
    struct _fixture *fixture = (struct _fixture *) user_data;
    OilClassData *cls_data = fixture->class_data;
    
    guint n_params = fixture->n_params;
    OilParameter *params = fixture->params;
    gint iterations;
    
    const OilGenericType *src_sample = \
            (const OilGenericType *) fixture->src_sample;
    const OilGenericType *ref_sample = \
            (const OilGenericType *) fixture->ref_sample;
    OilGenericType *tdata = fixture->tdata;
    OilGenericType *real_ref_sample = fixture->real_ref_sample;
    OilGenericType *real_tdata = fixture->real_tdata;

    OilChecker *checker = fixture->checker;
    OilProfiler *profiler = fixture->profiler;
    gdouble val;
    
    gchar *flagstr = oil_flags_to_string (flags);
    oil_test_log_printf  ("Check: implement %s %s", impl_name, flagstr);
    g_free (flagstr);
    
    /* flag check */
    if (oil_cpu_get_flags () & flags != flags) {
        oil_test_log_printf ("Ignore: not supported by current cpu");
        return 0;
    }
    
    iterations = oil_test_config_iterations ();
    
    profiler->begin (profiler, iterations, func);
    OIL_TRY (
        gint i;
        for (i = 0; i <= iterations; i++) {
            _arm_test_data (n_params, params, tdata, (OilGenericType *) src_sample);
            cls_data->call (func, real_tdata, profiler);
        }
        ,
        /* handle ill instruction exception*/
        oil_test_log ("Critical: illegal instruction detected");
        return 0;
    );
    val = profiler->end (profiler);
    
    if (!_check_violation (n_params, params, src_sample, tdata))
        return 0;
    
    if (!checker->check_result (checker, real_ref_sample, real_tdata))
        return 0;

    if (val < fixture->min_val) {
        fixture->min_val = val;
        fixture->chosen_impl = impl_name;
    }
    
    return 0;
}
#undef OIL_TRY

static gboolean _check_violation (
        guint n_params,
        OilParameter *params,
        const OilGenericType *src_data,
        OilGenericType *data)
{
    gint i;
    
    for (i = 0; i < n_params; i++) {
        OilParameter *p = params + i;
        
        if (p->is_pointer) {
            if (p->arg_class == OIL_ARG_T_SRC) {
                if (memcmp ((src_data + i)->p, (data + i)->p, p->size)) {
                    oil_test_log_printf (
                            "Critical: %d-> wrote source parameter",
                            i);
                    return FALSE;
                }
            } else {
                /* OIL_ARG_T_DST, OIL_ARG_T_INPLACE */
                guint8 *start = (guint8 *) (data + i)->p;
                guint8 *end = start + p->size;
                gint chunk_size = oil_type_sizeof (p->c_type) * p->pre_n;
                gint hole_size = p->stride - chunk_size;
                
                /* check header */
                if (!_check_guard (start, p->header, p->guard)) {
                    oil_test_log_printf (
                            "Critical: %d-> wrote before area for parameter",
                            i);
                    return FALSE;
                }
                
                /* check holes */
                if (hole_size) {
                    guint8 *a;
                    for (a = start + p->header; a != end - p->footer; a += p->stride)
                        if (!_check_guard (a + chunk_size, hole_size, p->guard)) {
                            oil_test_log_printf ("Critical: %d-> wrote holes", i);
                            return FALSE;
                        }
                }
                
                /* check footer */
                if (!_check_guard (end - p->footer, p->footer, p->guard)) {
                    oil_test_log_printf (
                            "Critical: %d-> wrote after area for parameter",
                            i);
                    return FALSE;
                }
            }
        }
    }
    
    return TRUE;
}

static gboolean _check_guard (guint8 *p, gint size, gint8 guard)
{
    gint j;
    
    for (j = 0; j < size; j++)
        if (p[j] != (guint8) guard)
            return FALSE;
    
    return TRUE;
}
