#include <stdlib.h>

#include "test.h"

START_TEST(test_convert_zero_positive) {
  s21_decimal dec = {{0}};
  float src = 0.0f;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_convert_zero_negative) {
  s21_decimal dec = {{0}};
  float src = -0.0f;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_convert_simple) {
  s21_decimal dec = {{0}};
  float src = 12.34567f;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_convert_simple_negativ) {
  s21_decimal dec = {{0}};
  float src = -8.765432f;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_convert_rounding_to_7_digits) {
  s21_decimal dec = {{0}};
  float src = 1.23456789f;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_convert_small_number) {
  s21_decimal dec = {{0}};
  float src = 1.23e-5f;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_convert_very_small_number_error) {
  s21_decimal dec = {{0}};
  float src = 1e-29f;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_convert_large_number) {
  s21_decimal dec = {{0}};
  float src = 12345670.0f;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_convert_too_large_number_error) {
  s21_decimal dec = {{0}};
  float src = 1e30f;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_convert_nan) {
  s21_decimal dec = {{0}};
  float src = NAN;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_convert_inf_positive) {
  s21_decimal dec = {{0}};
  float src = INFINITY;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_convert_inf_negative) {
  s21_decimal dec = {{0}};
  float src = -INFINITY;
  int status = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_null_point) {
  float src = 123.456f;
  int status = s21_from_float_to_decimal(src, NULL);
  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_scale_exactly_28) {
  float src = 2e-28f;
  s21_decimal result;
  int status = s21_from_float_to_decimal(src, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

Suite *test_from_float_to_decimal(void) {
  Suite *s = suite_create("=S21_FROM_FLOAT_TO_DECIMAL=");
  TCase *tc_from_float_to_decimal = tcase_create("from_float_to_decimal");
  tcase_add_test(tc_from_float_to_decimal, test_convert_zero_positive);
  tcase_add_test(tc_from_float_to_decimal, test_convert_zero_negative);
  tcase_add_test(tc_from_float_to_decimal, test_convert_simple);
  tcase_add_test(tc_from_float_to_decimal, test_convert_simple_negativ);
  tcase_add_test(tc_from_float_to_decimal, test_convert_rounding_to_7_digits);
  tcase_add_test(tc_from_float_to_decimal, test_convert_small_number);
  tcase_add_test(tc_from_float_to_decimal,
                 test_convert_very_small_number_error);
  tcase_add_test(tc_from_float_to_decimal, test_convert_large_number);
  tcase_add_test(tc_from_float_to_decimal, test_convert_too_large_number_error);
  tcase_add_test(tc_from_float_to_decimal, test_convert_nan);
  tcase_add_test(tc_from_float_to_decimal, test_convert_inf_positive);
  tcase_add_test(tc_from_float_to_decimal, test_convert_inf_negative);
  tcase_add_test(tc_from_float_to_decimal, test_null_point);
  tcase_add_test(tc_from_float_to_decimal, test_scale_exactly_28);
  suite_add_tcase(s, tc_from_float_to_decimal);
  return s;
}