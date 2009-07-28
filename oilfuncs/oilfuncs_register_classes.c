/* 
 * oilfuncs_register_classes.c:
 *  Auto generate from oilfuncs.def
 *  It will be rewritten next time run oil-regclasses
 *  Generate the class registration function
 */
#include <oilclass.h>
#include <inttypes.h>
void oilfuncs_register_classes ()
{

    extern void oil_copy8x8_u8_ref (uint8_t * d_8x8, int ds, uint8_t * s_8x8, int ss);
    extern void (*oil_copy8x8_u8) (uint8_t * d_8x8, int ds, uint8_t * s_8x8, int ss);
    oil_class_register ("oil_copy8x8_u8", oil_copy8x8_u8_ref, (void **) & oil_copy8x8_u8);

}
