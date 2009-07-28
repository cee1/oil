/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#ifndef __OIL_TESTER_H__
#define __OIL_TESTER_H__

#include <glib.h>
#include <oilcore.h>
#include <oilparameter.h>
#include <oilchecker.h>
#include <oilprofiler.h>

G_BEGIN_DECLS

void oil_test_log (gchar *str);

typedef void (*OilProfCall) (
        gpointer func, 
        OilGenericType *args,
        OilProfiler *profiler);

void oil_test_attach_class_full (
        gchar *cls_name, 
        gchar *prototype,
        OilProfCall call,
        OilChecker *checker,
        OilProfiler *profiler);

void oil_test_destroy_class_data (void *data);

void oil_test_log (gchar *msg);
void oil_test_log_printf (gchar *format, ...);
void oil_test_log_binary (guint8 *ptr, guint n);
void oil_test_log_seed (guint32 *seed, guint seed_len);
void oil_test_log_parameter (OilParameter *param);

void oil_test_optimize_class (char *cls_name);
void oil_test_optimize_class_all ();

G_END_DECLS

#endif /* __OIL_TESTER_H__ */
