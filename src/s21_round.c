#include <stdint.h>

#include "s21_decimal.h"
#include "s21_helper_functions.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  int status = OK;

  if (!result) {
    status = CALCULATION_ERROR;
    return status;
  }

  s21_set_zero(result);

  const int sign = get_sign(value);
  const int scale = get_scale(value);

  if (scale == 0) {
    *result = value;

  } else {
    s21_decimal abs_value = value;
    set_sign(&abs_value, 0);

    if ((uint32_t)abs_value.bits[0] == UINT32_MAX &&
        (uint32_t)abs_value.bits[1] == UINT32_MAX &&
        (uint32_t)abs_value.bits[2] == UINT32_MAX && scale > 0) {
      status = sign ? NUMBER_IS_TOO_SMALL : NUMBER_IS_TOO_BIG;
    } else {
      s21_decimal quotient = abs_value;
      int remainder = 0;
      int dummy_scale = scale;
      int has_nonzero_tail = 0;

      for (int i = 0; i < scale; i++) {
        int current_rem = s21_div_by_10(&quotient, &dummy_scale);
        if (i < scale - 1 && current_rem != 0) {
          has_nonzero_tail = 1;
        }
        remainder = current_rem;
      }

      int need_increment =
          (remainder > 5) ||
          (remainder == 5 && (has_nonzero_tail || (quotient.bits[0] & 1)));

      if (need_increment) {
        unsigned int carry = 1;
        for (int i = 0; i < 3 && carry; i++) {
          unsigned long long sum =
              (unsigned long long)(uint32_t)quotient.bits[i] + carry;
          quotient.bits[i] = (unsigned int)(sum & 0xFFFFFFFFu);
          carry = (unsigned int)(sum >> 32);
        }
        if (carry) {
          status = sign ? NUMBER_IS_TOO_SMALL : NUMBER_IS_TOO_BIG;
        }
      }

      if (status == OK) {
        *result = quotient;
        if (sign) set_sign(result, 1);
        set_scale(result, 0);
      }
    }
  }

  return status;
}