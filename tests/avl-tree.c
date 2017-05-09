#include "avl-tree.h"
#include "include/avl-tree.h"

#include <check.h>

#define init_avl_tree(tree, inplc, nds)             \
do {                                                \
    avl_tree_init(&tree, inplc, nds);               \
    ck_assert_int_eq(tree.count, 0);                \
    ck_assert_int_eq(tree.inplace, inplc);          \
    ck_assert_int_eq(tree.node_data_size, nds);     \
    ck_assert_ptr_eq(tree.root, NULL);              \
} while(0)

START_TEST(test_avl_tree_init_ok) {
    avl_tree_t t;

    init_avl_tree(t, false, 5);

    init_avl_tree(t, true, 10);
}
END_TEST

START_TEST(test_avl_tree_add_ok) {
    avl_tree_t t;
    avl_tree_node_t *atn[5];
    avl_tree_key_t k;

    init_avl_tree(t, false, 5);

    k = 0;
    atn[0] = avl_tree_add(&t, k);
    ck_assert_ptr_eq(atn[0]->host, &t);
    ck_assert_ptr_eq(atn[0]->left, NULL);
    ck_assert_ptr_eq(atn[0]->right, NULL);
    ck_assert_ptr_eq(atn[0]->parent, NULL);
    ck_assert_int_eq(atn[0]->height, 1);
    ck_assert_int_eq(atn[0]->key, k);
    ck_assert_ptr_eq(atn[0]->data, NULL);

    ck_assert_ptr_eq(t.root, atn[0]);

    k = -1;
    atn[1] = avl_tree_add(&t, k);
    ck_assert_ptr_eq(atn[1]->host, &t);
    ck_assert_ptr_eq(atn[1]->left, NULL);
    ck_assert_ptr_eq(atn[1]->right, NULL);
    ck_assert_ptr_eq(atn[1]->parent, atn[0]);
    ck_assert_int_eq(atn[1]->height, 1);
    ck_assert_int_eq(atn[1]->key, k);
    ck_assert_ptr_eq(atn[1]->data, NULL);

    ck_assert_ptr_eq(t.root, atn[0]);

    ck_assert_ptr_eq(atn[0]->left, atn[1]);
    ck_assert_int_eq(atn[0]->height, 2);

    k = 1;
    atn[2] = avl_tree_add(&t, k);
    ck_assert_ptr_eq(atn[2]->host, &t);
    ck_assert_ptr_eq(atn[2]->left, NULL);
    ck_assert_ptr_eq(atn[2]->right, NULL);
    ck_assert_ptr_eq(atn[2]->parent, atn[0]);
    ck_assert_int_eq(atn[2]->height, 1);
    ck_assert_int_eq(atn[2]->key, k);
    ck_assert_ptr_eq(atn[2]->data, NULL);

    ck_assert_ptr_eq(t.root, atn[0]);

    ck_assert_ptr_eq(atn[1]->parent, atn[0]);

    ck_assert_ptr_eq(atn[0]->left, atn[1]);
    ck_assert_ptr_eq(atn[0]->right, atn[2]);
    ck_assert_int_eq(atn[0]->height, 2);

    k = 2;
    atn[3] = avl_tree_add(&t, k);
    ck_assert_ptr_eq(atn[3]->host, &t);
    ck_assert_ptr_eq(atn[3]->left, NULL);
    ck_assert_ptr_eq(atn[3]->right, NULL);
    ck_assert_ptr_eq(atn[3]->parent, atn[2]);
    ck_assert_int_eq(atn[3]->height, 1);
    ck_assert_int_eq(atn[3]->key, k);
    ck_assert_ptr_eq(atn[3]->data, NULL);

    ck_assert_ptr_eq(t.root, atn[0]);

    ck_assert_ptr_eq(atn[1]->parent, atn[0]);
    ck_assert_ptr_eq(atn[2]->parent, atn[0]);

    ck_assert_ptr_eq(atn[0]->left, atn[1]);
    ck_assert_ptr_eq(atn[0]->right, atn[2]);

    ck_assert_ptr_eq(atn[2]->right, atn[3]);
    ck_assert_int_eq(atn[2]->height, 2);
    ck_assert_int_eq(atn[0]->height, 3);

    k = 3;
    atn[4] = avl_tree_add(&t, k);
    ck_assert_ptr_eq(atn[4]->host, &t);
    ck_assert_ptr_eq(atn[4]->left, NULL);
    ck_assert_ptr_eq(atn[4]->right, NULL);
    ck_assert_ptr_eq(atn[4]->parent, atn[3]);
    ck_assert_int_eq(atn[4]->height, 1);
    ck_assert_int_eq(atn[4]->key, k);
    ck_assert_ptr_eq(atn[4]->data, NULL);

    ck_assert_ptr_eq(t.root, atn[0]);

    ck_assert_int_eq(atn[0]->height, 3);
    ck_assert_int_eq(atn[1]->height, 1);
    ck_assert_int_eq(atn[2]->height, 1);
    ck_assert_int_eq(atn[3]->height, 2);

    ck_assert_ptr_eq(atn[0]->parent, NULL);
    ck_assert_ptr_eq(atn[0]->left, atn[1]);
    ck_assert_ptr_eq(atn[0]->right, atn[3]);

    ck_assert_ptr_eq(atn[1]->parent, atn[0]);
    ck_assert_ptr_eq(atn[1]->left, NULL);
    ck_assert_ptr_eq(atn[1]->right, NULL);

    ck_assert_ptr_eq(atn[3]->parent, atn[0]);
    ck_assert_ptr_eq(atn[3]->left, atn[2]);
    ck_assert_ptr_eq(atn[3]->right, atn[4]);

    ck_assert_ptr_eq(atn[2]->parent, atn[3]);
    ck_assert_ptr_eq(atn[2]->left, NULL);
    ck_assert_ptr_eq(atn[2]->right, NULL);

    ck_assert_ptr_eq(atn[4]->parent, atn[3]);
    ck_assert_ptr_eq(atn[4]->left, NULL);
    ck_assert_ptr_eq(atn[4]->right, NULL);

    ck_assert_int_eq(t.count, 5);

    avl_tree_purge(&t);
}
END_TEST

