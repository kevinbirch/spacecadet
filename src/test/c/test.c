
#include <stdlib.h>
#include <check.h>

#include "test.h"

int main(void)
{
    SRunner *runner = srunner_create(hashtable_suite());
    srunner_add_suite(runner, vector_suite());
    
    srunner_run_all(runner, CK_NORMAL);

    int failures = srunner_ntests_failed(runner);
    srunner_free(runner);
    
    return 0 == failures ? EXIT_SUCCESS : EXIT_FAILURE;
}
