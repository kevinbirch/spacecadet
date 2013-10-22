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
    assert_null(vector_map_into(empty_vector, (vector_map_function)1, NULL, NULL));
    assert_errno(EINVAL);

    vector_free(empty_vector);
}
END_TEST
    
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

void vector_setup(void)
{
    vector = make_vector_with_capacity(2);

    reset_errno();
    assert_true(vector_add(vector, (void *)foo));
    assert_noerr();
    assert_vector_length(vector, 1);

    reset_errno();
    assert_true(vector_add(vector, (void *)bar));
    assert_noerr();
    assert_vector_length(vector, 2);
}

void vector_teardown(void)
{
    vector_free(vector);
}

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
    
    TCase *basic_case = tcase_create("basic");
    tcase_add_test(basic_case, ctor_dtor);

    TCase *mutate_case = tcase_create("mutate");
    tcase_add_checked_fixture(mutate_case, vector_setup, vector_teardown);
    tcase_add_test(mutate_case, add);
    tcase_add_test(mutate_case, set);
    tcase_add_test(mutate_case, test_remove);
    tcase_add_test(mutate_case, add_all);

    TCase *iterate_case = tcase_create("iterate");
    tcase_add_checked_fixture(iterate_case, vector_setup, vector_teardown);
    tcase_add_test(iterate_case, iteration);
    tcase_add_test(iterate_case, fail_iteration);
    tcase_add_test(iterate_case, map);
    tcase_add_test(iterate_case, fail_map);

    Suite *suite = suite_create("Vector");
    suite_add_tcase(suite, bad_input_case);
    suite_add_tcase(suite, basic_case);
    suite_add_tcase(suite, mutate_case);
    suite_add_tcase(suite, iterate_case);

    return suite;
}

