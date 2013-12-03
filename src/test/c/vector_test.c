/*
 * 金棒 (kanabō)
 * Copyright (c) 2012 Kevin Birch <kmb@pobox.com>.  All rights reserved.
 * 
 * 金棒 is a tool to bludgeon YAML and JSON files from the shell: the strong
 * made stronger.
 *
 * For more information, consult the README file in the project root.
 *
 * Distributed under an [MIT-style][license] license.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal with
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimers.
 * - Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimers in the documentation and/or
 *   other materials provided with the distribution.
 * - Neither the names of the copyright holders, nor the names of the authors, nor
 *   the names of other contributors may be used to endorse or promote products
 *   derived from this Software without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
 *
 * [license]: http://www.opensource.org/licenses/ncsa
 */

#ifdef __linux__
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#endif

#ifdef __APPLE__
#define _DARWIN_SOURCE
#endif

#include <errno.h>
#include <stdio.h>
#include <check.h>

#include "vector.h"
#include "test.h"

#define assert_vector_length(VECTOR, EXPECTED_LENGTH) assert_uint_eq((EXPECTED_LENGTH), vector_length(VECTOR))
#define assert_vector_empty(VECTOR) assert_true(vector_is_empty(VECTOR))

void vector_setup(void);
void vector_teardown(void);

bool  fail_vector(void *each, void *context);
bool  check_vector(void *each, void *context);
bool  transform(void *each, void *context, Vector *target);
bool  fail_transform(void *each, void *context, Vector *target);

static const char * const foo = "foo";
static const char * const bar = "bar";

static Vector *vector;


START_TEST (bad_length)
{
    reset_errno();
    assert_vector_length(NULL, 0);
    assert_errno(EINVAL);
}
END_TEST

START_TEST (bad_is_empty)
{
    reset_errno();
    assert_true(vector_is_empty(NULL));
    assert_errno(EINVAL);
}
END_TEST

START_TEST (bad_get)
{
    reset_errno();
    assert_null(vector_get(NULL, 0));
    assert_errno(EINVAL);

    reset_errno();
    Vector *empty_vector = make_vector();
    assert_not_null(empty_vector);
    assert_noerr();

    reset_errno();
    assert_null(vector_get(empty_vector, 0));
    assert_errno(EINVAL);

    vector_free(empty_vector);
}
END_TEST

START_TEST (bad_add)
{
    reset_errno();
    assert_false(vector_add(NULL, NULL));
    assert_errno(EINVAL);

    reset_errno();
    Vector *empty_vector = make_vector();
    assert_not_null(empty_vector);
    assert_noerr();

    reset_errno();
    assert_false(vector_add(empty_vector, NULL));
    assert_errno(EINVAL);

    vector_free(empty_vector);
}
END_TEST

START_TEST (bad_set)
{
    reset_errno();
    assert_false(vector_set(NULL, NULL, 0));
    assert_errno(EINVAL);

    reset_errno();
    Vector *empty_vector = make_vector();
    assert_not_null(empty_vector);
    assert_noerr();

    reset_errno();
    assert_null(vector_set(empty_vector, (void *)foo, 0));
    assert_errno(EINVAL);

    vector_free(empty_vector);
}
END_TEST

START_TEST (bad_iterate)
{
    reset_errno();
    Vector *empty_vector = make_vector();
    assert_not_null(empty_vector);
    assert_noerr();

    reset_errno();
    assert_false(vector_iterate(NULL, NULL, NULL));
    assert_errno(EINVAL);

    reset_errno();
    assert_false(vector_iterate(empty_vector, NULL, NULL));
    assert_errno(EINVAL);

    vector_free(empty_vector);
}
END_TEST
    
START_TEST (bad_map)
{
    reset_errno();
    Vector *empty_vector = make_vector();
    assert_not_null(empty_vector);
    assert_noerr();

    reset_errno();
    assert_null(vector_map(NULL, NULL, NULL));
    assert_errno(EINVAL);

    reset_errno();
    assert_null(vector_map(empty_vector, NULL, NULL));
    assert_errno(EINVAL);

    vector_free(empty_vector);
}
END_TEST
    
