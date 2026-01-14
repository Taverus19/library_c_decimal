#include "test.h"

START_TEST(test_floor_positive_fraction) {
  s21_decimal value = {{789, 0, 0, 0x00020000}};
  s21_decimal result;
  int status = s21_floor(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_floor_positive_integer) {
  s21_decimal value = {{4200, 0, 0, 0x00020000}};
  s21_decimal result;
  int status = s21_floor(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_floor_negative_fraction) {
  s21_decimal value = {{314, 0, 0, 0x80020000}};
  s21_decimal result;
  int status = s21_floor(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_floor_negative_integer) {
  s21_decimal value = {{9900, 0, 0, 0x80020000}};
  s21_decimal result;
  int status = s21_floor(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_floor_zero) {
  s21_decimal value = {{0, 0, 0, 0x00020000}};
  s21_decimal result;
  int status = s21_floor(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_floor_zero_scale_no_rounding) {
  s21_decimal value = {{12345, 0, 0, 0x00000000}};
  s21_decimal result;
  int status = s21_floor(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_floor_invalid_decimal) {
  s21_decimal value = {{0, 0, 0, 0x7F000000}};
  s21_decimal result;
  int status = s21_floor(value, &result);
  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_floor_null_result) {
  s21_decimal value = {{12345, 0, 0, 0x00020000}};
  int status = s21_floor(value, NULL);
  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_floor_positive_no_adjustment) {
  s21_decimal value = {{78, 0, 0, 0x00010000}};
  s21_decimal result;

  int status = s21_floor(value, &result);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_floor_negative_has_fraction_detected) {
  // -3.14: bits[0] = 314, scale = 2, sign = 1
  s21_decimal value = {{314, 0, 0, 0x80020000}};
  s21_decimal result;

  int status = s21_floor(value, &result);

  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_floor_negative_no_fraction_not_detected) {
  // При делении на 10: 50 / 10 = 5, remainder = 0
  s21_decimal value = {{50, 0, 0, 0x80010000}};  // -5.0
  s21_decimal result;

  int status = s21_floor(value, &result);

  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_floor_fraction_with_zero_digit) {
  // 12.0: 120 / 10^1 = 12.0, scale = 1
  s21_decimal value = {{120, 0, 0, 0x00010000}};  // +12.0
  s21_decimal result;

  int status = s21_floor(value, &result);

  ck_assert_int_eq(status, 0);
}
END_TEST

Suite *test_floor(void) {
  Suite *s = suite_create("=S21_FLOOR=");
  TCase *tc_floor = tcase_create("floor");
  tcase_add_test(tc_floor, test_floor_positive_fraction);
  tcase_add_test(tc_floor, test_floor_positive_integer);
  tcase_add_test(tc_floor, test_floor_negative_fraction);
  tcase_add_test(tc_floor, test_floor_negative_integer);
  tcase_add_test(tc_floor, test_floor_zero);
  tcase_add_test(tc_floor, test_floor_invalid_decimal);
  tcase_add_test(tc_floor, test_floor_null_result);
  tcase_add_test(tc_floor, test_floor_zero_scale_no_rounding);
  tcase_add_test(tc_floor, test_floor_positive_no_adjustment);
  tcase_add_test(tc_floor, test_floor_negative_has_fraction_detected);
  tcase_add_test(tc_floor, test_floor_negative_no_fraction_not_detected);
  tcase_add_test(tc_floor, test_floor_fraction_with_zero_digit);
  suite_add_tcase(s, tc_floor);
  return s;
}