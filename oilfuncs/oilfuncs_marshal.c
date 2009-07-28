/* 
 * oilfuncs_marshal.c:
 *  Auto generate from oilfuncs.def
 *  It will be rewritten next time run oil-regclasses
 * 
 *  declares marshal functions for auto testing
 */
 
#include "oilfuncs_marshal.h"

void OIL_MARSHAL__UINT8_Tp_INT_UINT8_Tp_INT (void *func, OilGenericType *args, OilProfiler *profiler)
{
    typedef void (*prototype) (uint8_t *, int, uint8_t *, int);
    prototype _f = (prototype) func;
    uint8_t * _0 = (uint8_t *) (args + 0)->p;
    int _1 = (int) (args + 1)->p;
    uint8_t * _2 = (uint8_t *) (args + 2)->p;
    int _3 = (int) (args + 3)->p;

    profiler->start (profiler);
    _f (_0, _1, _2, _3);
    profiler->stop (profiler);
}

