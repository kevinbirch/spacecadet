
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "hashtable.h"

#include "test.h"


static const char * const WORDS [] = 
{
    "cabal",
    "cabby",
    "caber",
    "cabin",
    "cable",
    "cabob",
    "cacao",
    "cacas",
    "cache",
    "cacti",
    "caddy",
    "cades",
    "cadet",
    "cadge",
    "cadgy",
    "cadis",
    "cadre",
    "caeca",
    "cafes",
    "caffs",
    "caged",
    "cager",
    "cages",
    "cagey",
    "cahow",
    "caids",
    "cains",
    "caird",
    "cairn",
    "cajon",
    "caked",
    "cakes",
    "cakey",
    "calfs",
    "calif",
    "calix",
    "calks",
    "calla",
    "calls",
    "calms",
    "calos",
    "calve",
    "calyx",
    "camas",
    "camel",
    "cameo",
    "cames",
    "campi",
    "campo",
    "camps",
};

bool compare(const void *key1, const void *key2);
hashcode deadbeef(const void *key);
bool compare_int(const void *value1, const void *value2);

bool compare(const void *key1, const void *key2)
{
    return 0 == strcmp((char *)key1, (char *)key2);
}

hashcode deadbeef(const void *key __attribute__((unused)))
{
    return 0xDEADBEEF;
}