START_TEST(test_avl_tree_get_ok) {
    avl_tree_t t;
    avl_tree_node_t *atn[5];
    avl_tree_key_t k;

    init_avl_tree(t, false, 5);

    k = 0;
    atn[0] = avl_tree_add(&t, k);

    k = -1;
    atn[1] = avl_tree_add(&t, k);

    k = 1;
    atn[2] = avl_tree_add(&t, k);

    k = 2;
    atn[3] = avl_tree_add(&t, k);

    k = 3;
    atn[4] = avl_tree_add(&t, k);

    ck_assert_ptr_eq(avl_tree_get(&t, 0), atn[0]);
    ck_assert_ptr_eq(avl_tree_get(&t, -1), atn[1]);
    ck_assert_ptr_eq(avl_tree_get(&t, 1), atn[2]);
    ck_assert_ptr_eq(avl_tree_get(&t, 2), atn[3]);
    ck_assert_ptr_eq(avl_tree_get(&t, 3), atn[4]);

    avl_tree_purge(&t);
}
END_TEST

START_TEST(test_avl_tree_remove_ok) {
    avl_tree_t t;
    avl_tree_node_t *atn[5];
    avl_tree_key_t k;
    void *d;

    init_avl_tree(t, false, 5);

    k = 0;
    atn[0] = avl_tree_add(&t, k);
    atn[0]->data = (void *)1;

    k = -1;
    atn[1] = avl_tree_add(&t, k);
    atn[1]->data = (void *)2;

    k = 1;
    atn[2] = avl_tree_add(&t, k);
    atn[2]->data = (void *)3;

    k = 2;
    atn[3] = avl_tree_add(&t, k);
    atn[3]->data = (void *)4;

    k = 3;
    atn[4] = avl_tree_add(&t, k);
    atn[4]->data = (void *)5;

    ck_assert_int_eq(t.count, 5);

    d = avl_tree_remove_get_data(&t, 2);
    ck_assert_ptr_eq(d, (void *)4);

    ck_assert_ptr_eq(t.root, atn[0]);
    ck_assert_ptr_eq(atn[0]->parent, NULL);
    ck_assert_ptr_eq(atn[0]->left, atn[1]);
    ck_assert_ptr_eq(atn[0]->right, atn[4]);
    ck_assert_int_eq(atn[0]->height, 3);

    ck_assert_ptr_eq(atn[1]->parent, atn[0]);
    ck_assert_ptr_eq(atn[1]->left, NULL);
    ck_assert_ptr_eq(atn[1]->right, NULL);
    ck_assert_int_eq(atn[1]->height, 1);

    ck_assert_ptr_eq(atn[4]->parent, atn[0]);
    ck_assert_ptr_eq(atn[4]->left, atn[2]);
    ck_assert_ptr_eq(atn[4]->right, NULL);
    ck_assert_int_eq(atn[4]->height, 2);

    ck_assert_ptr_eq(atn[2]->parent, atn[4]);
    ck_assert_ptr_eq(atn[2]->left, NULL);
    ck_assert_ptr_eq(atn[2]->right, NULL);
    ck_assert_int_eq(atn[2]->height, 1);

    ck_assert_int_eq(t.count, 4);

    avl_tree_remove(&t, 3);

    ck_assert_ptr_eq(t.root, atn[0]);
    ck_assert_ptr_eq(atn[0]->parent, NULL);
    ck_assert_ptr_eq(atn[0]->left, atn[1]);
    ck_assert_ptr_eq(atn[0]->right, atn[2]);
    ck_assert_int_eq(atn[0]->height, 2);

    ck_assert_ptr_eq(atn[1]->parent, atn[0]);
    ck_assert_ptr_eq(atn[1]->left, NULL);
    ck_assert_ptr_eq(atn[1]->right, NULL);
    ck_assert_int_eq(atn[1]->height, 1);

    ck_assert_ptr_eq(atn[2]->parent, atn[0]);
    ck_assert_ptr_eq(atn[2]->left, NULL);
    ck_assert_ptr_eq(atn[2]->right, NULL);
    ck_assert_int_eq(atn[2]->height, 1);

    ck_assert_int_eq(t.count, 3);

#if 0
    avl_tree_remove_direct(&t, atn[0]);

    ck_assert_ptr_eq(t.root, atn[1]);
    ck_assert_ptr_eq(atn[1]->parent, NULL);
    ck_assert_ptr_eq(atn[1]->left, atn[2]);
    ck_assert_ptr_eq(atn[1]->right, NULL);
    ck_assert_ptr_eq(atn[1]->height, 2);

    ck_assert_ptr_eq(atn[2]->parent, atn[1]);
    ck_assert_ptr_eq(atn[2]->left, NULL);
    ck_assert_ptr_eq(atn[2]->right, NULL);
    ck_assert_ptr_eq(atn[2]->height, 1);

    ck_assert_int_eq(t.count, 2);
#endif

    avl_tree_purge(&t);
}
END_TEST

