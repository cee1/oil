/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#ifndef __OIL_TEST_CONFIG_H__
#define __OIL_TEST_CONFIG_H__
#include <glib.h>

G_BEGIN_DECLS

enum {
    OIL_CONF_OPT_0 = 0,
    OIL_CONF_OPT_LOG_FD,
    OIL_CONF_OPT_SEED,
    OIL_CONF_OPT_HEADER_LEN,
    OIL_CONF_OPT_FOOTER_LEN,
    OIL_CONF_OPT_TEST_M,
    OIL_CONF_OPT_TEST_N,
    OIL_CONF_OPT_SLIENT,
    OIL_CONF_OPT_ITERATIONS,
    OIL_CONF_OPT_TOLERANCE_HIGH,
    OIL_CONF_OPT_TOLERANCE_LOW,
} ;

guint oil_test_config_height ();
void oil_test_config_get (gint n_params, ...);
gint oil_test_config_iterations ();
gint oil_test_config_logfd ();
gboolean oil_test_config_slient ();
GRand *oil_test_config_rand ();
gdouble oil_test_config_tolerance_high ();
gdouble oil_test_config_tolerance_low ();

void oil_test_config_set (gint n_params, ...);
void oil_test_config_vget (gint n_params, va_list va_args);
void oil_test_config_vset (gint n_params, va_list va_args);
void oil_test_config_push (gint n_params, ...);
void oil_test_config_pop ();
void oil_test_config_pop_all ();

G_END_DECLS

#endif /* __OIL_TEST_CONFIG_H__ */
