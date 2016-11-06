#include "containers.h"
#include "include/containers.h"

#include <check.h>

struct el {
    int i;
    char j;
};

#define initialize_vector(vct,cnt)                                  \
do {                                                                \
    vector_init(&vct, sizeof(struct el), cnt);                      \
    ck_assert_int_eq(vct.count, cnt);                               \
    ck_assert_int_eq(vct.element_size, sizeof(struct el));          \
    ck_assert_ptr_ne(vct.data.data, NULL);                          \
    ck_assert_int_ge(vct.data.user_size, cnt * sizeof(struct el));  \
} while(0)

#define initialize_list(lst,inpl)                                   \
do {                                                                \
    list_init(&lst, inpl, sizeof(struct el));                       \
    ck_assert_int_eq(lst.count, 0);                                 \
    ck_assert_int_eq(lst.element_size, sizeof(struct el));          \
    ck_assert_int_eq(lst.inplace, inpl);                            \
    ck_assert_ptr_eq(lst.front, list_begin(&lst));                  \
    ck_assert_ptr_eq(lst.back, list_end(&lst));                     \
} while(0)

START_TEST(test_vector_init_ok) {
    vector_t v;

    initialize_vector(v, 17);

    vector_deinit(&v);
}
END_TEST

START_TEST(test_vector_remove_ok) {
    vector_t v;
    struct el *el;
    int i = 0;
    int sum = 0;

    initialize_vector(v, 17);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el), ++i) {
        el->i = 0x01 << i;
        sum += 0x01 << i;
    }

    vector_remove(&v, 2);

    ck_assert_int_eq(v.count, 16);
    ck_assert_int_eq(v.element_size, sizeof(struct el));
    ck_assert_ptr_ne(v.data.data, NULL);
    ck_assert_int_ge(v.data.user_size, 16 * sizeof(struct el));

    el = vector_get(&v, 2);

    ck_assert_int_ne(el->i, 0x01 << 2);
    ck_assert_int_eq(el->i, 0x01 << 3);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el)) {
        sum -= el->i;
    }

    ck_assert_int_eq(sum, 0x01 << 2);

    vector_deinit(&v);
}
END_TEST

START_TEST(test_vector_remove_range_ok) {
    vector_t v;
    struct el *el;
    int i = 0;
    int sum = 0;

    initialize_vector(v, 17);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el), ++i) {
        el->i = 0x01 << i;
        sum += 0x01 << i;
    }

    vector_remove_range(&v, 2, 3);

    ck_assert_int_eq(v.count, 14);
    ck_assert_int_eq(v.element_size, sizeof(struct el));
    ck_assert_ptr_ne(v.data.data, NULL);
    ck_assert_int_ge(v.data.user_size, 14 * sizeof(struct el));

    el = vector_get(&v, 2);
    ck_assert_int_ne(el->i, 0x01 << 2);
    ck_assert_int_eq(el->i, 0x01 << 5);

    el = vector_next(&v, el);
    ck_assert_int_ne(el->i, 0x01 << 3);
    ck_assert_int_eq(el->i, 0x01 << 6);

    el = vector_next(&v, el);
    ck_assert_int_ne(el->i, 0x01 << 4);
    ck_assert_int_eq(el->i, 0x01 << 7);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el)) {
        sum -= el->i;
    }

    ck_assert_int_eq(sum, (0x01 << 2) + (0x01 << 3) + (0x01 << 4));

    vector_deinit(&v);
}
END_TEST

START_TEST(test_vector_insert_ok) {
    vector_t v;
    struct el *el;
    int i = 0;
    int sum = 0;

    initialize_vector(v, 17);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el), ++i) {
        el->i = 0x01 << i;
        sum += 0x01 << i;
    }

    el = vector_insert(&v, 2);
    el->i = 0x01 << 17;

    ck_assert_int_eq(v.count, 18);
    ck_assert_int_eq(v.element_size, sizeof(struct el));
    ck_assert_ptr_ne(v.data.data, NULL);
    ck_assert_int_ge(v.data.user_size, 18 * sizeof(struct el));

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el)) {
        sum -= el->i;
    }

    ck_assert_int_eq(sum, -(0x01 << 17));

    vector_deinit(&v);
}
END_TEST

