
#pragma once

#include <errno.h>
#include <check.h>

#define assert_int_eq(X, Y)  ck_assert_int_eq(X, Y)
#define assert_int_ne(X, Y)  ck_assert_int_ne(X, Y)
#define assert_int_lt(X, Y)  ck_assert_int_lt(X, Y)
#define assert_int_le(X, Y)  ck_assert_int_le(X, Y)
#define assert_int_gt(X, Y)  ck_assert_int_gt(X, Y)
#define assert_int_ge(X, Y)  ck_assert_int_ge(X, Y)

#define assert_uint_eq(X, Y)  ck_assert_uint_eq(X, Y)
#define assert_uint_ne(X, Y)  ck_assert_uint_ne(X, Y)
#define assert_uint_lt(X, Y)  ck_assert_uint_lt(X, Y)
#define assert_uint_le(X, Y)  ck_assert_uint_le(X, Y)
#define assert_uint_gt(X, Y)  ck_assert_uint_gt(X, Y)
#define assert_uint_ge(X, Y)  ck_assert_uint_ge(X, Y)

#define assert_ptr_eq(X, Y)  ck_assert_msg((X) == (Y), "Assertion '" #X " == " #Y "' failed: "#X"==%p, "#Y"==%p", (X), (Y))
#define assert_ptr_ne(X, Y)  ck_assert_msg((X) != (Y), "Assertion '" #X " != " #Y "' failed: "#X"==%p, "#Y"==%p", (X), (Y))

#define assert_null(X)              ck_assert_msg((X) == NULL, "Assertion '"#X" == NULL' failed")
#define assert_not_null(X)          ck_assert_msg((X) != NULL, "Assertion '"#X" != NULL' failed")
#define assert_buf_eq(X, N1, Y, N2) ck_assert_msg(memcmp((X), (Y), (N1) > (N2) ? (N2) : (N1)) == 0, "Assertion 'memcmp("#X", "#Y", %zd)' failed", (N1) > (N2) ? (N2) : (N1))
#define assert_true(X)              ck_assert_msg((X) == true, "Assertion '"#X" == true' failed")
#define assert_false(X)             ck_assert_msg((X) == false, "Assertion '"#X" == false' failed")

#define assert_errno(X) ck_assert_msg(errno == (X), "Assertion 'errno == "#X"' failed. errno is %d (%s)", errno, strerror(errno))
#define assert_noerr()  assert_errno(0)
#define reset_errno()   errno = 0


Suite *hashtable_suite(void);
Suite *vector_suite(void);
