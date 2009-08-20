/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * not MT safe
 */

#include "oiltestconfig.h"
#include "oiltestlog.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>


/**
 * SECTION:test_log
 * @short_description: logging function used in test
 * 
 * These logging functions output messages in "header:body\n" text format.
 * 
 * To enable logging, you need to set silent_mode to "FALSE" and indicate a logfd
 * through oil_test_config_set()
 */

/**
 * oil_test_log:
 * @msg: the log message
 *
 * Append a "\n" to @msg, and output it.
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
 * Output the binary data in hex.
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
 * Output an #OilParameter.
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