START_TEST (bad_map_into)
{
    reset_errno();
    Vector *empty_vector = make_vector();
    assert_not_null(empty_vector);
    assert_noerr();

    reset_errno();
    assert_null(vector_map_into(NULL, NULL, NULL, NULL));
    assert_errno(EINVAL);

    reset_errno();
    assert_null(vector_map_into(empty_vector, NULL, NULL, NULL));
    assert_errno(EINVAL);

    reset_errno();
    assert_null(vector_map_into(empty_vector, (vector_mapper)1, NULL, NULL));
    assert_errno(EINVAL);

    vector_free(empty_vector);
}
END_TEST

void vector_setup(void)
{
    reset_errno();
    vector = make_vector_with_capacity(2);
    assert_noerr();
    assert_not_null(vector);
    assert_int_eq(2, vector_capacity(vector));

    reset_errno();
    assert_true(vector_add(vector, (void *)foo));
    assert_noerr();
    assert_vector_length(vector, 1);

    reset_errno();
    assert_true(vector_add(vector, (void *)bar));
    assert_noerr();
    assert_vector_length(vector, 2);

    reset_errno();
}

void vector_teardown(void)
{
    vector_free(vector);
}
    
START_TEST (ctor_dtor)
{
    reset_errno();
    Vector *empty_vector = make_vector();
    assert_not_null(empty_vector);
    assert_noerr();

    assert_vector_length(empty_vector, 0);
    assert_vector_empty(empty_vector);
    
    reset_errno();
    vector_free(empty_vector);
    assert_noerr();
}
END_TEST

START_TEST (get)
{
    char *result = vector_get(vector, 0);
    assert_ptr_eq(foo, result);
}
END_TEST

START_TEST (first)
{
    char *result = vector_first(vector);
    assert_ptr_eq(foo, result);
}
END_TEST

START_TEST (head)
{
    char *result = vector_head(vector);
    assert_ptr_eq(foo, result);
}
END_TEST

START_TEST (last)
{
    char *result = vector_last(vector);
    assert_ptr_eq(bar, result);
}
END_TEST

START_TEST (add)
{
    char *baz = "baz";

    reset_errno();
    vector_add(vector, baz);
    assert_noerr();
    assert_vector_length(vector, 3);

    char *result = vector_get(vector, 2);
    assert_ptr_eq(baz, result);
}
END_TEST

START_TEST (append)
{
    char *baz = "baz";

    reset_errno();
    vector_append(vector, baz);
    assert_noerr();
    assert_vector_length(vector, 3);

    char *result = vector_get(vector, 2);
    assert_ptr_eq(baz, result);
}
END_TEST

START_TEST (insert_front)
{
    char *baz = "baz";

    reset_errno();
    vector_insert(vector, baz, 0);
    assert_noerr();
    assert_vector_length(vector, 3);

    char *result = vector_get(vector, 0);
    assert_ptr_eq(baz, result);
    result = vector_get(vector, 1);
    assert_ptr_eq(foo, result);
    result = vector_get(vector, 2);
    assert_ptr_eq(bar, result);
}
END_TEST

START_TEST (insert_mid)
{
    char *baz = "baz";

    reset_errno();
    vector_insert(vector, baz, 1);
    assert_noerr();
    assert_vector_length(vector, 3);

    char *result = vector_get(vector, 1);
    assert_ptr_eq(baz, result);
    result = vector_get(vector, 0);
    assert_ptr_eq(foo, result);
    result = vector_get(vector, 2);
    assert_ptr_eq(bar, result);
}
END_TEST

START_TEST (insert_end)
{
    char *baz = "baz";

    reset_errno();
    vector_insert(vector, baz, vector_length(vector));
    assert_noerr();
    assert_vector_length(vector, 3);

    char *result = vector_get(vector, 2);
    assert_ptr_eq(baz, result);
}
END_TEST

