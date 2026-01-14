#include <stdint.h>
#include <stdio.h>

#include "s21_decimal.h"
#include "s21_helper_functions.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (!s21_is_decimal(value_1) || !s21_is_decimal(value_2) || result == NULL) {
    if (result) s21_set_zero(result);
    return NUMBER_IS_TOO_BIG;
  }

  int status = OK;
  unsigned int buf[6] = {0};
  unsigned sign_res = get_sign(value_1) ^ get_sign(value_2);

  mul_96x96_to_192(&value_1, &value_2, buf);

  if (!(buf[0] | buf[1] | buf[2] | buf[3] | buf[4] | buf[5])) {
    s21_set_zero(result);
    return OK;
  }

  unsigned scale = get_scale(value_1) + get_scale(value_2);
  unsigned last = 0;
  int has_nonzero_tail = 0;

  while ((scale > 28 || !fits_96(buf)) && scale > 0) {
    unsigned rem = div_192_by_10(buf);
    if (rem != 0 && scale > 1) has_nonzero_tail = 1;
    last = rem;
    --scale;
  }

  if (!fits_96(buf)) {
    status = (sign_res == 0) ? NUMBER_IS_TOO_BIG : NUMBER_IS_TOO_SMALL;
  } else {
    int need_round =
        (last > 5) || (last == 5 && (has_nonzero_tail || (buf[0] & 1u)));
    if (need_round) {
      add1_to_192(buf);
    }
  }

  if (status == OK) {
    result->bits[0] = buf[0];
    result->bits[1] = buf[1];
    result->bits[2] = buf[2];
    result->bits[3] = 0;
    set_scale(result, scale);
    set_sign(result, sign_res);
  } else {
    s21_set_zero(result);
  }

  return status;
}