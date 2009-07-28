/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__

#define test_fail_act(expr, fail_act) do {      \
    if (!(expr)) { fail_act; }                  \
} while (0)

#if defined (__GNUC__)
#   define TEST_STRFUNC ((const char*) (__PRETTY_FUNCTION__))
#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 19901L
#   define TEST_STRFUNC ((const char*) (__func__))
#else
#   define TEST_STRFUNC ((const char*) ("???"))
#endif

#define test_assert(expr, fmt, args...) test_fail_act(expr, printf ("[%s] "fmt"\n", TEST_STRFUNC, ##args); exit (-1))
#endif

#define test_in_range(val, bottom, up) ((val) > (bottom) && (val) < (up))