START_TEST (prepend)
{
    char *baz = "baz";

    reset_errno();
    vector_prepend(vector, baz);
    assert_noerr();
    assert_vector_length(vector, 3);

    char *result = vector_get(vector, 0);
    assert_ptr_eq(baz, result);
    result = vector_get(vector, 1);
    assert_ptr_eq(foo, result);
    result = vector_get(vector, 2);
    assert_ptr_eq(bar, result);
}
END_TEST

START_TEST (set)
{
    char *baz = "baz";

    reset_errno();
    char *previous = vector_set(vector, baz, 0);
    assert_noerr();
    assert_not_null(previous);
    assert_ptr_eq(foo, previous);
    assert_vector_length(vector, 2);

    char *result = vector_get(vector, 0);
    assert_ptr_eq(baz, result);
}
END_TEST

START_TEST (test_remove)
{
    reset_errno();
    char *previous = vector_remove(vector, 0);
    assert_noerr();
    assert_not_null(previous);
    assert_ptr_eq(foo, previous);
    assert_vector_length(vector, 1);

    char *result = vector_get(vector, 0);
    assert_noerr();
    assert_not_null(result);
    assert_ptr_eq(bar, result);
}
END_TEST

START_TEST(clear)
{
    reset_errno();
    vector_clear(vector);
    assert_noerr();

    assert_vector_empty(vector);
    assert_vector_length(vector, 0);
    assert_int_eq(2, vector_capacity(vector));
}
END_TEST

START_TEST (trim)
{
    vector_free(vector);

    reset_errno();
    vector = make_vector_with_capacity(6);
    assert_int_eq(6, vector_capacity(vector));
    assert_noerr();
    assert_not_null(vector);

    reset_errno();
    assert_true(vector_add(vector, (void *)foo));
    assert_noerr();
    assert_vector_length(vector, 1);

    reset_errno();
    assert_true(vector_add(vector, (void *)bar));
    assert_noerr();
    assert_vector_length(vector, 2);

    
    reset_errno();
    assert_true(vector_trim(vector));
    assert_noerr();
    assert_vector_length(vector, 2);
    assert_int_eq(2, vector_capacity(vector));

    reset_errno();
    char *one = vector_get(vector, 0);
    assert_noerr();
    assert_not_null(one);
    assert_ptr_eq(foo, one);

    reset_errno();
    char *two = vector_get(vector, 1);
    assert_noerr();
    assert_not_null(two);
    assert_ptr_eq(bar, two);
}
END_TEST

START_TEST (add_all)
{
    char *baz = "baz";
    char *quaz = "quaz";

    reset_errno();
    Vector *other_vector = make_vector_with_capacity(2);
    assert_noerr();
    assert_not_null(other_vector);

    reset_errno();
    assert_true(vector_add(other_vector, baz));
    assert_noerr();
    assert_vector_length(other_vector, 1);

    reset_errno();
    assert_true(vector_add(other_vector, quaz));
    assert_noerr();
    assert_vector_length(other_vector, 2);

    reset_errno();
    assert_true(vector_add_all(vector, other_vector));
    assert_noerr();
    assert_vector_length(vector, 4);
    assert_vector_length(other_vector, 2);
    
    vector_free(other_vector);
}
END_TEST

START_TEST (iteration)
{
    size_t count = 0;
    reset_errno();
    assert_true(vector_iterate(vector, check_vector, &count));
    assert_noerr();
    assert_uint_eq(2, count);
}
END_TEST

bool check_vector(void *each, void *context)
{
    assert_not_null(each);
    size_t *count = (size_t *)context;
    (*count)++;
    return true;
}

START_TEST (fail_iteration)
{
    size_t count = 0;
    reset_errno();
    assert_false(vector_iterate(vector, fail_vector, &count));
    assert_noerr();
    assert_uint_eq(1, count);
}
END_TEST

bool fail_vector(void *each __attribute__((unused)), void *context)
{
    size_t *count = (size_t *)context;
    if(0 < *count)
    {
        return false;
    }
    else
    {
        (*count)++;
        return true;
    }
}

