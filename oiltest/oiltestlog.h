/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#ifndef __OIL_TESTLOG_H__
#define __OIL_TESTLOG_H__

#include <glib.h>
#include <oilparameter.h>

G_BEGIN_DECLS

void oil_test_log (gchar *msg);
void oil_test_log_printf (gchar *format, ...);
void oil_test_log_binary (guint8 *ptr, guint n);
void oil_test_log_parameter (OilParameter *param);

G_END_DECLS

#endif /* __OIL_TESTLOG_H__ */