START_TEST(test_vector_append_ok) {
    vector_t v;
    struct el *el;
    int i = 0;
    int sum = 0;

    initialize_vector(v, 17);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el), ++i) {
        el->i = 0x01 << i;
        sum += 0x01 << i;
    }

    el = vector_append(&v);
    el->i = 0x01 << 17;

    ck_assert_int_eq(v.count, 18);
    ck_assert_int_eq(v.element_size, sizeof(struct el));
    ck_assert_ptr_ne(v.data.data, NULL);
    ck_assert_int_ge(v.data.user_size, 18 * sizeof(struct el));

    el = vector_get(&v, 17);
    ck_assert_int_eq(el->i, 0x01 << 17);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el)) {
        sum -= el->i;
    }

    ck_assert_int_eq(sum, -(0x01 << 17));

    vector_deinit(&v);
}
END_TEST

START_TEST(test_vector_prepend_ok) {
    vector_t v;
    struct el *el;
    int i = 0;
    int sum = 0;

    initialize_vector(v, 17);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el), ++i) {
        el->i = 0x01 << i;
        sum += 0x01 << i;
    }

    el = vector_prepend(&v);
    el->i = 0x01 << 17;

    ck_assert_int_eq(v.count, 18);
    ck_assert_int_eq(v.element_size, sizeof(struct el));
    ck_assert_ptr_ne(v.data.data, NULL);
    ck_assert_int_ge(v.data.user_size, 18 * sizeof(struct el));

    el = vector_get(&v, 0);
    ck_assert_int_eq(el->i, 0x01 << 17);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el)) {
        sum -= el->i;
    }

    ck_assert_int_eq(sum, -(0x01 << 17));

    vector_deinit(&v);
}
END_TEST

START_TEST(test_vector_begin_end_get_next_prev_ok) {
    vector_t v;
    struct el *el;
    int i = 0;
    int sum = 0;

    initialize_vector(v, 17);

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el), ++i) {
        el->i = 0x01 << i;
        sum += 0x01 << i;
    }

    ck_assert_ptr_eq(vector_get(&v, 0), vector_begin(&v));
    ck_assert_ptr_ne(vector_get(&v, 16), vector_end(&v));

    for (el = vector_begin(&v), i = 0;
         el != vector_end(&v); el = vector_next(&v, el), ++i) {
        ck_assert_int_eq(el->i, 0x01 << i);
    }

    for (i = 0;
         (unsigned)i < v.count; ++i) {
        el = vector_get(&v, i);
        ck_assert_int_eq(el->i, 0x01 << i);
    }

    for (el = vector_begin(&v);
         el != vector_end(&v); el = vector_next(&v, el)) {
        sum -= el->i;
    }

    ck_assert_int_eq(sum, 0);

    for (el = vector_get(&v, 16), i = 16;
         el != vector_begin(&v); el = vector_prev(&v, el), --i) {
        ck_assert_int_eq(el->i, 0x01 << i);
    }

    ck_assert_int_eq(i, 0);

    vector_deinit(&v);
}
END_TEST

START_TEST(test_list_init_ok) {
    list_t l;
    list_element_t *init_back, *init_front;

    initialize_list(l, true);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    ck_assert_ptr_eq(init_back, NULL);
    ck_assert_ptr_eq(init_front, NULL);

    list_purge(&l);

    initialize_list(l, false);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    ck_assert_ptr_eq(init_back, NULL);
    ck_assert_ptr_eq(init_front, NULL);

    list_purge(&l);
}
END_TEST

START_TEST(test_list_prepend_inplace_ok) {
    list_t l;
    list_element_t *lel[3];
    list_element_t *init_front, *init_back;
    struct el *element;

    initialize_list(l, true);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    /* inplace */
    lel[0] = list_prepend(&l);

    ck_assert_int_eq(l.count, 1);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[0]->host);
    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, init_back);

    element = lel[0]->data;
    element->i = 0x00;

    lel[1] = list_prepend(&l);

    ck_assert_int_eq(l.count, 2);
    ck_assert_ptr_eq(l.front, lel[1]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[1]->host);

    ck_assert_ptr_eq(lel[1]->prev, init_front);
    ck_assert_ptr_eq(lel[1]->next, lel[0]);

    ck_assert_ptr_eq(lel[0]->prev, lel[1]);
    ck_assert_ptr_eq(lel[0]->next, init_back);

    ck_assert_ptr_ne(lel[0]->data, lel[1]->data);

    element = lel[1]->data;
    element->i = 0x01;

    lel[2] = list_prepend(&l);

    ck_assert_int_eq(l.count, 3);
    ck_assert_ptr_eq(l.front, lel[2]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[2]->host);

    ck_assert_ptr_eq(lel[2]->prev, init_front);
    ck_assert_ptr_eq(lel[2]->next, lel[1]);

    ck_assert_ptr_eq(lel[1]->prev, lel[2]);
    ck_assert_ptr_eq(lel[1]->next, lel[0]);

    ck_assert_ptr_eq(lel[0]->prev, lel[1]);
    ck_assert_ptr_eq(lel[0]->next, init_back);

    ck_assert_ptr_ne(lel[0]->data, lel[2]->data);
    ck_assert_ptr_ne(lel[1]->data, lel[2]->data);

    element = lel[2]->data;
    element->i = 0x02;

    list_purge(&l);
}
END_TEST