START_TEST (map)
{
    size_t count = 0;

    reset_errno();
    Vector *result = vector_map(vector, transform, &count);
    assert_noerr();
    assert_not_null(result);
    assert_uint_eq(2, count);
    assert_vector_length(result, 2);

    char *zero = vector_get(result, 0);
    assert_not_null(zero);
    free(zero);

    char *one = vector_get(result, 1);
    assert_not_null(one);
    free(one);

    vector_free(result);
}
END_TEST

bool transform(void *each, void *context, Vector *target)
{
    assert_not_null(each);
    size_t *count = (size_t *)context;
    (*count)++;
    char *value;
    int result = asprintf(&value, "%zd", *count);
    assert_int_ne(-1, result);
    return vector_add(target, value);
}

START_TEST (fail_map)
{
    size_t count = 0;
    reset_errno();
    Vector *result = vector_map(vector, fail_transform, &count);
    assert_null(result);
    assert_not_null(vector);
    assert_noerr();
    assert_uint_eq(1, count);
}
END_TEST

bool fail_transform(void *each __attribute__((unused)), void *context, Vector *target)
{
    size_t *count = (size_t *)context;
    if(0 < *count)
    {
        return false;
    }
    else
    {
        (*count)++;
        return vector_add(target, "munky");
    }
}

void *reducer(const void *one, const void *two, void *context);
void *reducer(const void *one, const void *two, void *context __attribute__((unused)))
{
    const char *a = (const char *)one;
    size_t a_len = strlen(a);
    const char *b = (const char *)two;
    size_t b_len = strlen(b);
    size_t length = a_len + b_len;
    
    char *result = calloc(1, length + 1);
    memcpy(result, a, a_len);
    memcpy(result + a_len, b, b_len);
    result[length - 1] = '\0';

    return result;
}

START_TEST (reduce_emtpy)
{
    vector_clear(vector);
    assert_noerr();
   
    assert_null(vector_reduce(vector, reducer, NULL));
    assert_errno(EINVAL);
}
END_TEST

START_TEST (reduce_one)
{
    vector_pop(vector);
    assert_noerr();

    char *result = vector_reduce(vector, reducer, NULL);
    assert_noerr();
    assert_not_null(result);
    assert_ptr_eq(foo, result);    
}
END_TEST

START_TEST (reduce)
{
    char *result = vector_reduce(vector, reducer, NULL);
    assert_noerr();
    assert_not_null(result);
    assert_buf_eq("foobar", 6, result, strlen(result));
}
END_TEST

bool always_true(void *each, void *context);
bool always_true(void *each __attribute__((unused)), void *context __attribute__((unused)))
{
    return true;
}

bool always_false(void *each, void *context);
bool always_false(void *each __attribute__((unused)), void *context __attribute__((unused)))
{
    return false;
}

START_TEST (find)
{
    void *result = vector_find(vector, always_true, NULL);
    assert_ptr_eq(foo, result);
}
END_TEST

bool true_comparitor(const void *each, const void *context);
bool true_comparitor(const void *each __attribute__((unused)), const void *context __attribute__((unused)))
{
    return true;
}

bool false_comparitor(const void *each, const void *context);
bool false_comparitor(const void *each __attribute__((unused)), const void *context __attribute__((unused)))
{
    return false;
}

START_TEST (contains)
{
    assert_true(vector_contains(vector, true_comparitor, NULL));
    assert_false(vector_contains(vector, false_comparitor, NULL));
}
END_TEST

START_TEST (any)
{
    assert_true(vector_any(vector, always_true, NULL));    
    assert_false(vector_any(vector, always_false, NULL));    
}
END_TEST

START_TEST (all)
{
    assert_true(vector_all(vector, always_true, NULL));
    assert_false(vector_all(vector, always_false, NULL));
}
END_TEST

START_TEST (none)
{
    assert_false(vector_none(vector, always_true, NULL));
    assert_true(vector_none(vector, always_false, NULL));
}
END_TEST

