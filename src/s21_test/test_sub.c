#include <check.h>

#include "../s21_decimal.h"
#include "../s21_helper_functions.h"

static void make_decimal(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t scale,
                         int sign, s21_decimal *out) {
  out->bits[0] = w0;
  out->bits[1] = w1;
  out->bits[2] = w2;
  out->bits[3] = 0;
  set_scale(out, scale);
  set_sign(out, sign);
}

START_TEST(sub_null_result) {
  s21_decimal a, b;
  s21_from_int_to_decimal(1, &a);
  s21_from_int_to_decimal(2, &b);
  int code = s21_sub(a, b, NULL);
  ck_assert_int_eq(code, CALCULATION_ERROR);
}
END_TEST

START_TEST(sub_both_zero) {
  s21_decimal a = {{0}}, b = {{0}}, res;
  int code = s21_sub(a, b, &res);
  ck_assert_int_eq(code, OK);
  ck_assert_uint_eq(res.bits[0], 0u);
}
END_TEST

START_TEST(sub_equal_values) {
  s21_decimal a, b, res;
  s21_from_int_to_decimal(123, &a);
  s21_from_int_to_decimal(123, &b);
  int code = s21_sub(a, b, &res);
  ck_assert_int_eq(code, OK);
  ck_assert_uint_eq(res.bits[0], 0u);
}
END_TEST

START_TEST(sub_simple_positive) {
  s21_decimal a, b, res;
  s21_from_int_to_decimal(100, &a);
  s21_from_int_to_decimal(40, &b);
  s21_sub(a, b, &res);
  int ir;
  s21_from_decimal_to_int(res, &ir);
  ck_assert_int_eq(ir, 60);
}
END_TEST

START_TEST(sub_positive_to_negative) {
  s21_decimal a, b, res;
  s21_from_int_to_decimal(50, &a);
  s21_from_int_to_decimal(60, &b);
  s21_sub(a, b, &res);
  int ir;
  s21_from_decimal_to_int(res, &ir);
  ck_assert_int_eq(ir, -10);
}
END_TEST

START_TEST(sub_diff_signs) {
  s21_decimal a, b, res;
  s21_from_int_to_decimal(70, &a);
  s21_from_int_to_decimal(-20, &b);
  s21_sub(a, b, &res);
  int ir;
  s21_from_decimal_to_int(res, &ir);
  ck_assert_int_eq(ir, 90);
}
END_TEST

START_TEST(sub_frac_diff_scale) {
  s21_decimal a, b, res;
  s21_from_float_to_decimal(2.5f, &a);
  s21_from_float_to_decimal(0.75f, &b);
  s21_sub(a, b, &res);
  float fr;
  s21_from_decimal_to_float(res, &fr);
  ck_assert_float_eq_tol(fr, 1.75f, 1e-6);
}
END_TEST

START_TEST(sub_overflow_big) {
  s21_decimal a, b, res;

  make_decimal(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0, &a);
  set_scale(&a, 0);
  s21_from_int_to_decimal(-1, &b);
  int code = s21_sub(a, b, &res);
  ck_assert_int_eq(code, NUMBER_IS_TOO_BIG);
}
END_TEST

START_TEST(sub_overflow_small) {
  s21_decimal a, b, res;

  make_decimal(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 1, &a);
  s21_from_int_to_decimal(1, &b);
  int code = s21_sub(a, b, &res);
  ck_assert_int_eq(code, NUMBER_IS_TOO_SMALL);
}
END_TEST

START_TEST(sub_large_scale_align) {
  s21_decimal a, b, res;
  s21_from_float_to_decimal(0.0000001f, &a);
  s21_from_int_to_decimal(1, &b);
  s21_sub(a, b, &res);
  float fr;
  s21_from_decimal_to_float(res, &fr);
  ck_assert_float_eq_tol(fr, -0.9999999f, 1e-6);
}
END_TEST

START_TEST(sub_mul10_error_in_scale1) {
  s21_decimal a, b, res;
  make_decimal(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0, &a);
  set_scale(&a, 0);
  s21_from_int_to_decimal(1, &b);
  set_scale(&b, 1);
  int code = s21_sub(a, b, &res);
  ck_assert_int_eq(code, CALCULATION_ERROR);
}
END_TEST

Suite *test_sub(void) {
  Suite *s = suite_create("=S21_SUB=");
  TCase *tc = tcase_create("sub_tc");

  tcase_add_test(tc, sub_null_result);
  tcase_add_test(tc, sub_both_zero);
  tcase_add_test(tc, sub_equal_values);
  tcase_add_test(tc, sub_simple_positive);
  tcase_add_test(tc, sub_positive_to_negative);
  tcase_add_test(tc, sub_diff_signs);
  tcase_add_test(tc, sub_frac_diff_scale);
  tcase_add_test(tc, sub_overflow_big);
  tcase_add_test(tc, sub_overflow_small);
  tcase_add_test(tc, sub_large_scale_align);
  tcase_add_test(tc, sub_mul10_error_in_scale1);

  suite_add_tcase(s, tc);
  return s;
}