/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <glib.h>

extern void test_param ();
extern void test_testconfig ();

int main (int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);
    
    g_test_add_func ("/params", test_param);
    g_test_add_func ("/testconfig", test_testconfig);
    
    return g_test_run ();
}
