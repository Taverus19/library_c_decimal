#include <string.h>

#include "s21_decimal.h"
#include "s21_helper_functions.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  if (!result || !s21_is_decimal(value)) {
    return 1;
  }

  memset(result, 0, sizeof(s21_decimal));

  int sign = get_sign(value);
  int scale = get_scale(value);

  if (scale == 0) {
    *result = value;
    return 0;
  }

  s21_decimal truncated = value;
  int has_fraction = 0;

  for (int i = 0; i < scale; i++) {
    unsigned int remainder = 0;
    floor_divide_by_10(&truncated, &remainder);
    if (remainder != 0) {
      has_fraction = 1;
    }
  }
  set_scale(&truncated, 0);

  if (sign == 0) {
    *result = truncated;
  } else {
    if (has_fraction) {
      s21_decimal one = {{1, 0, 0, 0}};
      s21_decimal temp = {0};
      add_mantissa(&truncated, &one, result);
      *result = temp;
    } else {
      *result = truncated;
    }
  }

  set_sign(result, sign);

  return 0;
}