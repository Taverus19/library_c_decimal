#include "s21_decimal.h"
#include "s21_helper_functions.h"

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (!result) {
    return CALCULATION_ERROR;
  }

  int status = OK;
  s21_set_zero(result);

  int scale1 = get_scale(value_1);
  int scale2 = get_scale(value_2);
  int max_scale = (scale1 > scale2) ? scale1 : scale2;

  s21_decimal a = value_1;
  s21_decimal b = value_2;

  while (scale1 < max_scale && status == OK) {
    if (s21_mul_10(&a)) {
      status = CALCULATION_ERROR;
    } else {
      scale1++;
    }
  }
  while (scale2 < max_scale && status == OK) {
    if (s21_mul_10(&b)) {
      status = CALCULATION_ERROR;
    } else {
      scale2++;
    }
  }

  if (status == OK) {
    set_scale(&a, max_scale);
    set_scale(&b, max_scale);

    int sign1 = get_sign(a);
    int sign2 = get_sign(b);

    if (sign1 != sign2) {
      set_sign(&b, sign1);
      int st = s21_add_abs(a, b, result);
      if (st != OK) {
        status = sign1 ? NUMBER_IS_TOO_SMALL : NUMBER_IS_TOO_BIG;
      }
    } else {
      int cmp = s21_compare_bits(a, b);
      if (cmp < 0) {
        s21_decimal temp = a;
        a = b;
        b = temp;
        sign1 = !sign1;
      } else if (cmp == 0) {
        s21_set_zero(result);
        status = OK;
      }

      if (cmp != 0 && status == OK) {
        int st = s21_sub_abs(a, b, result);
        if (st != OK) {
          status = sign1 ? NUMBER_IS_TOO_SMALL : NUMBER_IS_TOO_BIG;
        } else {
          set_scale(result, max_scale);
          set_sign(result, sign1);
        }
      }
    }
  }

  return status;
}