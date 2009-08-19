#include <oilfuncs.h>
#include <stdlib.h>
#include <stdio.h>

int main ()
{
    oilfuncs_init_from_ini ();
    oil_class_uninit ();

    oilfuncs_init_auto ();
    oil_class_uninit ();

    return 0;
}
