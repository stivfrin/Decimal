#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  DecimalNull(dst);
  if (src < 0) {
    src *= -1;
    SetSign(dst, 1);
  }
  dst->bits[0] = src;
  return OK;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int status = OK;
  long long int bits = 0;
  if (GetBit(&src, 31) && GetBit(&src, 0) && src.bits[0] == 2147483649 &&
      src.bits[3] == 0) {
    src.bits[0] -= 2;
    SetBit(&src, 127, 1);
  }
  for (int i = 95; i >= 0; i--)
    if (GetBit(&src, i)) {
      bits = i;
      i = 0;
    }
  int power = GetExp(src);
  for (int i = 0; i < power; i++) bits -= 4;

  if (bits > 32 || power > 28)
    status = CONVERTATION_ERROR;
  else {
    double number = 0;
    for (int i = 0; i <= (4 * power) + bits; i++)
      number += pow(2, i) * GetBit(&src, i);
    number /= pow(10, power);
    if (GetBit(&src, 127)) number *= -1;
    *dst = number;
  }

  return status;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  DecimalNull(dst);
  int status = 0;
  if (isinf(src) || isnan(src)) {
    status = 1;
  } else {
    if (src != 0) {
      int exp = 0, power = 0;
      float copy = 0;

      int sign = (*(int *)&src & (1 << 31)) >> 31;
      double copy_src = (double)fabs(src);
      for (; power < 28 && (int)copy_src / (int)pow(2, 21) == 0;
           copy_src *= 10, power++) {
      }
      exp = FindFloatExp(src);

      SetSign(dst, sign);
      copy_src = (float)round(copy_src);
      for (; fmod(copy_src, 10) == 0 && power > 0; power--, copy_src /= 10) {
      }
      copy = copy_src;
      exp = FindFloatExp(copy);
      SetBit(dst, exp, 1);
      for (int i = exp - 1, j = 22; j >= 0; i--, j--)
        if ((*(int *)&copy & (1 << j)) != 0) SetBit(dst, i, 1);
      SetPow(dst, power);
    }
  }
  return status;
}

int FindFloatExp(float count) {
  int n = 7, exp = 0;
  for (int i = 30; i >= 23; i--) {
    exp += pow(2, n) * ((*(int *)&count & (1 << i)) >> i);
    n--;
  }
  exp -= 127;
  return exp;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int status = OK;
  long long int bits = 0;
  for (int i = 95; i >= 0; i--)
    if (GetBit(&src, i)) {
      bits = i;
      i = 0;
    }
  int power = GetExp(src);

  double number = 0;
  for (int i = 0; i <= bits; i++) number += pow(2, i) * GetBit(&src, i);
  number = number / pow(10, power);
  if (GetBit(&src, 127)) number *= -1;
  *dst = number;
  return status;
}