#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "s21_decimal.h"

int s21_is_zero(s21_decimal d) {
  return d.bits[0] == 0 && d.bits[1] == 0 && d.bits[2] == 0;
}

int get_sign(s21_decimal d) { return ((unsigned int)d.bits[3] >> 31) & 1; }

int get_scale(s21_decimal d) { return ((unsigned int)d.bits[3] >> 16) & 0xFF; }

unsigned int to_unsigned(int value) { return (unsigned int)value; }

int to_signed(unsigned int value) { return (int)value; }

void set_sign(s21_decimal *d, int sign) {
  unsigned int value = (unsigned int)d->bits[3];
  if (sign) {
    value |= (1U << 31);
  } else {
    value &= ~(1U << 31);
  }
  d->bits[3] = (int)value;
}

void set_scale(s21_decimal *d, int scale) {
  unsigned int value = (unsigned int)d->bits[3];
  int sign = (value >> 31) & 1;
  value = (sign << 31) | ((unsigned int)scale << 16);
  d->bits[3] = (int)value;
}

int s21_div_by_10(s21_decimal *dec, int *scale) {
  uint64_t remainder = 0;
  for (int i = 2; i >= 0; i--) {
    uint64_t temp =
        ((uint64_t)remainder << 32) | (uint64_t)(unsigned int)dec->bits[i];
    dec->bits[i] = (int)(temp / 10);
    remainder = temp % 10;
  }
  (*scale)--;
  return (int)remainder;
}

int s21_mul_10(s21_decimal *d) {
  unsigned int l = (unsigned int)d->bits[0];
  unsigned int m = (unsigned int)d->bits[1];
  unsigned int h = (unsigned int)d->bits[2];
  unsigned long long tmp;
  unsigned int new_l, new_m, new_h;
  unsigned long long carry = 0;
  int result = 0;

  tmp = (unsigned long long)l * 10;
  new_l = (unsigned int)(tmp & 0xFFFFFFFFU);
  carry = tmp >> 32;

  tmp = (unsigned long long)m * 10 + carry;
  new_m = (unsigned int)(tmp & 0xFFFFFFFFU);
  carry = tmp >> 32;

  tmp = (unsigned long long)h * 10 + carry;
  new_h = (unsigned int)(tmp & 0xFFFFFFFFU);
  carry = tmp >> 32;

  if (carry) {
    result = 1;
  } else {
    d->bits[0] = (int)new_l;
    d->bits[1] = (int)new_m;
    d->bits[2] = (int)new_h;
  }
  return result;
}

int s21_mul_10_exponent(s21_decimal *d, int exponent) {
  int old_scale = get_scale(*d);
  int sign = get_sign(*d);
  int overflow = 0;

  for (int i = 0; i < exponent && !overflow; i++) {
    overflow = s21_mul_10(d);
  }

  if (!overflow) {
    set_scale(d, old_scale + exponent);
  }
  set_sign(d, sign);
  return overflow;
}

int s21_compare_bits(s21_decimal a, s21_decimal b) {
  int result = 0;
  int high_compare = ((unsigned int)a.bits[2] > (unsigned int)b.bits[2]) -
                     ((unsigned int)a.bits[2] < (unsigned int)b.bits[2]);
  int mid_compare = ((unsigned int)a.bits[1] > (unsigned int)b.bits[1]) -
                    ((unsigned int)a.bits[1] < (unsigned int)b.bits[1]);
  int low_compare = ((unsigned int)a.bits[0] > (unsigned int)b.bits[0]) -
                    ((unsigned int)a.bits[0] < (unsigned int)b.bits[0]);

  if (high_compare) {
    result = high_compare;
  } else if (mid_compare) {
    result = mid_compare;
  } else {
    result = low_compare;
  }
  return result;
}

int s21_is_decimal(s21_decimal value) {
  int status = TRUE;
  s21_ctrl cw = {.word = value.bits[3]};
  if (cw.reserved_lo != 0 || cw.reserved_hi != 0) status = FALSE;
  if (status && cw.scale > 28) status = FALSE;
  return status;
}