START_TEST(test_list_prepend_noninplace_ok) {
    list_t l;
    list_element_t *lel[3];
    list_element_t *init_front, *init_back;

    initialize_list(l, false);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    /* non-inplace */
    lel[0] = list_prepend(&l);

    ck_assert_int_eq(l.count, 1);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[0]->host);
    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_eq(lel[0]->data, NULL);

    lel[1] = list_prepend(&l);

    ck_assert_int_eq(l.count, 2);
    ck_assert_ptr_eq(l.front, lel[1]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[1]->host);

    ck_assert_ptr_eq(lel[1]->prev, init_front);
    ck_assert_ptr_eq(lel[1]->next, lel[0]);

    ck_assert_ptr_eq(lel[0]->prev, lel[1]);
    ck_assert_ptr_eq(lel[0]->next, init_back);

    ck_assert_ptr_eq(lel[0]->data, lel[1]->data);

    lel[2] = list_prepend(&l);

    ck_assert_int_eq(l.count, 3);
    ck_assert_ptr_eq(l.front, lel[2]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[2]->host);

    ck_assert_ptr_eq(lel[2]->prev, init_front);
    ck_assert_ptr_eq(lel[2]->next, lel[1]);

    ck_assert_ptr_eq(lel[1]->prev, lel[2]);
    ck_assert_ptr_eq(lel[1]->next, lel[0]);

    ck_assert_ptr_eq(lel[0]->prev, lel[1]);
    ck_assert_ptr_eq(lel[0]->next, init_back);

    ck_assert_ptr_eq(lel[0]->data, lel[2]->data);
    ck_assert_ptr_eq(lel[1]->data, lel[2]->data);

    list_purge(&l);
}
END_TEST

START_TEST(test_list_append_inplace_ok) {
    list_t l;
    list_element_t *lel[3];
    list_element_t *init_front, *init_back;
    struct el *element;

    initialize_list(l, true);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    /* inplace */
    lel[0] = list_append(&l);

    ck_assert_int_eq(l.count, 1);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[0]->host);
    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_ne(lel[0]->data, NULL);

    element = lel[0]->data;
    element->i = 0x00;

    lel[1] = list_append(&l);

    ck_assert_int_eq(l.count, 2);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[1]);
    ck_assert_ptr_eq(&l, lel[1]->host);

    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, lel[1]);

    ck_assert_ptr_eq(lel[1]->prev, lel[0]);
    ck_assert_ptr_eq(lel[1]->next, init_back);

    ck_assert_ptr_ne(NULL, lel[1]->data);
    ck_assert_ptr_ne(lel[0]->data, lel[1]->data);

    element = lel[1]->data;
    element->i = 0x01;

    lel[2] = list_append(&l);

    ck_assert_int_eq(l.count, 3);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[2]);
    ck_assert_ptr_eq(&l, lel[2]->host);

    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, lel[1]);

    ck_assert_ptr_eq(lel[1]->prev, lel[0]);
    ck_assert_ptr_eq(lel[1]->next, lel[2]);

    ck_assert_ptr_eq(lel[2]->prev, lel[1]);
    ck_assert_ptr_eq(lel[2]->next, init_back);

    ck_assert_ptr_ne(NULL, lel[2]->data);
    ck_assert_ptr_ne(lel[0]->data, lel[2]->data);
    ck_assert_ptr_ne(lel[1]->data, lel[2]->data);

    element = lel[2]->data;
    element->i = 0x02;

    list_purge(&l);
}
END_TEST

