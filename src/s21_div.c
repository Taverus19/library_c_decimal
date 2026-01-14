#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "s21_decimal.h"
#include "s21_helper_functions.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (!result) return 1;
  *result = (s21_decimal){0};
  int error = 0;

  if (s21_is_zero(value_2)) {
    error = 3;
  } else {
    int sign = get_sign(value_1) ^ get_sign(value_2);
    int scale1 = get_scale(value_1);
    int scale2 = get_scale(value_2);

    s21_decimal a = value_1, b = value_2;
    set_sign(&a, 0);
    set_sign(&b, 0);

    s21_decimal quotient = {0}, remainder = a;
    if (s21_div_abs(a, b, &quotient, &remainder)) {
      error = sign ? 2 : 1;
    } else {
      s21_decimal res = quotient;
      int current_scale = 0;
      s21_decimal ten = {{10, 0, 0, 0}};

      while (!s21_is_zero(remainder) && current_scale < 29 && !error) {
        error = process_fractional_digit(&res, &remainder, b, ten, sign,
                                         &current_scale);
      }

      if (!error) {
        error =
            adjust_scale_and_sign(&res, scale1, scale2, current_scale, sign);
      }

      if (!error) {
        *result = res;
      }
    }
  }
  return error;
}