void s21_copy_decimal(s21_decimal value, s21_decimal *result) {
  for (int i = 0; i < 4; i++) {
    result->bits[i] = value.bits[i];
  }
}

int s21_align_scale(s21_decimal *value_1, s21_decimal *value_2) {
  int status = OK;

  unsigned scale_1 = get_scale(*value_1);
  unsigned scale_2 = get_scale(*value_2);

  while (status == OK && scale_1 != scale_2) {
    if (scale_1 < scale_2) {
      s21_decimal tmp = *value_1;
      status = s21_mul_10(&tmp);
      if (status == OK) {
        *value_1 = tmp;
        ++scale_1;
        set_scale(value_1, scale_1);
      }
    } else {
      s21_decimal tmp = *value_2;
      status = s21_mul_10(&tmp);
      if (status == OK) {
        *value_2 = tmp;
        ++scale_2;
        set_scale(value_2, scale_2);
      }
    }
  }
  return status;
}

void s21_set_zero(s21_decimal *value) {
  value->bits[0] = value->bits[1] = value->bits[2] = value->bits[3] = 0;
}

void mul_96x96_to_192(const s21_decimal *a, const s21_decimal *b,
                      unsigned int res[6]) {
  memset(res, 0, 6 * sizeof(unsigned int));

  for (int i = 0; i < 3; ++i) {
    unsigned long long carry = 0;
    for (int j = 0; j < 3; ++j) {
      unsigned long long tmp =
          (unsigned long long)a->bits[i] * (unsigned long long)b->bits[j] +
          res[i + j] + carry;
      res[i + j] = (unsigned int)tmp;
      carry = tmp >> 32;
    }
    res[i + 3] += (unsigned int)carry;
  }
}

unsigned div_192_by_10(unsigned int w[6]) {
  unsigned long long rem = 0;
  for (int i = 5; i >= 0; --i) {
    unsigned long long cur = (rem << 32) | w[i];
    w[i] = (unsigned int)(cur / 10u);
    rem = cur % 10u;
  }
  return (unsigned)rem;
}

int fits_96(unsigned int words[6]) {
  return (words[3] == 0 && words[4] == 0 && words[5] == 0);
}

int add1_to_192(unsigned int words[6]) {
  for (int i = 0; i < 6; ++i) {
    if (++words[i] != 0) return 0;
  }
  return 1;
}

int add_mantissa(s21_decimal *value_1, s21_decimal *value_2,
                 s21_decimal *result) {
  unsigned long carry = 0;

  for (int i = 0; i < 3; i++) {
    unsigned long sum = (unsigned long)to_unsigned(value_1->bits[i]) +
                        (unsigned long)to_unsigned(value_2->bits[i]) + carry;
    result->bits[i] = to_signed((unsigned int)(sum & 0xFFFFFFFF));
    carry = sum >> 32;
  }

  return (carry != 0) ? NUMBER_IS_TOO_BIG : OK;
}

int compare_mantissa(const s21_decimal *value_1, const s21_decimal *value_2) {
  for (int i = 2; i >= 0; i--) {
    unsigned int uvalue_1 = to_unsigned(value_1->bits[i]);
    unsigned int uvalue_2 = to_unsigned(value_2->bits[i]);

    if (uvalue_1 > uvalue_2) return 1;
    if (uvalue_1 < uvalue_2) return -1;
  }
  return 0;
}

int subtract_mantissa(const s21_decimal *value_1, const s21_decimal *value_2,
                      s21_decimal *result) {
  unsigned long borrow = 0;

  for (int i = 0; i < 3; i++) {
    unsigned long uvalue_1 = (unsigned long)to_unsigned(value_1->bits[i]);
    unsigned long uvalue_2 = (unsigned long)to_unsigned(value_2->bits[i]);

    if (uvalue_1 >= uvalue_2 + borrow) {
      result->bits[i] = to_signed((unsigned int)(uvalue_1 - uvalue_2 - borrow));
      borrow = 0;
    } else {
      result->bits[i] = to_signed(
          (unsigned int)(uvalue_1 + 0x100000000ULL - uvalue_2 - borrow));
      borrow = 1;
    }
  }

  return (borrow != 0) ? 1 : 0;
}

