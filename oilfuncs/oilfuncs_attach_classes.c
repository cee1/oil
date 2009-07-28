/* 
 * oilfuncs_attach_classes.c:
 *  Auto generate from oilfuncs.def
 *  It will be rewritten next time run oil-regclasses
 *  Generate function that attaching class_data to classes
 */
#include <oiltester.h>
#include <oilchecker.h>
#include <oilprofiler.h>
#include "oilfuncs_marshal.h"

void oilfuncs_attach_classes ()
{

    oil_test_attach_class_full ("oil_copy8x8_u8", "uint8_t * d_8x8, int ds, uint8_t * s_8x8, int ss", OIL_MARSHAL__UINT8_Tp_INT_UINT8_Tp_INT, &oil_checker_default, &oil_profiler_default);

}
