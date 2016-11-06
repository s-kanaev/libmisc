#include "containers.h"
#include "avl-tree.h"
#include "hash-map.h"
#include "set.h"

#include <check.h>
#include <stdlib.h>

int main(/*int argc, char *argv[]*/) {
    int nfailed = 0;

    Suite *s;
    SRunner *runner;

    s = containers_suite();
    runner = srunner_create(s);

    s = avl_tree_suite();
    srunner_add_suite(runner, s);

    s = hash_map_suite();
    srunner_add_suite(runner, s);

    s = set_suite();
    srunner_add_suite(runner, s);

    srunner_run_all(runner, CK_NORMAL);
    nfailed = srunner_ntests_failed(runner);

    srunner_free(runner);

    return nfailed ? EXIT_FAILURE : EXIT_SUCCESS;
}
