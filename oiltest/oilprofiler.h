/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#ifndef __OIL_PROFILER_H__
#define __OIL_PROFILER_H__

#include <glib.h>
#include <oilparameter.h>

G_BEGIN_DECLS

typedef struct _OilProfiler OilProfiler;
struct _OilProfiler {
    gpointer data;
    
    void (*start) (OilProfiler *profiler);
    void (*stop) (OilProfiler *profiler);

    void (*begin) (OilProfiler *profiler, gint iterations, gpointer func);
    gdouble (*end) (OilProfiler *profiler);
};

extern OilProfiler oil_profiler_default;

void oil_profiler_begin (
        OilProfiler *profiler,
        gint iterations,
        gpointer func);
gdouble oil_profiler_end (OilProfiler *profiler);
void oil_profiler_start (OilProfiler *profiler);
void oil_profiler_stop (OilProfiler *profiler);

G_END_DECLS

#endif /* __OIL_PROFILER_H__ */
