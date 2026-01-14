#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"
#include "s21_helper_functions.h"

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int status = OK;

  if (!dst) {
    status = CONVERTATION_ERROR;
  } else {
    s21_set_zero(dst);

    if (isnan(src) || isinf(src)) {
      status = CONVERTATION_ERROR;
    } else {
      if (fabsf(src) < 1e-28f) {
        s21_set_zero(dst);
      } else {
        int sign = 0;
        if (src < 0) {
          sign = 1;
          src = -src;
        }

        int scale = 0;
        double value = src;

        char buffer[32];
        sprintf(buffer, "%.7g", value);

        char *end;
        double rounded_value = strtod(buffer, &end);
        value = rounded_value;

        while (scale < 28 && floor(value) != value) {
          value *= 10.0;
          scale++;
        }

        value = round(value);

        if (value > 79228162514264337593543950335.0) {
          status = CONVERTATION_ERROR;
        } else {
          unsigned long ull_value = (unsigned long)value;
          dst->bits[0] = (int)(ull_value & 0xFFFFFFFF);
          dst->bits[1] = (int)((ull_value >> 32) & 0xFFFFFFFF);
          dst->bits[2] = 0;

          set_scale(dst, scale);
          set_sign(dst, sign);
        }
      }
    }
  }

  return status;
}