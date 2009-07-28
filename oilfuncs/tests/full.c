#include <oiltest.h>
#include <oilfuncs.h>
#include <stdlib.h>

int main ()
{
    
    oil_class_init (oil_test_destroy_class_data);
    oilfuncs_register_classes ();
    oilfuncs_attach_classes ();
    
    oilfuncs_register_implements ();
    
    oil_test_config_set (
            -1,
            OIL_CONF_OPT_SLIENT, FALSE,
            OIL_CONF_OPT_LOG_FD, 0,
            NULL);
    oil_test_optimize_class_all ();
    
    return 0;
}
