#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  *result = value;
  result->bits[3] = value.bits[3] ^ 1 << 31;
  return 0;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  int err_num = OK;
  int sign = CheckSign(value);
  s21_decimal one = {0};
  s21_from_int_to_decimal(1, &one);
  s21_truncate(value, result);
  if (sign == 1 && !s21_is_equal(value, *result)) {
    s21_sub(*result, one, result);
  }
  SetSign(result, sign);
  return err_num;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int power = GetExp(value);
  int sign = GetBit(&value, 127);
  float remain_fl = 0;
  s21_decimal one = {0};
  s21_from_int_to_decimal(1, &one);
  while (power > 1) {
    DivisionBy10(&value);
    power--;
  }
  if (power != 0) {
    s21_decimal copy = value;
    DivisionBy10(&value);
    s21_decimal remain = {0};
    s21_sub(copy, value, &remain);
    s21_from_decimal_to_float(remain, &remain_fl);
    if (fabs(remain_fl) > 0.5) {
      if (sign)
        s21_sub(value, one, &value);
      else
        s21_add(value, one, &value);
    } else if (fabsl(remain_fl) == 0.5) {
      if (GetBit(&value, 0) == 1) {
        if (sign)
          s21_sub(value, one, &value);
        else
          s21_add(value, one, &value);
      }
    }
  }
  for (int i = 0; i < 4; i++) result->bits[i] = value.bits[i];
  return OK;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  DecimalNull(result);
  int res = 0;
  int scale = GetExp(value);
  int sign = CheckSign(value);
  *result = value;
  if (scale) {
    for (int t = scale; t > 0; t--) {
      DivisionBy10(result);
    }
    float dec = 0.0;
    s21_from_decimal_to_float(*result, &dec);
    SetPow(result, 0);
    SetSign(result, sign);
  }
  return res;
}