START_TEST(test_list_append_noninplace_ok) {
    list_t l;
    list_element_t *lel[3];
    list_element_t *init_front, *init_back;

    initialize_list(l, false);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    /* inplace */
    lel[0] = list_append(&l);

    ck_assert_int_eq(l.count, 1);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[0]->host);
    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_eq(lel[0]->data, NULL);

    lel[1] = list_append(&l);

    ck_assert_int_eq(l.count, 2);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[1]);
    ck_assert_ptr_eq(&l, lel[1]->host);

    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, lel[1]);

    ck_assert_ptr_eq(lel[1]->prev, lel[0]);
    ck_assert_ptr_eq(lel[1]->next, init_back);

    ck_assert_ptr_eq(NULL, lel[1]->data);
    ck_assert_ptr_eq(lel[0]->data, lel[1]->data);

    lel[2] = list_append(&l);

    ck_assert_int_eq(l.count, 3);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[2]);
    ck_assert_ptr_eq(&l, lel[2]->host);

    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, lel[1]);

    ck_assert_ptr_eq(lel[1]->prev, lel[0]);
    ck_assert_ptr_eq(lel[1]->next, lel[2]);

    ck_assert_ptr_eq(lel[2]->prev, lel[1]);
    ck_assert_ptr_eq(lel[2]->next, init_back);

    ck_assert_ptr_eq(NULL, lel[2]->data);
    ck_assert_ptr_eq(lel[0]->data, lel[2]->data);
    ck_assert_ptr_eq(lel[1]->data, lel[2]->data);

    list_purge(&l);
}
END_TEST

START_TEST(test_list_add_after_inplace_ok) {
    list_t l;
    list_element_t *lel[3];
    list_element_t *init_front, *init_back;

    initialize_list(l, true);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    lel[0] = list_add_after(&l, list_begin(&l));

    ck_assert_int_eq(l.count, 1);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[0]->host);
    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_ne(lel[0]->data, NULL);

    lel[1] = list_add_after(&l, lel[0]);

    ck_assert_int_eq(l.count, 2);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[1]);
    ck_assert_ptr_eq(&l, lel[1]->host);
    ck_assert_ptr_eq(lel[0]->next, lel[1]);
    ck_assert_ptr_eq(lel[1]->prev, lel[0]);

    ck_assert_ptr_eq(lel[1]->next, init_back);
    ck_assert_ptr_eq(lel[1]->prev, lel[0]);

    ck_assert_ptr_ne(lel[1]->data, NULL);
    ck_assert_ptr_ne(lel[0]->data, lel[1]->data);

    lel[2] = list_add_after(&l, lel[0]);

    ck_assert_int_eq(l.count, 3);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[1]);
    ck_assert_ptr_eq(&l, lel[2]->host);
    ck_assert_ptr_eq(lel[0]->next, lel[2]);
    ck_assert_ptr_eq(lel[1]->prev, lel[2]);

    ck_assert_ptr_eq(lel[1]->next, init_back);
    ck_assert_ptr_eq(lel[1]->prev, lel[2]);

    ck_assert_ptr_eq(lel[2]->next, lel[1]);
    ck_assert_ptr_eq(lel[2]->prev, lel[0]);

    ck_assert_ptr_ne(lel[2]->data, NULL);
    ck_assert_ptr_ne(lel[2]->data, lel[1]->data);
    ck_assert_ptr_ne(lel[2]->data, lel[0]->data);

    list_purge(&l);
}
END_TEST

START_TEST(test_list_add_after_noninplace_ok) {
    list_t l;
    list_element_t *lel[3];
    list_element_t *init_front, *init_back;

    initialize_list(l, false);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    lel[0] = list_add_after(&l, list_begin(&l));

    ck_assert_int_eq(l.count, 1);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[0]->host);
    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_eq(lel[0]->data, NULL);


    lel[1] = list_add_after(&l, lel[0]);

    ck_assert_int_eq(l.count, 2);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[1]);
    ck_assert_ptr_eq(&l, lel[1]->host);
    ck_assert_ptr_eq(lel[0]->next, lel[1]);
    ck_assert_ptr_eq(lel[1]->prev, lel[0]);

    ck_assert_ptr_eq(lel[1]->next, init_back);
    ck_assert_ptr_eq(lel[1]->prev, lel[0]);

    ck_assert_ptr_eq(lel[1]->data, NULL);
    ck_assert_ptr_eq(lel[0]->data, lel[1]->data);

    lel[2] = list_add_after(&l, lel[0]);

    ck_assert_int_eq(l.count, 3);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[1]);
    ck_assert_ptr_eq(&l, lel[2]->host);
    ck_assert_ptr_eq(lel[0]->next, lel[2]);
    ck_assert_ptr_eq(lel[1]->prev, lel[2]);

    ck_assert_ptr_eq(lel[1]->next, init_back);
    ck_assert_ptr_eq(lel[1]->prev, lel[2]);

    ck_assert_ptr_eq(lel[2]->next, lel[1]);
    ck_assert_ptr_eq(lel[2]->prev, lel[0]);

    ck_assert_ptr_eq(lel[2]->data, NULL);
    ck_assert_ptr_eq(lel[2]->data, lel[1]->data);
    ck_assert_ptr_eq(lel[2]->data, lel[0]->data);

    list_purge(&l);
}
END_TEST

