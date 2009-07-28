/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <glib.h>
#include <oiltest.h>

static void print_config ();

void test_testconfig ()
{
    gint fd, fd2;
    gint header_len, footer_len, header_len2, footer_len2;
    gint test_m, test_n, test_m2, test_n2;
    gint iterations, iterations2;
    gdouble tl, th, tl2, th2;
    gchar *seed, *seed2;
    gboolean slient, slient2;
    
    print_config ();
    
    oil_test_config_get (
            -1,
            OIL_CONF_OPT_LOG_FD, &fd,
            OIL_CONF_OPT_SEED, &seed,
            OIL_CONF_OPT_HEADER_LEN, &header_len,
            OIL_CONF_OPT_FOOTER_LEN, &footer_len,
            OIL_CONF_OPT_TEST_M, &test_m,
            OIL_CONF_OPT_TEST_N, &test_n,
            OIL_CONF_OPT_SLIENT, &slient,
            OIL_CONF_OPT_ITERATIONS, &iterations,
            OIL_CONF_OPT_TOLERANCE_HIGH, &th,
            OIL_CONF_OPT_TOLERANCE_LOW, &tl,
            NULL
    );
    g_assert (oil_test_config_height () == 1);
    g_assert (slient == oil_test_config_slient ());
    g_assert_cmpint (fd, ==, oil_test_config_logfd ());
    g_assert_cmpfloat (th, ==, oil_test_config_tolerance_high ());
    g_assert_cmpfloat (tl, ==, oil_test_config_tolerance_low ());
    g_assert (oil_test_config_rand () != NULL);
    
    {
#define SEED_STR "R02S0e820092b8961870982f83f37bd3564b"

        gint fd = 2;
        gint header_len = 200, footer_len = 300;
        gint test_m = 300, test_n = 700;
        gint iterations = 10000;
        gdouble tl = 0.01, th = 0.5;
        gchar *seed =SEED_STR;
        gboolean slient = TRUE;
        
        oil_test_config_push (0);
        oil_test_config_set (
                -1,
                OIL_CONF_OPT_LOG_FD, fd,
                OIL_CONF_OPT_SEED, seed,
                OIL_CONF_OPT_HEADER_LEN, header_len,
                OIL_CONF_OPT_FOOTER_LEN, footer_len,
                OIL_CONF_OPT_TEST_M, test_m,
                OIL_CONF_OPT_TEST_N, test_n,
                OIL_CONF_OPT_SLIENT, slient,
                OIL_CONF_OPT_ITERATIONS, iterations,
                OIL_CONF_OPT_TOLERANCE_HIGH, th,
                OIL_CONF_OPT_TOLERANCE_LOW, tl,
                NULL
        );
        g_assert (oil_test_config_height () == 2);
        
        oil_test_config_push (0);
        g_assert (oil_test_config_height () == 3);
        oil_test_config_get (
                -1,
                OIL_CONF_OPT_LOG_FD, &fd2,
                OIL_CONF_OPT_SEED, &seed2,
                OIL_CONF_OPT_HEADER_LEN, &header_len2,
                OIL_CONF_OPT_FOOTER_LEN, &footer_len2,
                OIL_CONF_OPT_TEST_M, &test_m2,
                OIL_CONF_OPT_TEST_N, &test_n2,
                OIL_CONF_OPT_SLIENT, &slient2,
                OIL_CONF_OPT_ITERATIONS, &iterations2,
                OIL_CONF_OPT_TOLERANCE_HIGH, &th2,
                OIL_CONF_OPT_TOLERANCE_LOW, &tl2,
                NULL
        );
        g_assert (slient == slient2);
        g_assert_cmpint (test_m, ==, test_m2);
        g_assert_cmpint (test_n, ==, test_n2);
        g_assert_cmpint (header_len, ==, header_len2);
        g_assert_cmpint (footer_len, ==, footer_len2);
        g_assert_cmpint (iterations, ==, iterations2);
        g_assert_cmpint (fd, ==, fd2);
        g_assert_cmpfloat (th, ==, th2);
        g_assert_cmpfloat (tl, ==, tl2);
        g_assert_cmpstr (seed, ==, seed2);
        g_assert (oil_test_config_rand () != NULL);
    }
    
    oil_test_config_pop ();
    g_assert (oil_test_config_height () == 2);
    oil_test_config_pop ();
    g_assert (oil_test_config_height () == 1);
    
    oil_test_config_get (
            -1,
            OIL_CONF_OPT_LOG_FD, &fd2,
            OIL_CONF_OPT_SEED, &seed2,
            OIL_CONF_OPT_HEADER_LEN, &header_len2,
            OIL_CONF_OPT_FOOTER_LEN, &footer_len2,
            OIL_CONF_OPT_TEST_M, &test_m2,
            OIL_CONF_OPT_TEST_N, &test_n2,
            OIL_CONF_OPT_SLIENT, &slient2,
            OIL_CONF_OPT_ITERATIONS, &iterations2,
            OIL_CONF_OPT_TOLERANCE_HIGH, &th2,
            OIL_CONF_OPT_TOLERANCE_LOW, &tl2,
            NULL
        );
    g_assert (slient == slient2);
    g_assert_cmpint (test_m, ==, test_m2);
    g_assert_cmpint (test_n, ==, test_n2);
    g_assert_cmpint (header_len, ==, header_len2);
    g_assert_cmpint (footer_len, ==, footer_len2);
    g_assert_cmpint (iterations, ==, iterations2);
    g_assert_cmpint (fd, ==, fd2);
    g_assert_cmpfloat (th, ==, th2);
    g_assert_cmpfloat (tl, ==, tl2);
    g_assert (seed == seed2);
    g_assert (oil_test_config_rand () != NULL);
    
    oil_test_config_pop_all ();
    g_assert (oil_test_config_height () == 0);
}

static void print_config ()
{
    gint fd;
    gint header_len, footer_len;
    gint test_m, test_n;
    gint iterations;
    gdouble tl, th;
    gchar *seed;
    gboolean slient;
    
    oil_test_config_get (
            -1,
            OIL_CONF_OPT_LOG_FD, &fd,
            OIL_CONF_OPT_SEED, &seed,
            OIL_CONF_OPT_HEADER_LEN, &header_len,
            OIL_CONF_OPT_FOOTER_LEN, &footer_len,
            OIL_CONF_OPT_TEST_M, &test_m,
            OIL_CONF_OPT_TEST_N, &test_n,
            OIL_CONF_OPT_SLIENT, &slient,
            OIL_CONF_OPT_ITERATIONS, &iterations,
            OIL_CONF_OPT_TOLERANCE_HIGH, &th,
            OIL_CONF_OPT_TOLERANCE_LOW, &tl,
            NULL
    );
    
    g_print ("log_fd: %d, seed: \"%s\"\n", fd, seed);
    g_print ("header_len: %d, footer_len: %d\n", header_len, footer_len);
    g_print ("test_m: %d, test_n: %d\n", test_m, test_n);
    g_print ("slient: %s, iterations: %d\n", slient ? "True": " False", iterations);
    g_print ("Tolerance: High %f, Low %f\n", th, tl);
}