START_TEST(test_constructor)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable);
    assert_true(hashtable_is_mutable(hashtable));
    assert_false(hashtable_is_immutable(hashtable));    

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_size)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable);

    uint32_t value = 42ul;
    
    size_t length = (sizeof(WORDS) / sizeof(char *)) >> 1;
    for(size_t i = 0; i < length; i++)
    {
        reset_errno();
        void *prev = hashtable_put(hashtable, (void *)WORDS[i], &value);
        assert_noerr();
        assert_null(prev);
    }

    assert_uint_eq(length, hashtable_size(hashtable));

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_contains)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable_with_function(compare, shift_add_xor_string_hash);
    assert_noerr();
    assert_not_null(hashtable);

    char *key = "i am a string key";
    uint32_t value = 42ul;
    
    reset_errno();
    void *prev = hashtable_put(hashtable, key, &value);
    assert_noerr();
    assert_null(prev);

    reset_errno();
    bool result = hashtable_contains(hashtable, key);
    assert_noerr();
    assert_true(result);
    
    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_get_if_absent)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable);

    char *key = "i am a string key";
    uint32_t value = 42ul;
    
    reset_errno();
    void *prev = hashtable_put(hashtable, key, &value);
    assert_noerr();
    assert_null(prev);

    char *key2 = "second key";
    reset_errno();
    assert_false(hashtable_contains(hashtable, key2));
    reset_errno();
    void *result = hashtable_get_if_absent(hashtable, key2, &value);
    assert_noerr();
    assert_uint_eq(42ul, *(uint32_t *)result);
    assert_ptr_eq(&value, result);

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_get_if_absent_put)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable);

    char *key = "i am a string key";
    uint32_t value = 42ul;
    
    reset_errno();
    void *prev = hashtable_put(hashtable, key, &value);
    assert_noerr();
    assert_null(prev);

    char *key2 = "second key";
    reset_errno();
    assert_false(hashtable_contains(hashtable, key2));
    assert_noerr();

    reset_errno();
    void *result = hashtable_get_if_absent_put(hashtable, key2, &value);
    assert_noerr();
    assert_uint_eq(42ul, *(uint32_t *)result);
    assert_ptr_eq(&value, result);

    assert_uint_eq(2, hashtable_size(hashtable));

    reset_errno();
    assert_true(hashtable_contains(hashtable, key2));
    assert_noerr();

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_put_get)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable_with_function(compare, shift_add_xor_string_hash);
    assert_noerr();
    assert_not_null(hashtable);

    char *key = "i am a string key";
    uint32_t value = 42ul;
    
    reset_errno();
    void *prev = hashtable_put(hashtable, key, &value);
    assert_noerr();
    assert_null(prev);

    reset_errno();
    void *result = hashtable_get(hashtable, key);
    assert_noerr();
    assert_not_null(result);
    assert_uint_eq(42ul, *(uint32_t *)result);
    assert_ptr_eq(&value, result);

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_put_many)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable_with_function(compare, shift_add_xor_string_hash);
    assert_noerr();
    assert_not_null(hashtable);

    uint32_t value = 42ul;
    
    size_t length = sizeof(WORDS) / sizeof(char *);
    for(size_t i = 0; i < length; i++)
    {
        reset_errno();
        void *prev = hashtable_put(hashtable, (void *)WORDS[i], &value);
        assert_noerr();
        assert_null(prev);
    }

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_remove)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable);

    char *key = "i am a string key";
    uint32_t value = 42ul;
    
    reset_errno();
    void *prev = hashtable_put(hashtable, key, &value);
    assert_noerr();
    assert_null(prev);

    reset_errno();
    void *result = hashtable_remove(hashtable, key);
    assert_noerr();

    assert_not_null(result);
    assert_uint_eq(42ul, *(uint32_t *)result);
    assert_ptr_eq(&value, result);

    assert_uint_eq(0, hashtable_size(hashtable));

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_chained_remove_head)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable_with_function(compare, deadbeef);
    assert_noerr();
    assert_not_null(hashtable);

    uint32_t value = 42ul;
    
    for(size_t i = 0; i < 5ul; i++)
    {
        reset_errno();
        void *prev = hashtable_put(hashtable, (void *)WORDS[i], &value);
        assert_noerr();
        assert_null(prev);
    }

    reset_errno();
    void *result = hashtable_remove(hashtable, (char *)WORDS[4]);
    assert_noerr();

    assert_not_null(result);
    assert_uint_eq(42ul, *(uint32_t *)result);
    assert_ptr_eq(&value, result);

    assert_uint_eq(4, hashtable_size(hashtable));

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_chained_remove_mid)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable_with_function(compare, deadbeef);
    assert_noerr();
    assert_not_null(hashtable);

    uint32_t value = 42ul;
    
    for(size_t i = 0; i < 5ul; i++)
    {
        reset_errno();
        void *prev = hashtable_put(hashtable, (void *)WORDS[i], &value);
        assert_noerr();
        assert_null(prev);
    }

    reset_errno();
    void *result = hashtable_remove(hashtable, (char *)WORDS[2]);
    assert_noerr();

    assert_not_null(result);
    assert_uint_eq(42ul, *(uint32_t *)result);
    assert_ptr_eq(&value, result);

    assert_uint_eq(4, hashtable_size(hashtable));

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_chained_remove_tail)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable_with_function(compare, deadbeef);
    assert_noerr();
    assert_not_null(hashtable);

    uint32_t value = 42ul;
    
    for(size_t i = 0; i < 5ul; i++)
    {
        reset_errno();
        void *prev = hashtable_put(hashtable, (void *)WORDS[i], &value);
        assert_noerr();
        assert_null(prev);
    }

    reset_errno();
    void *result = hashtable_remove(hashtable, (char *)WORDS[0]);
    assert_noerr();

    assert_not_null(result);
    assert_uint_eq(42ul, *(uint32_t *)result);
    assert_ptr_eq(&value, result);

    assert_uint_eq(4, hashtable_size(hashtable));

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_chained_remove_all)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable_with_function(compare, deadbeef);
    assert_noerr();
    assert_not_null(hashtable);

    uint32_t value = 42ul;
    
    for(size_t i = 0; i < 2ul; i++)
    {
        reset_errno();
        void *prev = hashtable_put(hashtable, (void *)WORDS[i], &value);
        assert_noerr();
        assert_null(prev);
    }

    reset_errno();
    hashtable_remove(hashtable, (char *)WORDS[0]);
    assert_noerr();

    reset_errno();
    hashtable_remove(hashtable, (char *)WORDS[1]);
    assert_noerr();

    assert_uint_eq(0, hashtable_size(hashtable));

    hashtable_free(hashtable);
}
END_TEST

START_TEST(test_chained_remove_most)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable_with_function(compare, deadbeef);
    assert_noerr();
    assert_not_null(hashtable);

    uint32_t value = 42ul;
    
    for(size_t i = 0; i < 2ul; i++)
    {
        reset_errno();
        void *prev = hashtable_put(hashtable, (void *)WORDS[i], &value);
        assert_noerr();
        assert_null(prev);
    }

    reset_errno();
    hashtable_remove(hashtable, (char *)WORDS[0]);
    assert_noerr();

    assert_uint_eq(1, hashtable_size(hashtable));

    hashtable_free(hashtable);
}
END_TEST

