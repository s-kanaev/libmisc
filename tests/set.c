#include "set.h"

#include "include/set.h"
#include "include/hash-functions.h"

#include <check.h>

START_TEST(test_set_init_ok) {
    set_t s;
    set_init(&s);

    set_purge(&s);
}
END_TEST

START_TEST(test_set_add_remove_begin_end_next_prev_get_ok) {
    set_t s;
    ssize_t i, j;
    set_iterator_t it;

    set_init(&s);

    for (j = 0; j < 50; ++j)
        for (i = 0; i < 50; ++i) {
            ck_assert_int_eq(set_add(&s, i), j);
            ck_assert_int_eq(set_size(&s), j ? 50 : i + 1);
        }

    ck_assert_int_eq(set_size(&s), 50);

    for (i = 0; i < 50; ++i)
        ck_assert_int_eq(set_count(&s, i), 50);

    for (j = 0; j < 49; ++j)
        for (i = 0; i < 50; ++i) {
            ck_assert_int_eq(set_remove(&s, i), 50 - j - 1);
            ck_assert_int_eq(set_size(&s), 50);
        }

    for (i = 0; i < 100; ++i)
        ck_assert_int_eq(set_count(&s, i), i < 50);

    ck_assert_int_eq(set_size(&s), 50);

    for (it = set_begin(&s), i = 0; it.it; it = set_next(&s, it.it), ++i) {
        ck_assert_int_eq(set_get_data(it.it), i);
        ck_assert_int_eq(set_get_data(it.it), it.k);
        ck_assert_int_eq(set_data_count(it.it), 1);
        ck_assert_int_eq(set_data_count(it.it), it.count);
    }

    ck_assert_int_eq(set_size(&s), 50);

    for (it = set_end(&s), i = 49; it.it; it = set_prev(&s, it.it), --i) {
        ck_assert_int_eq(set_get_data(it.it), i);
        ck_assert_int_eq(set_get_data(it.it), it.k);
        ck_assert_int_eq(set_data_count(it.it), 1);
        ck_assert_int_eq(set_data_count(it.it), it.count);
    }

    ck_assert_int_eq(set_size(&s), 50);

    for (i = 0; i < 50; ++i) {
        ck_assert_int_eq(set_remove(&s, i), 0);
        ck_assert_int_eq(set_size(&s), 50 - i - 1);
    }

    ck_assert_int_eq(set_size(&s), 0);

    set_purge(&s);
}
END_TEST

Suite *set_suite(void) {
    Suite *s;
    TCase *tc;

    s = suite_create("set");

    tc = tcase_create("set");

    tcase_add_test(tc, test_set_init_ok);
    tcase_add_test(tc, test_set_add_remove_begin_end_next_prev_get_ok);

    suite_add_tcase(s, tc);

    return s;
}