int floor_divide_by_10(s21_decimal *value, unsigned int *remainder) {
  unsigned long dividend = 0;
  *remainder = 0;

  for (int i = 2; i >= 0; i--) {
    dividend =
        ((unsigned long)(*remainder) << 32) | to_unsigned(value->bits[i]);
    value->bits[i] = to_signed((unsigned int)(dividend / 10));
    *remainder = (unsigned int)(dividend % 10);
  }

  return (*remainder != 0);
}

static inline int count_leading_zeros(uint32_t x) {
  int count = 32;
  for (uint32_t i = 1 << 31; i > 0 && count == 32; i >>= 1) {
    if (x & i) count = 0;
  }
  if (count != 32) {
    for (uint32_t i = 1 << (31 - count); i > 0; i >>= 1) {
      if (!(x & i))
        count++;
      else
        break;
    }
  }
  return count;
}

int s21_compare_abs(s21_decimal a, s21_decimal b) {
  int result = 0;
  for (int i = 2; i >= 0 && !result; i--) {
    uint32_t a_val = (uint32_t)a.bits[i];
    uint32_t b_val = (uint32_t)b.bits[i];
    if (a_val != b_val) {
      result = (a_val > b_val) ? 1 : -1;
    }
  }
  return result;
}

int s21_shift_left(s21_decimal *d, int n) {
  if (!d || n < 0) return 1;
  for (int i = 0; i < n; i++) {
    uint32_t carry = 0;
    for (int j = 0; j < 3; j++) {
      uint32_t current = (uint32_t)d->bits[j];
      uint32_t new_carry = current >> (32 - 1);
      current = (current << 1) | carry;
      d->bits[j] = (int)current;
      carry = new_carry;
    }
    if (carry) return 1;
  }
  return 0;
}

int s21_shift_right(s21_decimal *d, int n) {
  if (!d || n < 0) return 1;
  for (int i = 0; i < n; i++) {
    uint32_t carry = 0;
    for (int j = 2; j >= 0; j--) {
      uint32_t current = (uint32_t)d->bits[j];
      uint32_t new_carry = (current & 1) << (32 - 1);
      current = (current >> 1) | carry;
      d->bits[j] = (int)current;
      carry = new_carry;
    }
  }
  return 0;
}

int s21_add_abs(s21_decimal a, s21_decimal b, s21_decimal *result) {
  if (!result) return 1;
  uint32_t carry = 0;
  for (int i = 0; i < 3; i++) {
    uint64_t sum = (uint64_t)((uint32_t)a.bits[i]) +
                   (uint64_t)((uint32_t)b.bits[i]) + carry;
    result->bits[i] = (int)(sum & 0xFFFFFFFFU);
    carry = (uint32_t)(sum >> 32);
  }
  return carry ? 1 : 0;
}

int s21_sub_abs(s21_decimal a, s21_decimal b, s21_decimal *result) {
  if (!result || s21_compare_abs(a, b) < 0) return 1;
  uint32_t borrow = 0;
  for (int i = 0; i < 3; i++) {
    uint64_t diff = (uint64_t)((uint32_t)a.bits[i]) -
                    (uint64_t)((uint32_t)b.bits[i]) - borrow;
    result->bits[i] = (int)(diff & 0xFFFFFFFFU);
    borrow = (diff >> 32) ? 1 : 0;
  }
  return 0;
}

int s21_mul_abs(s21_decimal a, s21_decimal b, s21_decimal *result) {
  if (!result) return 1;
  s21_decimal temp = {0};
  int error = 0;
  for (int i = 0; i < 3 && !error; i++) {
    uint32_t multiplier = (uint32_t)b.bits[i];
    for (int j = 0; j < 32 && !error; j++) {
      if (multiplier & 1) {
        s21_decimal add = a;
        if (s21_shift_left(&add, j)) error = 1;
        if (!error && s21_add_abs(temp, add, &temp)) error = 1;
      }
      multiplier >>= 1;
    }
  }
  *result = temp;
  return error;
}