START_TEST(test_list_add_before_inplace_ok) {
    list_t l;
    list_element_t *lel[3];
    list_element_t *init_front, *init_back;

    initialize_list(l, true);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    lel[0] = list_add_before(&l, list_begin(&l));

    ck_assert_int_eq(l.count, 1);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[0]->host);
    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_ne(lel[0]->data, NULL);


    lel[1] = list_add_before(&l, lel[0]);

    ck_assert_int_eq(l.count, 2);
    ck_assert_ptr_eq(l.front, lel[1]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[1]->host);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_eq(lel[0]->prev, lel[1]);

    ck_assert_ptr_eq(lel[1]->next, lel[0]);
    ck_assert_ptr_eq(lel[1]->prev, init_front);

    ck_assert_ptr_ne(lel[1]->data, NULL);
    ck_assert_ptr_ne(lel[0]->data, lel[1]->data);

    lel[2] = list_add_before(&l, lel[0]);

    ck_assert_int_eq(l.count, 3);
    ck_assert_ptr_eq(l.front, lel[1]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[2]->host);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_eq(lel[0]->prev, lel[2]);

    ck_assert_ptr_eq(lel[1]->next, lel[2]);
    ck_assert_ptr_eq(lel[1]->prev, init_front);

    ck_assert_ptr_eq(lel[2]->next, lel[0]);
    ck_assert_ptr_eq(lel[2]->prev, lel[1]);

    ck_assert_ptr_ne(lel[2]->data, NULL);
    ck_assert_ptr_ne(lel[0]->data, lel[2]->data);
    ck_assert_ptr_ne(lel[1]->data, lel[2]->data);

    list_purge(&l);
}
END_TEST

START_TEST(test_list_add_before_noninplace_ok) {
    list_t l;
    list_element_t *lel[3];
    list_element_t *init_front, *init_back;

    initialize_list(l, false);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    lel[0] = list_add_before(&l, list_begin(&l));

    ck_assert_int_eq(l.count, 1);
    ck_assert_ptr_eq(l.front, lel[0]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[0]->host);
    ck_assert_ptr_eq(lel[0]->prev, init_front);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_eq(lel[0]->data, NULL);

    lel[1] = list_add_before(&l, lel[0]);

    ck_assert_int_eq(l.count, 2);
    ck_assert_ptr_eq(l.front, lel[1]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[1]->host);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_eq(lel[0]->prev, lel[1]);

    ck_assert_ptr_eq(lel[1]->next, lel[0]);
    ck_assert_ptr_eq(lel[1]->prev, init_front);

    ck_assert_ptr_eq(lel[1]->data, NULL);
    ck_assert_ptr_eq(lel[0]->data, lel[1]->data);

    lel[2] = list_add_before(&l, lel[0]);

    ck_assert_int_eq(l.count, 3);
    ck_assert_ptr_eq(l.front, lel[1]);
    ck_assert_ptr_eq(l.back, lel[0]);
    ck_assert_ptr_eq(&l, lel[2]->host);
    ck_assert_ptr_eq(lel[0]->next, init_back);
    ck_assert_ptr_eq(lel[0]->prev, lel[2]);

    ck_assert_ptr_eq(lel[1]->next, lel[2]);
    ck_assert_ptr_eq(lel[1]->prev, init_front);

    ck_assert_ptr_eq(lel[2]->next, lel[0]);
    ck_assert_ptr_eq(lel[2]->prev, lel[1]);

    ck_assert_ptr_eq(lel[2]->data, NULL);
    ck_assert_ptr_eq(lel[0]->data, lel[2]->data);
    ck_assert_ptr_eq(lel[1]->data, lel[2]->data);

    list_purge(&l);
}
END_TEST

