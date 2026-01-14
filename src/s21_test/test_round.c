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

START_TEST(round_almost_max_scale) {
  s21_decimal a, res;
  make_decimal(UINT32_MAX, UINT32_MAX, UINT32_MAX - 1, 1, 0, &a);
  int code = s21_round(a, &res);

  ck_assert_int_eq(code, OK);
}
END_TEST

START_TEST(round_full_max_scale0) {
  s21_decimal a, res;
  make_decimal(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0, &a);
  int code = s21_round(a, &res);
  ck_assert_int_eq(code, OK);

  ck_assert_uint_eq((uint32_t)res.bits[0], UINT32_MAX);
  ck_assert_uint_eq((uint32_t)res.bits[1], UINT32_MAX);
  ck_assert_uint_eq((uint32_t)res.bits[2], UINT32_MAX);
}
END_TEST

START_TEST(round_remainder5_with_tail) {
  s21_decimal a, res;
  s21_from_float_to_decimal(2.501f, &a);
  int code = s21_round(a, &res);
  int ir;
  s21_from_decimal_to_int(res, &ir);
  ck_assert_int_eq(code, OK);
  ck_assert_int_eq(ir, 3);
}
END_TEST

START_TEST(round_remainder5_no_tail_odd) {
  s21_decimal a, res;
  s21_from_float_to_decimal(3.5f, &a);
  int code = s21_round(a, &res);
  int ir;
  s21_from_decimal_to_int(res, &ir);
  ck_assert_int_eq(code, OK);
  ck_assert_int_eq(ir, 4);
}
END_TEST

START_TEST(round_remainder5_no_tail_even) {
  s21_decimal a, res;
  s21_from_float_to_decimal(4.5f, &a);
  int code = s21_round(a, &res);
  int ir;
  s21_from_decimal_to_int(res, &ir);
  ck_assert_int_eq(code, OK);
  ck_assert_int_eq(ir, 4);
}
END_TEST

START_TEST(s21_round_1) {
  s21_decimal value_2 = {0};
  s21_decimal result = {0};
  s21_from_float_to_decimal(1.7111000, &value_2);
  int return_value = s21_round(value_2, &result), result_int = 0;
  s21_from_decimal_to_int(result, &result_int);
  ck_assert_int_eq(return_value, 0);
  ck_assert_int_eq(result_int, 2);
}
END_TEST

START_TEST(s21_round_3) {
  s21_decimal value_2 = {{0xFFFFFFFF, 0, 0xFFFFFFFF, 0x80020000}};
  s21_decimal result = {0};
  int return_value = s21_round(value_2, &result);
  ck_assert_int_eq(return_value, 0);
}
END_TEST

START_TEST(s21_round_4) {
  s21_decimal value_2 = {0};
  s21_decimal result = {0};
  s21_from_float_to_decimal(2.2, &value_2);
  int return_value = s21_round(value_2, &result), result_int = 0;
  s21_from_decimal_to_int(result, &result_int);
  ck_assert_int_eq(return_value, 0);
  ck_assert_int_eq(result_int, 2);
}
END_TEST

START_TEST(s21_round_5) {
  s21_decimal value_2 = {{15, 0, 0, pow(2, 16)}};
  s21_decimal result = {0};
  int return_value = s21_round(value_2, &result);
  ck_assert_int_eq(return_value, 0);
}
END_TEST

START_TEST(s21_round_6) {
  s21_decimal value_2 = {{15, 0, 0, 0x80010000}};
  s21_decimal result = {0};
  int return_value = s21_round(value_2, &result);
  ck_assert_int_eq(return_value, 0);
}
END_TEST

START_TEST(s21_round_7) {
  s21_decimal value_2 = {{16, 0, 0, 0x80010000}};
  s21_decimal result = {0};
  s21_decimal reference = {{20, 0, 0, 0x80010000}};
  int return_value = s21_round(value_2, &result);
  ck_assert_int_eq(s21_is_equal(result, reference), 1);
  ck_assert_int_eq(return_value, 0);
}
END_TEST