START_TEST (count)
{
    assert_int_eq(2, vector_count(vector, always_true, NULL));
    assert_int_eq(0, vector_count(vector, always_false, NULL));
}
END_TEST

START_TEST (peek)
{
    void *result = vector_peek(vector);
    assert_noerr();
    assert_vector_length(vector, 2);
    assert_ptr_eq(bar, result);
}
END_TEST

START_TEST (push)
{
    char *baz = "baz";

    reset_errno();
    vector_push(vector, baz);
    assert_noerr();
    assert_vector_length(vector, 3);

    char *result = vector_get(vector, 2);
    assert_ptr_eq(baz, result);
}
END_TEST

START_TEST (pop)
{
    char *value = vector_pop(vector);
    assert_noerr();
    assert_vector_length(vector, 1);

    assert_ptr_eq(bar, value);
}
END_TEST

Suite *vector_suite(void)
{
    TCase *bad_input_case = tcase_create("bad input");
    tcase_add_test(bad_input_case, bad_length);
    tcase_add_test(bad_input_case, bad_is_empty);
    tcase_add_test(bad_input_case, bad_get);
    tcase_add_test(bad_input_case, bad_add);
    tcase_add_test(bad_input_case, bad_set);
    tcase_add_test(bad_input_case, bad_iterate);
    tcase_add_test(bad_input_case, bad_map);
    tcase_add_test(bad_input_case, bad_map_into);
    
    TCase *element_case = tcase_create("element");
    tcase_add_checked_fixture(element_case, vector_setup, vector_teardown);
    tcase_add_test(element_case, ctor_dtor);
    tcase_add_test(element_case, get);
    tcase_add_test(element_case, first);
    tcase_add_test(element_case, head);
    tcase_add_test(element_case, last);

    TCase *mutation_case = tcase_create("mutation");
    tcase_add_checked_fixture(mutation_case, vector_setup, vector_teardown);
    tcase_add_test(mutation_case, add);
    tcase_add_test(mutation_case, append);
    tcase_add_test(mutation_case, insert_front);
    tcase_add_test(mutation_case, insert_mid);
    tcase_add_test(mutation_case, insert_end);
    tcase_add_test(mutation_case, prepend);
    tcase_add_test(mutation_case, set);
    tcase_add_test(mutation_case, test_remove);
    tcase_add_test(mutation_case, add_all);
    tcase_add_test(mutation_case, trim);
    tcase_add_test(mutation_case, clear);

    TCase *functional_case = tcase_create("functional");
    tcase_add_checked_fixture(functional_case, vector_setup, vector_teardown);
    tcase_add_test(functional_case, iteration);
    tcase_add_test(functional_case, fail_iteration);
    tcase_add_test(functional_case, map);
    tcase_add_test(functional_case, fail_map);
    tcase_add_test(functional_case, reduce_emtpy);
    tcase_add_test(functional_case, reduce_one);
    tcase_add_test(functional_case, reduce);

    TCase *search_case = tcase_create("search");
    tcase_add_checked_fixture(search_case, vector_setup, vector_teardown);
    tcase_add_test(search_case, find);
    tcase_add_test(search_case, contains);
    tcase_add_test(search_case, any);
    tcase_add_test(search_case, all);
    tcase_add_test(search_case, none);
    tcase_add_test(search_case, count);

    TCase *queue_case = tcase_create("queue");
    tcase_add_checked_fixture(queue_case, vector_setup, vector_teardown);
    tcase_add_test(queue_case, peek);
    tcase_add_test(queue_case, push);
    tcase_add_test(queue_case, pop);

    Suite *suite = suite_create("Vector");
    suite_add_tcase(suite, bad_input_case);
    suite_add_tcase(suite, element_case);
    suite_add_tcase(suite, mutation_case);
    suite_add_tcase(suite, functional_case);
    suite_add_tcase(suite, search_case);
    suite_add_tcase(suite, queue_case);

    return suite;
}

