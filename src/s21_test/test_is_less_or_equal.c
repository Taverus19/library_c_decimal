#include "test.h"

START_TEST(s21_is_less_or_equal_1) {
  s21_decimal value_1 = {{256584u, 25698u, 0xFFFFFFFF, 0}};
  s21_decimal value_2 = {{256583u, 25698u, 0xFFFFFFFF, 0}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 0);
}
END_TEST

START_TEST(s21_is_less_or_equal_2) {
  s21_decimal value_1 = {{256584u, 25698u, 0xFFFFFFFF, MINUS}};
  s21_decimal value_2 = {{256583u, 25698u, 0xFFFFFFFF, 0}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_3) {
  s21_decimal value_1 = {{256583u, 25698u, 0xFFFFFFFF, 0}};
  s21_decimal value_2 = {{256584u, 25698u, 0xFFFFFFFF, MINUS}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 0);
}
END_TEST

START_TEST(s21_is_less_or_equal_4) {
  s21_decimal value_1 = {{0, 0, 0, 0}};
  s21_decimal value_2 = {{0, 0, 0, MINUS}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_5) {
  s21_decimal value_1 = {{256583u, 25698u, 0xFFFFFFFF, 0}};
  s21_decimal value_2 = {{256584u, 25698u, 0xFFFFFFFF, 0}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_6) {
  s21_decimal value_1 = {{256583u, 25698u, 0xFFFFFFFF, MINUS}};
  s21_decimal value_2 = {{256583u, 25698u, 0xFFFFFFFF, 0}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_7) {
  s21_decimal value_1 = {{-1, -1, -1, MINUS}};
  s21_decimal value_2 = {{-1, -1, -1, MINUS}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_8) {
  s21_decimal value_1 = {{0, 0, 0, MINUS}};
  s21_decimal value_2 = {{0, 0, 0, 0}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_9) {
  s21_decimal value_1 = {{0, 0, 0, 0}};
  s21_decimal value_2 = {{0, 0, 0, 0}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_10) {
  s21_decimal value_1 = {{0, 0, 0, 0x801C0000}};
  s21_decimal value_2 = {{0, 0, 0, 0x801B0000}};
  int result = s21_is_less_or_equal(value_1, value_2);
  ck_assert_int_eq(result, 1);
}
END_TEST

static s21_decimal dec(unsigned lo, unsigned mid, unsigned hi, int scale,
                       int sign) {
  s21_decimal d = {{lo, mid, hi, 0}};
  d.bits[3] = ((unsigned)scale << 16) | (sign ? 0x80000000u : 0u);
  return d;
}

static const unsigned MAX32 = 0xFFFFFFFFu;

START_TEST(both_zero_pos_neg) {
  s21_decimal a = dec(0, 0, 0, 0, 0);
  s21_decimal b = dec(0, 0, 0, 0, 1);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(sign_mismatch_pos_vs_neg) {
  s21_decimal a = dec(1, 0, 0, 0, 0);
  s21_decimal b = dec(1, 0, 0, 0, 1);

  ck_assert_int_eq(s21_is_less_or_equal(a, b), 0);
}
END_TEST

START_TEST(sign_mismatch_neg_vs_pos) {
  s21_decimal a = dec(1, 0, 0, 0, 1);
  s21_decimal b = dec(1, 0, 0, 0, 0);

  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(pos_less_no_overflow) {
  s21_decimal a = dec(12, 0, 0, 1, 0);
  s21_decimal b = dec(13, 0, 0, 1, 0);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(pos_equal_diffB) {
  s21_decimal a = dec(130, 0, 0, 2, 0);
  s21_decimal b = dec(13, 0, 0, 1, 0);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(pos_greater_diffA) {
  s21_decimal a = dec(251, 0, 0, 2, 0);
  s21_decimal b = dec(25, 0, 0, 1, 0);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 0);
}
END_TEST

START_TEST(neg_equal_no_overflow) {
  s21_decimal a = dec(130, 0, 0, 2, 1);
  s21_decimal b = dec(13, 0, 0, 1, 1);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(neg_cmp_positive) {
  s21_decimal a = dec(24, 0, 0, 1, 1);
  s21_decimal b = dec(23, 0, 0, 1, 1);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(neg_cmp_negative) {
  s21_decimal a = dec(23, 0, 0, 1, 1);
  s21_decimal b = dec(24, 0, 0, 1, 1);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 0);
}
END_TEST

START_TEST(overflowA_pos_sign) {
  s21_decimal a = dec(MAX32, MAX32, MAX32, 0, 0);
  s21_decimal b = dec(0, 0, 0, 1, 0);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 0);
}
END_TEST

START_TEST(overflowA_neg_sign) {
  s21_decimal a = dec(MAX32, MAX32, MAX32, 0, 1);
  s21_decimal b = dec(0, 0, 0, 1, 1);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(overflowB_pos_sign) {
  s21_decimal a = dec(1, 0, 0, 1, 0);
  s21_decimal b = dec(MAX32, MAX32, MAX32, 0, 0);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(overflowB_neg_sign) {
  s21_decimal a = dec(1, 0, 0, 1, 1);
  s21_decimal b = dec(MAX32, MAX32, MAX32, 0, 1);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 0);
}
END_TEST

Suite *test_is_less_or_equal(void) {
  Suite *s = suite_create("=S21_IS_LESS_OR_EQUAL=");
  TCase *tc = tcase_create("is_less_or_equal_tc");

  tcase_add_test(tc, s21_is_less_or_equal_1);
  tcase_add_test(tc, s21_is_less_or_equal_2);
  tcase_add_test(tc, s21_is_less_or_equal_3);
  tcase_add_test(tc, s21_is_less_or_equal_4);
  tcase_add_test(tc, s21_is_less_or_equal_5);
  tcase_add_test(tc, s21_is_less_or_equal_6);
  tcase_add_test(tc, s21_is_less_or_equal_7);
  tcase_add_test(tc, s21_is_less_or_equal_8);
  tcase_add_test(tc, s21_is_less_or_equal_9);
  tcase_add_test(tc, s21_is_less_or_equal_10);
  tcase_add_test(tc, both_zero_pos_neg);

  tcase_add_test(tc, sign_mismatch_pos_vs_neg);
  tcase_add_test(tc, sign_mismatch_neg_vs_pos);

  tcase_add_test(tc, pos_less_no_overflow);
  tcase_add_test(tc, pos_equal_diffB);
  tcase_add_test(tc, pos_greater_diffA);

  tcase_add_test(tc, neg_equal_no_overflow);
  tcase_add_test(tc, neg_cmp_positive);
  tcase_add_test(tc, neg_cmp_negative);

  tcase_add_test(tc, overflowA_pos_sign);
  tcase_add_test(tc, overflowA_neg_sign);
  tcase_add_test(tc, overflowB_pos_sign);
  tcase_add_test(tc, overflowB_neg_sign);

  suite_add_tcase(s, tc);
  return s;
}