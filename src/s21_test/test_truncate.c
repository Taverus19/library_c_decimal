#include "test.h"

START_TEST(test_truncate_positive_fraction) {
  s21_decimal value = {{789, 0, 0, 0x00020000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_negative_fraction) {
  s21_decimal value = {{314, 0, 0, 0x80020000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_positive_integer) {
  s21_decimal value = {{4200, 0, 0, 0x00020000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_negative_integer) {
  s21_decimal value = {{9900, 0, 0, 0x80020000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_large_positive) {
  s21_decimal value = {{0x75BCD15, 0x1, 0, 0x00090000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_large_negative) {
  s21_decimal value = {{0x75BCD15, 0x1, 0, 0x80090000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_zero) {
  s21_decimal value = {{0, 0, 0, 0x00020000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_already_integer) {
  s21_decimal value = {{123, 0, 0, 0x00000000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_max_scale) {
  s21_decimal value = {{123456789, 0, 0, 0x001C0000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_negative_small_fraction) {
  s21_decimal value = {{999, 0, 0, 0x80030000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_truncate_invalid_decimal) {
  s21_decimal value = {{0, 0, 0, 0x7F000000}};
  s21_decimal result;
  int status = s21_truncate(value, &result);
  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_truncate_null_result) {
  s21_decimal value = {{12345, 0, 0, 0x00020000}};
  int status = s21_truncate(value, NULL);
  ck_assert_int_eq(status, 1);
}
END_TEST

Suite *test_truncate(void) {
  Suite *s = suite_create("=S21_TRUNCATE=");
  TCase *tc_truncate = tcase_create("truncate");
  tcase_add_test(tc_truncate, test_truncate_positive_fraction);
  tcase_add_test(tc_truncate, test_truncate_negative_fraction);
  tcase_add_test(tc_truncate, test_truncate_positive_integer);
  tcase_add_test(tc_truncate, test_truncate_negative_integer);
  tcase_add_test(tc_truncate, test_truncate_large_positive);
  tcase_add_test(tc_truncate, test_truncate_large_negative);
  tcase_add_test(tc_truncate, test_truncate_zero);
  tcase_add_test(tc_truncate, test_truncate_already_integer);
  tcase_add_test(tc_truncate, test_truncate_max_scale);
  tcase_add_test(tc_truncate, test_truncate_negative_small_fraction);
  tcase_add_test(tc_truncate, test_truncate_invalid_decimal);
  tcase_add_test(tc_truncate, test_truncate_null_result);
  suite_add_tcase(s, tc_truncate);
  return s;
}