/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <math.h>
#include "oilprofiler.h"
#include "oilutil.h"
#include "oiltestconfig.h"

/**
 * SECTION:profiler
 * @short_description: The profiler.
 * 
 * The profiler will be internally used like this:
 * |[
 * /&ast; The profiler is registered when calling oil_test_attach_class_full() &ast;/
 * profiler->begin (profiler, iterations, test_function);
 *
 * for (i = 0; i < iterations; i++) {
 *     profiler->start (profiler);
 *     test_function (arg1, arg2, ...);
 *     profiler->end (profiler);
 * }
 * 
 * /&ast; Gets summary &ast;/
 * gdouble average_time = profiler->end (profiler);
 * ]|
 */

/**
 * oil_profiler_default:
 * 
 * The default profiler. 
 * <note>
 * <para>
 * all member functions of the default profiler are not thread-safe.
 * </para>
 * </note>
 */
OilProfiler oil_profiler_default = {
    .begin = oil_profiler_begin,
    .end = oil_profiler_end,
    .start = oil_profiler_start,
    .stop = oil_profiler_stop,
    .data = NULL,
};

struct _profiler_data {
    gpointer func;
    gpointer timer;
    gint iterations;
    
    gdouble total;
    gdouble min;
    
    guint hist_n;
    gdouble *hist_time;
    guint *hist_count;
};

/**
 * oil_profiler_begin:
 * @profiler: the profiler
 * @iterations: the times the @func should be runned 
 * @func: the address of the function
 *
 * Arm the profiler.
 */
void oil_profiler_begin (
        OilProfiler *profiler,
        gint iterations,
        gpointer func)
{
    struct _profiler_data *data = g_try_new0 (struct _profiler_data, 1);
    
    data->func = func;
    data->timer = oil_timer_class.new ();
    data->iterations = iterations;
    data->total = 0;
    data->min = G_MAXDOUBLE;
    
    data->hist_n = 0;
    data->hist_time = g_try_new0 (gdouble, iterations);
    data->hist_count = g_try_new0 (gint, iterations);
    
    profiler->data = (gpointer) data;
}

/**
 * oil_profiler_end:
 * @profiler: the profiler
 * 
 * Get the summary of the tested function, and finalize the profiler.
 *
 * Returns: average time of the tested function spent, in seconds.
 */
gdouble oil_profiler_end (OilProfiler *profiler)
{
    struct _profiler_data *data = (struct _profiler_data *) profiler->data;
    
    gdouble ave, sum = 0, std, off;
    
    do {
        gdouble x, s2 = 0;
        gint max_i = -1, n = 0, i;

        for (i = 0; i < data->iterations; i++) {
            x = data->hist_time[i];
            s2 += x * x * data->hist_count[i];
            sum += x * data->hist_count[i];
            n += data->hist_count[i];
        
            if (data->hist_count[i] > 0) {
                if (max_i == -1 || data->hist_time[i] > data->hist_time[max_i])
                    max_i = i;
            }
        }
        
        ave = sum / n;
        std = sqrt (s2 - sum * sum / n + n * n) / (n - 1);
        off = (data->hist_time[max_i] - ave) / std;

        /* drop bad result */
        if (off > 4.0) {
            data->hist_count[max_i] = 0;
            oil_test_log_printf ("Drop: %g", data->hist_time[max_i]);
        }

    } while (off > 4.0);
    
    oil_test_log_printf ("Average: %g", ave);
    oil_test_log_printf ("Deviation.Std: %g", std);
    
    /* cleanup */
    profiler->data = NULL;
    oil_timer_class.destroy (data->timer);
    g_free (data->hist_time);
    g_free (data->hist_count);
    g_free (data);
    
    return ave;
}

/**
 * oil_profiler_start:
 * @profiler: the profiler
 *
 * Activate the profiler, do profiling
 */
void oil_profiler_start (OilProfiler *profiler)
{
    struct _profiler_data *data = (struct _profiler_data *) profiler->data;
    
    oil_timer_class.start (data->timer);
}

/**
 * oil_profiler_stop:
 * @profiler: the profiler
 *
 * Deactivate the profiler, do profiling
 */
void oil_profiler_stop (OilProfiler *profiler)
{
    struct _profiler_data *data = (struct _profiler_data *) profiler->data;
    gdouble elapsed;
    gint i;
    
    oil_timer_class.stop (data->timer);
    elapsed = oil_timer_class.elapsed (data->timer, NULL);
    
    data->total += elapsed;
    if (elapsed < data->min)
        data->min = elapsed;
    
    for (i = 0; i < data->hist_n; i++) {
        if (ABS (elapsed - data->hist_time[i]) < oil_timer_class.precise) {
            data->hist_count[i]++;
            break;
        }
    }
    /* sample not in history */
    if (i == data->hist_n) {
        data->hist_time[data->hist_n] = elapsed;
        data->hist_count[data->hist_n] = 1;
        data->hist_n++;
    }
}