bool compare_int(const void *value1, const void *value2)
{
    return *(uint32_t *)value1 == *(uint32_t *)value2;
}

START_TEST(test_equals)
{
    reset_errno();
    Hashtable *hashtable1 = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable1);

    reset_errno();
    Hashtable *hashtable2 = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable2);

    reset_errno();
    Hashtable *hashtable3 = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable3);

    uint32_t value = 42ul;
    
    for(size_t i = 0; i < 2ul; i++)
    {
        reset_errno();
        hashtable_put(hashtable1, (void *)WORDS[i], &value);
        assert_noerr();
        reset_errno();
        hashtable_put(hashtable2, (void *)WORDS[i], &value);
        assert_noerr();
    }

    assert_true(hashtable_equals(hashtable1, hashtable1, compare_int));
    assert_true(hashtable_equals(hashtable1, hashtable2, compare_int));
    assert_true(hashtable_equals(hashtable2, hashtable1, compare_int));

    assert_false(hashtable_equals(hashtable1, hashtable3, compare_int));
    assert_false(hashtable_equals(hashtable3, hashtable1, compare_int));
    assert_false(hashtable_equals(hashtable2, hashtable3, compare_int));
    assert_false(hashtable_equals(hashtable3, hashtable2, compare_int));
}
END_TEST

START_TEST(test_copy)
{
    reset_errno();
    Hashtable *hashtable1 = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable1);

    uint32_t value = 42ul;
    
    for(size_t i = 0; i < 2ul; i++)
    {
        reset_errno();
        hashtable_put(hashtable1, (void *)WORDS[i], &value);
        assert_noerr();
    }

    reset_errno();
    Hashtable *hashtable2 = hashtable_copy(hashtable1);
    assert_noerr();
    assert_not_null(hashtable2);

    assert_true(hashtable_equals(hashtable1, hashtable2, compare_int));
    assert_true(hashtable_equals(hashtable2, hashtable1, compare_int));

}
END_TEST

START_TEST(test_clear)
{
    reset_errno();
    Hashtable *hashtable = make_hashtable(compare);
    assert_noerr();
    assert_not_null(hashtable);

    uint32_t value = 42ul;
    
    size_t length = (sizeof(WORDS) / sizeof(char *)) >> 1;
    for(size_t i = 0; i < length; i++)
    {
        reset_errno();
        void *prev = hashtable_put(hashtable, (void *)WORDS[i], &value);
        assert_noerr();
        assert_null(prev);
    }

    assert_uint_eq(length, hashtable_size(hashtable));

    hashtable_clear(hashtable);
    
    assert_uint_eq(0, hashtable_size(hashtable));

    hashtable_free(hashtable);
}
END_TEST

Suite *hashtable_suite(void)
{
    TCase *lifecycle_case = tcase_create("lifecycle");
    tcase_add_test(lifecycle_case, test_constructor);

    TCase *basic_case = tcase_create("basic");
    tcase_add_test(basic_case, test_size);
    tcase_add_test(basic_case, test_contains);
    tcase_add_test(basic_case, test_get_if_absent);
    tcase_add_test(basic_case, test_get_if_absent_put);
    tcase_add_test(basic_case, test_put_get);
    tcase_add_test(basic_case, test_put_many);
    tcase_add_test(basic_case, test_remove);
    tcase_add_test(basic_case, test_chained_remove_head);
    tcase_add_test(basic_case, test_chained_remove_mid);
    tcase_add_test(basic_case, test_chained_remove_tail);
    tcase_add_test(basic_case, test_chained_remove_all);
    tcase_add_test(basic_case, test_chained_remove_most);
    tcase_add_test(basic_case, test_clear);
    tcase_add_test(basic_case, test_equals);
    tcase_add_test(basic_case, test_copy);

    Suite *hashtable = suite_create("Hashtable");
    suite_add_tcase(hashtable, lifecycle_case);
    suite_add_tcase(hashtable, basic_case);

    return hashtable;
}
