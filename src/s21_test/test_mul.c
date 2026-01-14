#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "test.h"

START_TEST(test_mul_ok) {
  s21_decimal a = {{2, 0, 0, 0x00000000}};
  s21_decimal b = {{3, 0, 0, 0x00000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, OK);
  ck_assert_uint_eq(res.bits[0], 6);
  ck_assert_uint_eq(get_scale(res), 0);
  ck_assert_uint_eq(get_sign(res), 0);
}
END_TEST

START_TEST(test_mul_zero_left) {
  s21_decimal a = {{0, 0, 0, 0x00000000}};
  s21_decimal b = {{123, 0, 0, 0x00000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, OK);
  ck_assert_uint_eq(res.bits[0], 0);
}
END_TEST

START_TEST(test_mul_zero_right) {
  s21_decimal a = {{123, 0, 0, 0x00000000}};
  s21_decimal b = {{0, 0, 0, 0x00000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, OK);
  ck_assert_uint_eq(res.bits[0], 0);
}
END_TEST

START_TEST(test_mul_one) {
  s21_decimal a = {{1, 0, 0, 0x00000000}};
  s21_decimal b = {{123, 0, 0, 0x00000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, OK);
  ck_assert_uint_eq(res.bits[0], 123);
}
END_TEST

START_TEST(test_mul_sign_pos_neg) {
  s21_decimal a = {{2, 0, 0, 0x00000000}};
  s21_decimal b = {{3, 0, 0, 0x80000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, OK);
  ck_assert_uint_eq(res.bits[0], 6);
  ck_assert_uint_eq(get_sign(res), 1);
}
END_TEST

START_TEST(test_mul_sign_neg_neg) {
  s21_decimal a = {{2, 0, 0, 0x80000000}};
  s21_decimal b = {{3, 0, 0, 0x80000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, OK);
  ck_assert_uint_eq(res.bits[0], 6);
  ck_assert_uint_eq(get_sign(res), 0);
}
END_TEST

START_TEST(test_mul_scale) {
  s21_decimal a = {{12, 0, 0, 0x00010000}};
  s21_decimal b = {{15, 0, 0, 0x00010000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, OK);
  ck_assert_uint_eq(res.bits[0], 180);
  ck_assert_uint_eq(get_scale(res), 2);
}
END_TEST

START_TEST(test_mul_scale_reduce) {
  s21_decimal a = {{1, 0, 0, 0x001C0000}};
  s21_decimal b = {{1, 0, 0, 0x001C0000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, OK);
  ck_assert_uint_le(get_scale(res), 28);
}
END_TEST

START_TEST(test_mul_too_big) {
  s21_decimal a = {{UINT_MAX, UINT_MAX, UINT_MAX, 0x00000000}};
  s21_decimal b = {{2, 0, 0, 0x00000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, NUMBER_IS_TOO_BIG);
}
END_TEST

START_TEST(test_mul_too_small) {
  s21_decimal a = {{UINT_MAX, UINT_MAX, UINT_MAX, 0x80000000}};
  s21_decimal b = {{2, 0, 0, 0x00000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, NUMBER_IS_TOO_SMALL);
}
END_TEST

START_TEST(test_mul_invalid_decimal) {
  s21_decimal a = {{1, 0, 0, 0x00200000}};
  s21_decimal b = {{1, 0, 0, 0x00000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, NUMBER_IS_TOO_BIG);
}
END_TEST

START_TEST(test_mul_null_result) {
  s21_decimal a = {{1, 0, 0, 0x00000000}};
  s21_decimal b = {{1, 0, 0, 0x00000000}};
  int status = s21_mul(a, b, NULL);
  ck_assert_int_eq(status, NUMBER_IS_TOO_BIG);
}
END_TEST

START_TEST(test_mul_div_by_zero_code) {
  s21_decimal a = {{1, 0, 0, 0x00000000}};
  s21_decimal b = {{1, 0, 0, 0x00000000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);
  ck_assert_int_ne(status, DIVISION_BY_ZERO);
}
END_TEST

START_TEST(test_mul_bankers_rounding) {
  s21_decimal a = {{15, 0, 0, 0x001D0000}};
  s21_decimal b = {{10, 0, 0, 0x001D0000}};
  s21_decimal res = {0};
  int status = s21_mul(a, b, &res);

  ck_assert_int_eq(status, NUMBER_IS_TOO_BIG);
}
END_TEST

static void make_decimal(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t scale,
                         int sign, s21_decimal *out) {
  out->bits[0] = w0;
  out->bits[1] = w1;
  out->bits[2] = w2;
  out->bits[3] = 0;
  set_scale(out, scale);
  set_sign(out, sign);
}

START_TEST(mul_round_last_gt5) {
  s21_decimal a, b, res;
  make_decimal(2, 0, 0, 14, 0, &a);
  make_decimal(8, 0, 0, 15, 0, &b);
  int code = s21_mul(a, b, &res);
  ck_assert_int_eq(code, OK);
  ck_assert_uint_eq((uint32_t)res.bits[0], 2u);
  ck_assert_uint_eq((uint32_t)res.bits[1], 0u);
  ck_assert_uint_eq((uint32_t)res.bits[2], 0u);
  ck_assert_int_eq(get_scale(res), 28);
  ck_assert_int_eq(get_sign(res), 0);
}
END_TEST

START_TEST(mul_round_last5_tail) {
  s21_decimal a, b, res;

  s21_from_float_to_decimal(1.505f, &a);
  s21_from_float_to_decimal(10.0f, &b);
  int code = s21_mul(a, b, &res);
  ck_assert_int_eq(code, OK);
}
END_TEST

START_TEST(mul_round_last5_odd) {
  s21_decimal a, b, res;

  s21_from_float_to_decimal(1.5f, &a);
  s21_from_float_to_decimal(1.0f, &b);
  int code = s21_mul(a, b, &res);
  ck_assert_int_eq(code, OK);
}
END_TEST

START_TEST(mul_round_add1_overflow) {
  s21_decimal a, b, res;

  make_decimal(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0, &a);
  s21_from_int_to_decimal(2, &b);
  int code = s21_mul(a, b, &res);
  ck_assert_int_eq(code, NUMBER_IS_TOO_BIG);
}
END_TEST

START_TEST(mul_round_last5_with_tail) {
  s21_decimal a, b, res;
  make_decimal(3, 0, 0, 14, 0, &a);
  make_decimal(5, 0, 0, 15, 0, &b);
  int code = s21_mul(a, b, &res);
  ck_assert_int_eq(code, OK);
  ck_assert_uint_eq((uint32_t)res.bits[0], 2u);
  ck_assert_int_eq(get_scale(res), 28);
}
END_TEST

START_TEST(mul_overflow_no_scale) {
  s21_decimal a, b, res;
  make_decimal(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0, &a);
  make_decimal(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0, &b);
  int code = s21_mul(a, b, &res);
  ck_assert_int_eq(code, NUMBER_IS_TOO_BIG);
}
END_TEST

START_TEST(mul_invalid_null_result) {
  s21_decimal a, b;
  make_decimal(1, 0, 0, 0, 0, &a);
  make_decimal(1, 0, 0, 0, 0, &b);
  ck_assert_int_eq(s21_mul(a, b, NULL), NUMBER_IS_TOO_BIG);
}
END_TEST

START_TEST(add1_no_carry_out) {
  unsigned int w[6] = {0, 0, 0, 0, 0, 0};
  int r = add1_to_192(w);
  ck_assert_int_eq(r, 0);
  ck_assert_uint_eq(w[0], 1u);
  for (int i = 1; i < 6; ++i) ck_assert_uint_eq(w[i], 0u);
}
END_TEST

START_TEST(add1_propagate_partial) {
  unsigned int w[6] = {0xFFFFFFFFu, 0xFFFFFFFFu, 5u, 0, 0, 0};
  int r = add1_to_192(w);
  ck_assert_int_eq(r, 0);
  ck_assert_uint_eq(w[0], 0u);
  ck_assert_uint_eq(w[1], 0u);
  ck_assert_uint_eq(w[2], 6u);
}

START_TEST(add1_full_overflow) {
  unsigned int w[6] = {0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
                       0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu};
  int r = add1_to_192(w);
  ck_assert_int_eq(r, 1);
  for (int i = 0; i < 6; ++i) ck_assert_uint_eq(w[i], 0u);
}
END_TEST

// 1) scale_1 > scale_2: успешное масштабирование value_2 (else-ветка, строки
// 145–151)
START_TEST(align_scale_else_ok) {
  s21_decimal a, b;

  // a со scale=2, b со scale=0 → зайдём в else, увеличивая scale у b
  make_decimal(123, 0, 0, 2, 0, &a);  // scale_1 = 2
  make_decimal(45, 0, 0, 0, 0, &b);   // scale_2 = 0

  int st = s21_align_scale(&a, &b);
  ck_assert_int_eq(st, OK);

  // Скейлы выровнены
  ck_assert_int_eq(get_scale(a), get_scale(b));
  ck_assert_int_eq(get_scale(a), 2);

  // b умножался на 10 дважды: 45 -> 450 -> 4500
  ck_assert_uint_eq((uint32_t)b.bits[0], 4500u);
  ck_assert_uint_eq((uint32_t)b.bits[1], 0u);
  ck_assert_uint_eq((uint32_t)b.bits[2], 0u);

  // Знак/масштаб у a не изменились помимо scale
  ck_assert_int_eq(get_sign(a), 0);
  ck_assert_int_eq(get_sign(b), 0);
}
END_TEST

// 2) scale_1 > scale_2: переполнение при s21_mul_10(&value_2) (else-ветка,
// строки 145–147)
START_TEST(align_scale_else_overflow) {
  s21_decimal a, b;

  // a c большим scale
  make_decimal(1, 0, 0, 2, 0, &a);  // scale_1 = 2

  // b с максимальной мантиссой (умножение на 10 должно вернуть ошибку)
  make_decimal(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0, &b);  // scale_2 = 0

  int st = s21_align_scale(&a, &b);
  // Ожидаем любой не-OK код (переполнение при попытке поднять scale меньшего)
  ck_assert(st != OK);

  // b должен остаться неизменным по scale (попытка масштабирования не удалась)
  ck_assert_int_eq(get_scale(b), 0);
  // a по-прежнему со scale=2
  ck_assert_int_eq(get_scale(a), 2);
}
END_TEST

Suite *test_mul(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("=s21_mul=");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_mul_ok);
  tcase_add_test(tc_core, test_mul_zero_left);
  tcase_add_test(tc_core, test_mul_zero_right);
  tcase_add_test(tc_core, test_mul_one);
  tcase_add_test(tc_core, test_mul_sign_pos_neg);
  tcase_add_test(tc_core, test_mul_sign_neg_neg);
  tcase_add_test(tc_core, test_mul_scale);
  tcase_add_test(tc_core, test_mul_scale_reduce);
  tcase_add_test(tc_core, test_mul_too_big);
  tcase_add_test(tc_core, test_mul_too_small);
  tcase_add_test(tc_core, test_mul_invalid_decimal);
  tcase_add_test(tc_core, test_mul_null_result);
  tcase_add_test(tc_core, test_mul_div_by_zero_code);
  tcase_add_test(tc_core, test_mul_bankers_rounding);

  tcase_add_test(tc_core, mul_round_last_gt5);
  tcase_add_test(tc_core, mul_round_last5_tail);
  tcase_add_test(tc_core, mul_round_last5_odd);
  tcase_add_test(tc_core, mul_round_add1_overflow);
  tcase_add_test(tc_core, mul_round_last5_with_tail);
  tcase_add_test(tc_core, mul_overflow_no_scale);
  tcase_add_test(tc_core, mul_invalid_null_result);

  tcase_add_test(tc_core, add1_no_carry_out);
  tcase_add_test(tc_core, add1_propagate_partial);
  tcase_add_test(tc_core, add1_full_overflow);
  tcase_add_test(tc_core, align_scale_else_ok);
  tcase_add_test(tc_core, align_scale_else_overflow);

  suite_add_tcase(s, tc_core);
  return s;
}