int s21_div_abs(s21_decimal a, s21_decimal b, s21_decimal *quotient,
                s21_decimal *remainder) {
  if (!quotient || s21_is_zero(b)) return 1;
  s21_decimal q = {0};
  s21_decimal r = a;
  s21_decimal one = {{1, 0, 0, 0}};
  int error = 0;
  if (s21_compare_abs(a, b) >= 0) {
    int bits_a = 0, bits_b = 0;
    for (int i = 2; i >= 0; i--) {
      if (a.bits[i] && !bits_a)
        bits_a = i * 32 + 32 - count_leading_zeros((uint32_t)a.bits[i]);
      if (b.bits[i] && !bits_b)
        bits_b = i * 32 + 32 - count_leading_zeros((uint32_t)b.bits[i]);
    }
    int shift = bits_a - bits_b;
    s21_decimal temp = b;
    if (s21_shift_left(&temp, shift)) error = 1;
    s21_decimal multiple = one;
    if (!error && s21_shift_left(&multiple, shift)) error = 1;
    while (shift >= 0 && !error) {
      if (s21_compare_abs(r, temp) >= 0) {
        if (s21_sub_abs(r, temp, &r)) error = 1;
        if (!error && s21_add_abs(q, multiple, &q)) error = 1;
      }
      if (!error) {
        s21_shift_right(&temp, 1);
        s21_shift_right(&multiple, 1);
        shift--;
      }
    }
  }
  *quotient = q;
  if (remainder) *remainder = r;
  return error;
}

int s21_bank_rounding(s21_decimal *d, int digits) {
  if (!d || digits <= 0) return 1;
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal remainder = {0};
  int original_scale = get_scale(*d);
  int error = 0;
  for (int i = 0; i < digits && !error; i++) {
    if (s21_div_abs(*d, ten, d, &remainder)) error = 1;
  }
  if (!error) {
    int last_digit = remainder.bits[0];
    if (last_digit > 5 || (last_digit == 5 && (d->bits[0] & 1))) {
      s21_decimal one = {{1, 0, 0, 0}};
      s21_decimal new_val;
      if (s21_add_abs(*d, one, &new_val))
        error = 1;
      else
        *d = new_val;
    }
    int new_scale = original_scale - digits;
    if (new_scale < 0) new_scale = 0;
    set_scale(d, new_scale);
  }
  return error;
}

int process_fractional_digit(s21_decimal *res, s21_decimal *remainder,
                             s21_decimal b, s21_decimal ten, int sign,
                             int *current_scale) {
  s21_decimal rem10 = {0}, digit = {0}, new_rem = {0};
  s21_decimal res10 = {0}, new_res = {0};
  int error = 0;

  if (s21_mul_abs(*remainder, ten, &rem10)) error = sign ? 2 : 1;

  if (!error && s21_div_abs(rem10, b, &digit, &new_rem)) error = sign ? 2 : 1;

  if (!error && s21_mul_abs(*res, ten, &res10)) error = sign ? 2 : 1;

  if (!error && s21_add_abs(res10, digit, &new_res)) error = sign ? 2 : 1;

  if (!error) {
    *res = new_res;
    *remainder = new_rem;
    (*current_scale)++;
  }
  return error;
}

int adjust_scale_and_sign(s21_decimal *res, int scale1, int scale2,
                          int current_scale, int sign) {
  s21_decimal ten = {{10, 0, 0, 0}};
  int total_scale = scale1 - scale2 + current_scale;
  int error = 0;
  if (total_scale < 0) {
    int mul_times = -total_scale;
    total_scale = 0;
    while (mul_times-- && !error) {
      s21_decimal temp;
      if (s21_mul_abs(*res, ten, &temp))
        error = sign ? 2 : 1;
      else
        *res = temp;
    }
  }
  if (!error && total_scale > 28) {
    int digits = total_scale - 28;
    if (s21_bank_rounding(res, digits))
      error = sign ? 2 : 1;
    else
      total_scale = 28;
  }
  if (!error) {
    if (s21_is_zero(*res)) {
      *res = (s21_decimal){0};
    } else {
      set_scale(res, total_scale);
      set_sign(res, sign);
    }
  }
  return error;
}
