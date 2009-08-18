#include <oilfuncs.h>
#include <stdlib.h>
#include <stdio.h>

int main ()
{
    oilfuncs_load_ini ();
    oil_class_uninit ();

    oilfuncs_load_auto ();
    oil_class_uninit ();

    return 0;
}