START_TEST(test_avl_tree_node_next_prev_min_max_ok) {
    avl_tree_t t;
    avl_tree_node_t *atn[5], *min, *max, *n;
    avl_tree_key_t k;

    init_avl_tree(t, false, 5);

    k = 0;
    atn[0] = avl_tree_add(&t, k);

    k = -1;
    atn[1] = avl_tree_add(&t, k);

    k = 1;
    atn[2] = avl_tree_add(&t, k);

    k = 2;
    atn[3] = avl_tree_add(&t, k);

    k = 3;
    atn[4] = avl_tree_add(&t, k);

    ck_assert_int_eq(t.count, 5);

    min = avl_tree_node_min(t.root);
    max = avl_tree_node_max(t.root);

    ck_assert_ptr_eq(min, atn[1]);
    ck_assert_ptr_eq(max, atn[4]);

    n = avl_tree_node_next(min);
    ck_assert_ptr_eq(n, atn[0]);

    n = avl_tree_node_prev(max);
    ck_assert_ptr_eq(n, atn[3]);

    avl_tree_purge(&t);
}
END_TEST

START_TEST(test_avl_tree_add_or_get_ok) {
    avl_tree_t t;
    avl_tree_node_t *atn[5], *n, *r;
    avl_tree_key_t k;
    bool inserted = false;

    init_avl_tree(t, false, 5);

    k = 0;
    atn[0] = avl_tree_add(&t, k);

    k = -1;
    atn[1] = avl_tree_add(&t, k);

    k = 1;
    atn[2] = avl_tree_add(&t, k);

    k = 2;
    atn[3] = avl_tree_add(&t, k);

    k = 3;
    atn[4] = avl_tree_add(&t, k);

    ck_assert_int_eq(t.count, 5);

    r = t.root;

    k = 0;
    n = avl_tree_add_or_get(&t, k, &inserted);
    ck_assert_int_eq(inserted, false);
    ck_assert_ptr_eq(n, atn[0]);
    ck_assert_int_eq(t.count, 5);
    ck_assert_ptr_eq(t.root, r);

    k = -1;
    n = avl_tree_add_or_get(&t, k, &inserted);
    ck_assert_int_eq(inserted, false);
    ck_assert_ptr_eq(n, atn[1]);
    ck_assert_int_eq(t.count, 5);
    ck_assert_ptr_eq(t.root, r);

    k = 1;
    n = avl_tree_add_or_get(&t, k, &inserted);
    ck_assert_int_eq(inserted, false);
    ck_assert_ptr_eq(n, atn[2]);
    ck_assert_int_eq(t.count, 5);
    ck_assert_ptr_eq(t.root, r);

    k = 2;
    n = avl_tree_add_or_get(&t, k, &inserted);
    ck_assert_int_eq(inserted, false);
    ck_assert_ptr_eq(n, atn[3]);
    ck_assert_int_eq(t.count, 5);
    ck_assert_ptr_eq(t.root, r);

    k = 3;
    n = avl_tree_add_or_get(&t, k, &inserted);
    ck_assert_int_eq(inserted, false);
    ck_assert_ptr_eq(n, atn[4]);
    ck_assert_int_eq(t.count, 5);
    ck_assert_ptr_eq(t.root, r);

    k = 4;
    n = avl_tree_add_or_get(&t, k, &inserted);
    ck_assert_int_eq(inserted, true);
    ck_assert_ptr_ne(n, atn[0]);
    ck_assert_ptr_ne(n, atn[1]);
    ck_assert_ptr_ne(n, atn[2]);
    ck_assert_ptr_ne(n, atn[3]);
    ck_assert_ptr_ne(n, atn[4]);
    ck_assert_int_eq(t.count, 6);

    ck_assert_ptr_eq(n->host, &t);
    ck_assert_int_eq(n->key, k);

    ck_assert_ptr_eq(t.root, atn[3]);
    ck_assert_ptr_eq(atn[3]->parent, NULL);
    ck_assert_ptr_eq(atn[3]->left, atn[0]);
    ck_assert_ptr_eq(atn[3]->right, atn[4]);

    ck_assert_ptr_eq(atn[0]->parent, atn[3]);
    ck_assert_ptr_eq(atn[0]->left, atn[1]);
    ck_assert_ptr_eq(atn[0]->right, atn[2]);

    ck_assert_ptr_eq(atn[4]->parent, atn[3]);
    ck_assert_ptr_eq(atn[4]->left, NULL);
    ck_assert_ptr_eq(atn[4]->right, n);

    ck_assert_ptr_eq(atn[1]->parent, atn[0]);
    ck_assert_ptr_eq(atn[1]->left, NULL);
    ck_assert_ptr_eq(atn[1]->right, NULL);

    ck_assert_ptr_eq(atn[2]->parent, atn[0]);
    ck_assert_ptr_eq(atn[2]->left, NULL);
    ck_assert_ptr_eq(atn[2]->right, NULL);

    ck_assert_ptr_eq(n->parent, atn[4]);
    ck_assert_ptr_eq(n->left, NULL);
    ck_assert_ptr_eq(n->right, NULL);

    avl_tree_purge(&t);
}
END_TEST

Suite *avl_tree_suite(void) {
    Suite *s;
    TCase *tc;

    s = suite_create("avl-tree");

    tc = tcase_create("avl-tree test case");

    tcase_add_test(tc, test_avl_tree_init_ok);
    tcase_add_test(tc, test_avl_tree_add_ok);
    tcase_add_test(tc, test_avl_tree_get_ok);
    tcase_add_test(tc, test_avl_tree_remove_ok);
    tcase_add_test(tc, test_avl_tree_node_next_prev_min_max_ok);
    tcase_add_test(tc, test_avl_tree_add_or_get_ok);

    suite_add_tcase(s, tc);

    return s;
}
