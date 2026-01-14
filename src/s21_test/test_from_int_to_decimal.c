#include <limits.h>

#include "test.h"

START_TEST(int_to_dec_null_pointer) {
  int src = 123;
  int code = s21_from_int_to_decimal(src, NULL);
  ck_assert_int_eq(code, CONVERTATION_ERROR);
}
END_TEST

START_TEST(int_to_dec_zero) {
  s21_decimal d;
  int code = s21_from_int_to_decimal(0, &d);
  ck_assert_int_eq(code, OK);

  ck_assert_uint_eq(d.bits[0], 0u);
  ck_assert_uint_eq(d.bits[1], 0u);
  ck_assert_uint_eq(d.bits[2], 0u);
  ck_assert_int_eq(get_scale(d), 0);
  ck_assert_int_eq(get_sign(d), 0);
}
END_TEST

START_TEST(int_to_dec_positive) {
  s21_decimal d;
  int code = s21_from_int_to_decimal(12345, &d);
  ck_assert_int_eq(code, OK);
  ck_assert_uint_eq(d.bits[0], 12345u);
  ck_assert_int_eq(get_sign(d), 0);
  ck_assert_int_eq(get_scale(d), 0);
}
END_TEST

START_TEST(int_to_dec_negative) {
  s21_decimal d;
  int code = s21_from_int_to_decimal(-54321, &d);
  ck_assert_int_eq(code, OK);
  ck_assert_uint_eq(d.bits[0], 54321u);
  ck_assert_int_eq(get_sign(d), 1);
  ck_assert_int_eq(get_scale(d), 0);
}
END_TEST

START_TEST(int_to_dec_intmax) {
  s21_decimal d;
  int code = s21_from_int_to_decimal(INT_MAX, &d);
  ck_assert_int_eq(code, OK);
  ck_assert_uint_eq(d.bits[0], (uint32_t)INT_MAX);
  ck_assert_int_eq(get_sign(d), 0);
}
END_TEST

START_TEST(int_to_dec_intmin) {
  s21_decimal d;
  int code = s21_from_int_to_decimal(INT_MIN, &d);
  ck_assert_int_eq(code, OK);
  ck_assert_uint_eq((uint32_t)d.bits[0], 0x80000000u);
  ck_assert_int_eq(get_sign(d), 1);
}
END_TEST

Suite *test_from_int_to_decimal(void) {
  Suite *s = suite_create("=S21_FROM_INT_TO_DECIMAL=");
  TCase *tc = tcase_create("from_int_to_decimal_tc");

  tcase_add_test(tc, int_to_dec_null_pointer);
  tcase_add_test(tc, int_to_dec_zero);
  tcase_add_test(tc, int_to_dec_positive);
  tcase_add_test(tc, int_to_dec_negative);
  tcase_add_test(tc, int_to_dec_intmax);
  tcase_add_test(tc, int_to_dec_intmin);

  suite_add_tcase(s, tc);
  return s;
}