START_TEST(s21_round_2) {
  s21_decimal value_2 = {{0xFFFFFFFF, 0, 0xFFFFFFFF, MINUS}};
  s21_decimal result = {0};
  int return_value = s21_round(value_2, &result);
  ck_assert_int_eq(return_value, OK);
  ck_assert_uint_eq((unsigned)result.bits[2], 0xFFFFFFFFu);
}
END_TEST

START_TEST(s21_round_8) {
  s21_decimal decimal = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00180101}};
  s21_decimal result = {0};

  int code = s21_round(decimal, &result);

  ck_assert_int_eq(code, NUMBER_IS_TOO_BIG);
}

START_TEST(s21_round_9) {
  s21_decimal decimal = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xF0180000}};

  s21_decimal result = {0};

  int code = s21_round(decimal, &result);

  ck_assert_int_eq(code, NUMBER_IS_TOO_SMALL);
}
END_TEST

START_TEST(s21_round_10) {
  s21_decimal decimal = {{1, 0, 0, 0x001c0000}};
  s21_decimal result = {0};
  s21_decimal reference = {0};

  int return_value = s21_round(decimal, &result);
  ck_assert_int_eq(s21_is_equal(result, reference), 1);
  ck_assert_int_eq(return_value, 0);
}

START_TEST(s21_NULL) {
  s21_decimal decimal = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x140000}};
  int code = s21_round(decimal, NULL);
  ck_assert_int_eq(code, 1);
}
END_TEST

START_TEST(s21_round_late_overflow_big) {
  s21_decimal dec = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0x00010000}};

  s21_decimal result = {0};
  int code = s21_round(dec, &result);
  ck_assert_int_eq(code, NUMBER_IS_TOO_BIG);
}
END_TEST

START_TEST(s21_round_late_overflow_small) {
  s21_decimal dec = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0x80010000}};

  s21_decimal result = {0};
  int code = s21_round(dec, &result);
  ck_assert_int_eq(code, NUMBER_IS_TOO_SMALL);
}
END_TEST

START_TEST(s21_round_carry_overflow) {
  s21_decimal dec = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0x00010000}};

  dec.bits[0] = UINT32_MAX;
  dec.bits[1] = UINT32_MAX;
  dec.bits[2] = UINT32_MAX;
  s21_decimal result = {0};
  int code = s21_round(dec, &result);
  ck_assert_int_eq(code, NUMBER_IS_TOO_BIG);
}
END_TEST

START_TEST(round_carry_overflow) {
  s21_decimal a, res;

  a.bits[0] = UINT32_MAX;
  a.bits[1] = UINT32_MAX;
  a.bits[2] = UINT32_MAX;
  a.bits[3] = 0;
  set_scale(&a, 1);

  int code = s21_round(a, &res);

  ck_assert_int_eq(code, NUMBER_IS_TOO_BIG);
}
END_TEST

Suite *test_round(void) {
  Suite *s = suite_create("=S21_ROUND=");
  TCase *tc = tcase_create("round_tc");

  tcase_add_test(tc, s21_round_1);
  tcase_add_test(tc, s21_round_2);
  tcase_add_test(tc, s21_round_3);
  tcase_add_test(tc, s21_round_4);
  tcase_add_test(tc, s21_round_5);
  tcase_add_test(tc, s21_round_6);
  tcase_add_test(tc, s21_round_7);
  tcase_add_test(tc, s21_round_8);
  tcase_add_test(tc, s21_round_9);
  tcase_add_test(tc, s21_round_10);

  tcase_add_test(tc, s21_round_late_overflow_big);
  tcase_add_test(tc, s21_round_late_overflow_small);
  tcase_add_test(tc, s21_round_carry_overflow);

  tcase_add_test(tc, round_almost_max_scale);
  tcase_add_test(tc, round_full_max_scale0);
  tcase_add_test(tc, round_remainder5_with_tail);
  tcase_add_test(tc, round_remainder5_no_tail_odd);
  tcase_add_test(tc, round_remainder5_no_tail_even);

  tcase_add_test(tc, round_carry_overflow);

  tcase_add_test(tc, s21_NULL);
  suite_add_tcase(s, tc);
  return s;
}