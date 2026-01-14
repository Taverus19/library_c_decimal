#include "test.h"

static void make_decimal(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t scale,
                         int sign, s21_decimal *out) {
  out->bits[0] = w0;
  out->bits[1] = w1;
  out->bits[2] = w2;
  out->bits[3] = 0;
  set_scale(out, scale);
  set_sign(out, sign);
}

START_TEST(from_dec_to_int_null_dst) {
  s21_decimal a;
  s21_from_int_to_decimal(5, &a);
  int code = s21_from_decimal_to_int(a, NULL);
  ck_assert_int_eq(code, CONVERTATION_ERROR);
}
END_TEST

START_TEST(from_dec_to_int_invalid_dec) {
  s21_decimal a = {{123, 0, 0, 0}};
  set_scale(&a, 29);
  int dst = 0;
  int code = s21_from_decimal_to_int(a, &dst);
  ck_assert_int_eq(code, CONVERTATION_ERROR);
}
END_TEST

START_TEST(from_dec_to_int_simple_ok) {
  s21_decimal a;
  s21_from_int_to_decimal(12345, &a);
  int dst = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(a, &dst), OK);
  ck_assert_int_eq(dst, 12345);
}
END_TEST

START_TEST(from_dec_to_int_max_ok) {
  s21_decimal a;
  s21_from_int_to_decimal(2147483647, &a);
  int dst = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(a, &dst), OK);
  ck_assert_int_eq(dst, 2147483647);
}
END_TEST

START_TEST(from_dec_to_int_neg_max_ok) {
  s21_decimal a;
  s21_from_int_to_decimal(-2147483647, &a);
  int dst = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(a, &dst), OK);
  ck_assert_int_eq(dst, -2147483647);
}
END_TEST

START_TEST(from_dec_to_int_min_ok) {
  s21_decimal a;
  make_decimal(0x80000000, 0, 0, 0, 1, &a);
  int dst = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(a, &dst), OK);
  ck_assert_int_eq(dst, -2147483648);
}
END_TEST

START_TEST(from_dec_to_int_overflow_pos) {
  s21_decimal a;
  make_decimal(0x80000000, 0, 0, 0, 0, &a);
  int dst = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(a, &dst), CONVERTATION_ERROR);
}
END_TEST

START_TEST(from_dec_to_int_overflow_neg) {
  s21_decimal a;
  make_decimal(0x80000001, 0, 0, 0, 1, &a);
  int dst = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(a, &dst), CONVERTATION_ERROR);
}
END_TEST

START_TEST(from_dec_to_int_frac_pos) {
  s21_decimal a;
  s21_from_float_to_decimal(123.9f, &a);
  int dst = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(a, &dst), OK);
  ck_assert_int_eq(dst, 123);
}
END_TEST

START_TEST(from_dec_to_int_frac_neg) {
  s21_decimal a;
  s21_from_float_to_decimal(-123.9f, &a);
  int dst = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(a, &dst), OK);
  ck_assert_int_eq(dst, -123);
}
END_TEST

START_TEST(from_dec_to_int_neg_zero) {
  s21_decimal a;
  s21_from_int_to_decimal(0, &a);
  set_sign(&a, 1);
  int dst = 42;
  ck_assert_int_eq(s21_from_decimal_to_int(a, &dst), OK);
  ck_assert_int_eq(dst, 0);
}
END_TEST

Suite *test_from_decimal_to_int(void) {
  Suite *s = suite_create("=S21_FROM_DECIMAL_TO_INT=");
  TCase *tc = tcase_create("from_dec_to_int_tc");

  tcase_add_test(tc, from_dec_to_int_null_dst);
  tcase_add_test(tc, from_dec_to_int_invalid_dec);
  tcase_add_test(tc, from_dec_to_int_simple_ok);
  tcase_add_test(tc, from_dec_to_int_max_ok);
  tcase_add_test(tc, from_dec_to_int_neg_max_ok);
  tcase_add_test(tc, from_dec_to_int_min_ok);
  tcase_add_test(tc, from_dec_to_int_overflow_pos);
  tcase_add_test(tc, from_dec_to_int_overflow_neg);
  tcase_add_test(tc, from_dec_to_int_frac_pos);
  tcase_add_test(tc, from_dec_to_int_frac_neg);
  tcase_add_test(tc, from_dec_to_int_neg_zero);

  suite_add_tcase(s, tc);
  return s;
}