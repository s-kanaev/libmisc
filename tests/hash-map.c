#include "hash-map.h"
#include "include/hash-map.h"
#include "include/hash-functions.h"

#include <check.h>

START_TEST(test_hash_map_init_ok) {
    hash_map_t hm;

    hash_map_init(&hm, hash_pearson, hash_update_pearson);

    ck_assert_ptr_eq(hm.hasher, hash_pearson);
    ck_assert_ptr_eq(hm.hash_updater, hash_update_pearson);

    hash_map_purge(&hm);
}
END_TEST

START_TEST(test_hash_map_add_get_remove_ok) {
    hash_map_t hm;
    hash_map_node_t *hmn[50], *hmn_tst;
    int i;

    hash_map_init(&hm, hash_pearson, hash_update_pearson);

    for (i = 0; i < 50; ++i) {
        hmn[i] = hash_map_add(&hm, i);

        ck_assert_int_eq(hash_map_size(&hm), i + 1);
        ck_assert_ptr_ne(hmn[i], NULL);

        ck_assert_int_eq(hash_map_node_size(hmn[i]), 0);
    }

    for (i = 0; i < 50; ++i) {
        hmn_tst = hash_map_get(&hm, i);

        ck_assert_ptr_eq(hmn[i], hmn_tst);
    }

    for (i = 0; i < 50; ++i) {
        hash_map_remove(&hm, i);

        ck_assert_int_eq(hash_map_size(&hm), 50 - i - 1);
    }

    hash_map_purge(&hm);
}
END_TEST

START_TEST(test_hash_map_begin_end_next_prev_ok) {
    hash_map_t hm;
    hash_map_node_t *hmn, *hmn_tst, *hmn2;

    hash_map_init(&hm, hash_pearson, hash_update_pearson);

    hmn = hash_map_add(&hm, 1);

    hmn2 = hash_map_add(&hm, 2);

    hmn_tst = hash_map_begin(&hm);
    ck_assert_ptr_eq(hmn_tst, hmn);

    hmn_tst = hash_map_next(&hm, hmn_tst);
    ck_assert_ptr_eq(hmn_tst, hmn2);

    hmn_tst = hash_map_next(&hm, hmn_tst);
    ck_assert_ptr_eq(hmn_tst, NULL);

    hmn_tst = hash_map_end(&hm);
    ck_assert_ptr_eq(hmn_tst, hmn2);

    hmn_tst = hash_map_prev(&hm, hmn_tst);
    ck_assert_ptr_eq(hmn_tst, hmn);

    hmn_tst = hash_map_prev(&hm, hmn_tst);
    ck_assert_ptr_eq(hmn_tst, NULL);

    hash_map_purge(&hm);
}
END_TEST

START_TEST(test_hash_map_node_full_ok) {
    hash_map_t hm;
    hash_map_node_t *hmn;
    hash_map_node_element_t *el[50], *el_tst;
    hash_map_node_data_t hmnd, *hmnd_tst, *hmnd2;
    ssize_t i;

    hash_map_init(&hm, hash_pearson, hash_update_pearson);

    hmn = hash_map_add(&hm, 1);

    for (i = 0; i < 50; ++i) {
        hmnd.data = (void *)i;
        hmnd.size = i;

        el[i] = hash_map_node_add(hmn, hmnd);

        ck_assert_int_eq(hash_map_node_size(hmn), i + 1);

        hmnd_tst = (hash_map_node_data_t *)hash_map_node_element_get_data(el[i]);

        ck_assert_ptr_eq(hmnd_tst->data, hmnd.data);
        ck_assert_int_eq(hmnd_tst->size, hmnd.size);
    }

    for (i = 0, el_tst = hash_map_node_begin(hmn);
         i < 50; ++i, el_tst = hash_map_node_next(hmn, el_tst)) {
        ck_assert_ptr_eq(el_tst, el[i]);

        hmnd_tst = (hash_map_node_data_t *)hash_map_node_element_get_data(el_tst);
        hmnd2 = (hash_map_node_data_t *)hash_map_node_element_get_data(el[i]);

        ck_assert_ptr_eq(hmnd_tst->data, hmnd2->data);
        ck_assert_int_eq(hmnd_tst->size, hmnd2->size);
    }

    for (i = 49, el_tst = hash_map_node_end(hmn);
         i >= 0; --i, el_tst = hash_map_node_prev(hmn, el_tst)) {
        ck_assert_ptr_eq(el_tst, el[i]);

        hmnd_tst = (hash_map_node_data_t *)hash_map_node_element_get_data(el_tst);
        hmnd2 = (hash_map_node_data_t *)hash_map_node_element_get_data(el[i]);

        ck_assert_ptr_eq(hmnd_tst->data, hmnd2->data);
        ck_assert_int_eq(hmnd_tst->size, hmnd2->size);
    }

    for (i = 0; i < 25; ++i) {
        hash_map_node_remove(hmn, el[i]);

        ck_assert_int_eq(hash_map_node_size(hmn), 50 - i - 1);
    }

    ck_assert_int_eq(hash_map_node_size(hmn), 25);

    for (i = 0, el_tst = hash_map_node_begin(hmn);
         i < 25; ++i, el_tst = hash_map_node_next(hmn, el_tst)) {
        ck_assert_ptr_eq(el_tst, el[25 + i]);

        hmnd_tst = (hash_map_node_data_t *)hash_map_node_element_get_data(el_tst);
        hmnd2 = (hash_map_node_data_t *)hash_map_node_element_get_data(el[25 + i]);

        ck_assert_ptr_eq(hmnd_tst->data, hmnd2->data);
        ck_assert_int_eq(hmnd_tst->size, hmnd2->size);
    }

    for (i = 49, el_tst = hash_map_node_end(hmn);
         i >= 25; --i, el_tst = hash_map_node_prev(hmn, el_tst)) {
        ck_assert_ptr_eq(el_tst, el[i]);

        hmnd_tst = (hash_map_node_data_t *)hash_map_node_element_get_data(el_tst);
        hmnd2 = (hash_map_node_data_t *)hash_map_node_element_get_data(el[i]);

        ck_assert_ptr_eq(hmnd_tst->data, hmnd2->data);
        ck_assert_int_eq(hmnd_tst->size, hmnd2->size);
    }

    hash_map_purge(&hm);
}
END_TEST

Suite *hash_map_suite(void) {
    Suite *s;
    TCase *tc;

    s = suite_create("hash map");

    tc = tcase_create("hash map");

    tcase_add_test(tc, test_hash_map_init_ok);
    tcase_add_test(tc, test_hash_map_add_get_remove_ok);
    tcase_add_test(tc, test_hash_map_begin_end_next_prev_ok);

    suite_add_tcase(s, tc);

    tc = tcase_create("hash map node");

    tcase_add_test(tc, test_hash_map_node_full_ok);

    suite_add_tcase(s, tc);

    return s;
}
