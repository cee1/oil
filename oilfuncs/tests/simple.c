#include <oilfuncs.h>
#include <stdlib.h>

int main ()
{

    oil_class_init (NULL);
    oilfuncs_register_classes ();
    oilfuncs_register_implements ();
    
    return 0;
}