START_TEST(test_list_remove_and_advance_ok) {
    list_t l;
    list_element_t *lel[4], *l_tst;
    list_element_t *init_back;

    initialize_list(l, false);

    init_back = list_end(&l);

    lel[0] = list_append(&l);
    lel[1] = list_append(&l);
    lel[2] = list_append(&l);
    lel[3] = list_append(&l);

    ck_assert_int_eq(l.count, 4);

    l_tst = list_remove_and_advance(&l, lel[1]);
    ck_assert_ptr_eq(l_tst, lel[2]);
    ck_assert_int_eq(l.count, 3);

    l_tst = list_remove_and_advance(&l, lel[2]);
    ck_assert_ptr_eq(l_tst, lel[3]);
    ck_assert_int_eq(l.count, 2);

    l_tst = list_remove_and_advance(&l, lel[3]);
    ck_assert_ptr_eq(l_tst, init_back);
    ck_assert_int_eq(l.count, 1);

    list_purge(&l);
}
END_TEST

START_TEST(test_list_begin_end_next_prev) {
    list_t l;
    list_element_t *lel[4], *l_tst;
    list_element_t *init_front, *init_back;

    initialize_list(l, false);

    init_front = list_begin(&l);
    init_back = list_end(&l);

    lel[0] = list_append(&l);
    lel[1] = list_append(&l);
    lel[2] = list_append(&l);
    lel[3] = list_append(&l);

    ck_assert_int_eq(l.count, 4);

    l_tst = list_begin(&l);
    ck_assert_ptr_eq(l_tst, lel[0]);

    l_tst = list_next(&l, l_tst);
    ck_assert_ptr_eq(l_tst, lel[1]);

    l_tst = list_next(&l, l_tst);
    ck_assert_ptr_eq(l_tst, lel[2]);

    l_tst = list_next(&l, l_tst);
    ck_assert_ptr_eq(l_tst, lel[3]);

    l_tst = list_next(&l, l_tst);
    ck_assert_ptr_eq(l_tst, init_back);

    l_tst = list_prev(&l, l_tst);
    ck_assert_ptr_eq(l_tst, lel[3]);

    l_tst = list_prev(&l, l_tst);
    ck_assert_ptr_eq(l_tst, lel[2]);

    l_tst = list_prev(&l, l_tst);
    ck_assert_ptr_eq(l_tst, lel[1]);

    l_tst = list_prev(&l, l_tst);
    ck_assert_ptr_eq(l_tst, lel[0]);

    l_tst = list_prev(&l, l_tst);
    ck_assert_ptr_eq(l_tst, init_front);

    list_purge(&l);
}
END_TEST

Suite *containers_suite(void)  {
    Suite *s;
    TCase *tc;

    s = suite_create("containers");

    tc = tcase_create("vector");

    tcase_add_test(tc, test_vector_init_ok);
    tcase_add_test(tc, test_vector_insert_ok);
    tcase_add_test(tc, test_vector_remove_ok);
    tcase_add_test(tc, test_vector_remove_range_ok);
    tcase_add_test(tc, test_vector_insert_ok);
    tcase_add_test(tc, test_vector_append_ok);
    tcase_add_test(tc, test_vector_prepend_ok);
    tcase_add_test(tc, test_vector_begin_end_get_next_prev_ok);

    suite_add_tcase(s, tc);

    tc = tcase_create("list");

    tcase_add_test(tc, test_list_init_ok);
    tcase_add_test(tc, test_list_prepend_inplace_ok);
    tcase_add_test(tc, test_list_prepend_noninplace_ok);
    tcase_add_test(tc, test_list_append_inplace_ok);
    tcase_add_test(tc, test_list_append_noninplace_ok);
    tcase_add_test(tc, test_list_add_after_inplace_ok);
    tcase_add_test(tc, test_list_add_after_noninplace_ok);
    tcase_add_test(tc, test_list_add_before_inplace_ok);
    tcase_add_test(tc, test_list_add_before_noninplace_ok);
    tcase_add_test(tc, test_list_remove_and_advance_ok);
    tcase_add_test(tc, test_list_begin_end_next_prev);

    suite_add_tcase(s, tc);

    return s;
}
