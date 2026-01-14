#include <limits.h>

#include "../s21_decimal.h"
#include "../s21_helper_functions.h"
#include "test.h"

void init_decimal(s21_decimal *d, unsigned int b0, unsigned int b1,
                  unsigned int b2, int sign, int scale) {
  d->bits[0] = b0;
  d->bits[1] = b1;
  d->bits[2] = b2;
  set_sign(d, sign);
  set_scale(d, scale);
}

int is_equal(s21_decimal a, s21_decimal b) {
  for (int i = 0; i < 4; i++) {
    if (a.bits[i] != b.bits[i]) {
      return 0;
    }
  }
  return 1;
}

START_TEST(test_div_by_zero) {
  s21_decimal value_1 = {{1, 0, 0, 0}};
  s21_decimal value_2 = {{0, 0, 0, 0}};
  s21_decimal result;
  int code = s21_div(value_1, value_2, &result);
  ck_assert_int_eq(code, 3);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
  ck_assert_uint_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_zero_div_zero) {
  s21_decimal value_1 = {{0, 0, 0, 0}};
  s21_decimal value_2 = {{0, 0, 0, 0}};
  s21_decimal result;
  int code = s21_div(value_1, value_2, &result);
  ck_assert_int_eq(code, 3);
}
END_TEST

START_TEST(test_positive_div_positive) {
  s21_decimal value_1 = {{10, 0, 0, 0}};
  s21_decimal value_2 = {{5, 0, 0, 0}};
  s21_decimal result;
  int code = s21_div(value_1, value_2, &result);
  ck_assert_int_eq(code, 0);
  ck_assert_uint_eq(result.bits[0], 2);
  ck_assert_uint_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_negative_div_negative) {
  s21_decimal value_1, value_2;
  init_decimal(&value_1, 10, 0, 0, 1, 0);
  init_decimal(&value_2, 5, 0, 0, 1, 0);
  s21_decimal result;
  int code = s21_div(value_1, value_2, &result);
  ck_assert_int_eq(code, 0);
  ck_assert_uint_eq(result.bits[0], 2);
  ck_assert_uint_eq(result.bits[3] & 0x7FFFFFFF, 0);
}
END_TEST

START_TEST(test_positive_div_negative) {
  s21_decimal value_1, value_2;
  init_decimal(&value_1, 10, 0, 0, 0, 0);
  init_decimal(&value_2, 5, 0, 0, 1, 0);
  s21_decimal result;
  int code = s21_div(value_1, value_2, &result);
  ck_assert_int_eq(code, 0);
  ck_assert_uint_eq(result.bits[0], 2);
  ck_assert_uint_eq((unsigned)result.bits[3] >> 31, 1);
}
END_TEST

START_TEST(test_negative_div_positive) {
  s21_decimal value_1, value_2;
  init_decimal(&value_1, 10, 0, 0, 1, 0);
  init_decimal(&value_2, 5, 0, 0, 0, 0);
  s21_decimal result;
  int code = s21_div(value_1, value_2, &result);
  ck_assert_int_eq(code, 0);
  ck_assert_uint_eq(result.bits[0], 2);
  ck_assert_uint_eq((unsigned)result.bits[3] >> 31, 1);
}
END_TEST

START_TEST(test_high_scale) {
  s21_decimal a, b, result;
  init_decimal(&a, 1, 0, 0, 0, 28);
  init_decimal(&b, 10, 0, 0, 0, 0);
  int ret = s21_div(a, b, &result);
  ck_assert_int_eq(ret, 0);

  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
  ck_assert_uint_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_1_div_3) {
  s21_decimal value_1 = {{1, 0, 0, 0}};
  s21_decimal value_2 = {{3, 0, 0, 0}};
  s21_decimal result;
  int code = s21_div(value_1, value_2, &result);
  ck_assert_int_eq(code, 0);
  ck_assert_int_eq(get_scale(result), 28);

  ck_assert_uint_eq(result.bits[0], 89478485);   // 0x05555555
  ck_assert_uint_eq(result.bits[1], 347537611);  // 0x1500000B
  ck_assert_uint_eq(result.bits[2], 180700362);  // 0x0AC471C2
}
END_TEST

START_TEST(test_2_div_3) {
  s21_decimal value_1 = {{2, 0, 0, 0}};
  s21_decimal value_2 = {{3, 0, 0, 0}};
  s21_decimal result;
  int code = s21_div(value_1, value_2, &result);
  ck_assert_int_eq(code, 0);
  ck_assert_int_eq(get_scale(result), 28);

  ck_assert_uint_eq(result.bits[0], 178956971);
  ck_assert_uint_eq(result.bits[1], 695075222);
  ck_assert_uint_eq(result.bits[2], 361400724);
}
END_TEST

START_TEST(test_div_5e_minus28_by_10) {
  s21_decimal a = {{5, 0, 0, 0}};
  set_scale(&a, 28);
  s21_decimal b = {{10, 0, 0, 0}};
  s21_decimal result = {0};

  int code = s21_div(a, b, &result);

  ck_assert_int_eq(code, 0);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
}
END_TEST

START_TEST(test_div_15e_minus28_by_10) {
  s21_decimal a = {{15, 0, 0, 0}};
  set_scale(&a, 28);
  s21_decimal b = {{10, 0, 0, 0}};
  s21_decimal result = {0};

  int code = s21_div(a, b, &result);

  ck_assert_int_eq(code, 0);
  ck_assert_uint_eq(result.bits[0], 2);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
  ck_assert_uint_eq(result.bits[3], 28 << 16);
}
END_TEST

START_TEST(test_div_25e_minus28_by_10) {
  s21_decimal a = {{25, 0, 0, 0}};
  set_scale(&a, 28);
  s21_decimal b = {{10, 0, 0, 0}};
  s21_decimal result = {0};

  int code = s21_div(a, b, &result);

  ck_assert_int_eq(code, 0);
  ck_assert_uint_eq(result.bits[0], 2);
  ck_assert_uint_eq(result.bits[1], 0);
  ck_assert_uint_eq(result.bits[2], 0);
  ck_assert_uint_eq(result.bits[3], 28 << 16);
}
END_TEST

START_TEST(test_overflow_positive) {
  s21_decimal max = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal tenth = {{1, 0, 0, 0}};
  set_scale(&tenth, 1);
  s21_decimal result;
  int code = s21_div(max, tenth, &result);
  ck_assert_int_eq(code, 1);
}
END_TEST

START_TEST(test_overflow_negative) {
  s21_decimal max = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  set_sign(&max, 1);
  s21_decimal tenth = {{1, 0, 0, 0}};
  set_scale(&tenth, 1);
  s21_decimal result;
  int code = s21_div(max, tenth, &result);
  ck_assert_int_eq(code, 2);
}
END_TEST

START_TEST(test_result_null) {
  s21_decimal value_1 = {{1, 0, 0, 0}};
  s21_decimal value_2 = {{1, 0, 0, 0}};
  int code = s21_div(value_1, value_2, NULL);
  ck_assert_int_eq(code, 1);
}
END_TEST

START_TEST(test_overflow_in_process_fractional_digit_positive) {
  s21_decimal a, b;
  init_decimal(&a, 0xFFFFFFFF, 0xFFFFFFFF, 0x19999999, 0, 0);
  init_decimal(&b, 0xFFFFFFFF, 0xFFFFFFFF, 0x1999999A, 0, 0);
  s21_decimal result;
  int code = s21_div(a, b, &result);
  ck_assert_int_eq(code, 1);
}
END_TEST

START_TEST(test_overflow_in_process_fractional_digit_negative) {
  s21_decimal a, b;
  init_decimal(&a, 0xFFFFFFFF, 0xFFFFFFFF, 0x19999999, 1, 0);
  init_decimal(&b, 0xFFFFFFFF, 0xFFFFFFFF, 0x1999999A, 0, 0);
  s21_decimal result;
  int code = s21_div(a, b, &result);
  ck_assert_int_eq(code, 2);
}
END_TEST

START_TEST(test_overflow_in_adjust_scale_positive) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  set_scale(&b, 1);
  s21_decimal result;
  int code = s21_div(a, b, &result);
  ck_assert_int_eq(code, 1);
}
END_TEST

START_TEST(test_overflow_in_adjust_scale_negative) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  set_scale(&b, 1);
  set_sign(&a, 1);
  s21_decimal result;
  int code = s21_div(a, b, &result);
  ck_assert_int_eq(code, 2);
}
END_TEST

START_TEST(test_overflow_in_process_fractional_digit_mul_res_positive) {
  s21_decimal a, b;
  init_decimal(&a, 0xFFFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF, 0, 0);
  init_decimal(&b, 2, 0, 0, 0, 0);
  s21_decimal result;
  int code = s21_div(a, b, &result);
  ck_assert_int_eq(code, 1);
}
END_TEST

START_TEST(test_overflow_in_process_fractional_digit_mul_res_negative) {
  s21_decimal a, b;
  init_decimal(&a, 0xFFFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF, 1, 0);
  init_decimal(&b, 2, 0, 0, 0, 0);
  s21_decimal result;
  int code = s21_div(a, b, &result);
  ck_assert_int_eq(code, 2);
}
END_TEST

Suite *test_div(void) {
  Suite *s = suite_create("=S21_DIV=");
  TCase *tc_core = tcase_create("div_tc");

  tcase_add_test(tc_core, test_div_by_zero);
  tcase_add_test(tc_core, test_zero_div_zero);
  tcase_add_test(tc_core, test_positive_div_positive);
  tcase_add_test(tc_core, test_negative_div_negative);
  tcase_add_test(tc_core, test_positive_div_negative);
  tcase_add_test(tc_core, test_negative_div_positive);
  tcase_add_test(tc_core, test_high_scale);
  tcase_add_test(tc_core, test_2_div_3);
  tcase_add_test(tc_core, test_1_div_3);
  tcase_add_test(tc_core, test_div_5e_minus28_by_10);
  tcase_add_test(tc_core, test_div_15e_minus28_by_10);
  tcase_add_test(tc_core, test_div_25e_minus28_by_10);
  tcase_add_test(tc_core, test_overflow_positive);
  tcase_add_test(tc_core, test_overflow_negative);

  tcase_add_test(tc_core, test_result_null);
  tcase_add_test(tc_core, test_overflow_in_process_fractional_digit_positive);
  tcase_add_test(tc_core, test_overflow_in_process_fractional_digit_negative);
  tcase_add_test(tc_core, test_overflow_in_adjust_scale_positive);
  tcase_add_test(tc_core, test_overflow_in_adjust_scale_negative);
  tcase_add_test(tc_core,
                 test_overflow_in_process_fractional_digit_mul_res_positive);
  tcase_add_test(tc_core,
                 test_overflow_in_process_fractional_digit_mul_res_negative);

  suite_add_tcase(s, tc_core);
  return s;
}