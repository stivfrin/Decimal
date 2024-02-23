#include "s21_decimal.h"

void BDecimalNull(s21_big_decimal* value) {
  for (int i = 0; i < 8; i++) {
    value->bits[i] = 0;
  }
}

int CheckIsZero(s21_decimal value) {
  int res =
      (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0) ? 1 : 0;

  return res;
}

int CheckSign(s21_decimal value1) { return (value1.bits[3] & (1 << 31)) >> 31; }

void DecimalNull(s21_decimal* count) {
  for (int i = 0; i < 4; i++) count->bits[i] = 0;
}

int GetBit(s21_decimal* count, int index) {
  int num_bit = index / 32;
  int pos = index % 32;
  return (count->bits[num_bit] & (1 << pos)) >> pos;
}

int GetExp(s21_decimal bit) {
  int power = 0, n = 0;
  for (int i = 16; i <= 23; i++) {
    power += pow(2, n) * GetBit(&bit, i + 96);
    n++;
  }
  return power;
}

void SetBit(s21_decimal* count, int index, int bit) {
  int num_bit = index / 32;
  int pos = index % 32;
  if (bit)
    count->bits[num_bit] |= (1 << pos);
  else
    count->bits[num_bit] &= ~(1 << pos);
}

void SetPow(s21_decimal* count, int pow) {
  for (int i = 15; i <= 23; i++) {
    SetBit(count, i + 97, pow & (1 << (i - 15)));
  }
}

void SetSign(s21_decimal* count, int sign) {
  if (sign)
    count->bits[3] |= (1 << 31);
  else
    count->bits[3] &= ~(1 << 31);
}

void ShiftDecimalLeft(s21_decimal* count) {
  s21_decimal copy = {0};
  for (int i = 1; i <= 96; i++)
    if (GetBit(count, i - 1)) SetBit(&copy, i, 1);
  count->bits[0] = copy.bits[0];
  count->bits[1] = copy.bits[1];
  count->bits[2] = copy.bits[2];
}

void ShiftDecimalRight(s21_decimal* count) {
  s21_decimal copy = {0};
  for (int i = 0; i <= 96; i++)
    if (GetBit(count, i + 1)) SetBit(&copy, i, 1);
  count->bits[0] = copy.bits[0];
  count->bits[1] = copy.bits[1];
  count->bits[2] = copy.bits[2];
}

void TransfToBig(s21_big_decimal* bvalue1, s21_decimal value1) {
  for (int t = 0; t < 3; t++) {
    bvalue1->bits[t] = value1.bits[t];
  }
  bvalue1->bits[7] = value1.bits[3];
}

void DivisionBy10(s21_decimal* count) {
  int exp = -1;
  int pow_count = GetExp(*count);
  int sign = GetBit(count, 127);
  s21_decimal sum = {0};
  s21_decimal ten = {0};
  s21_from_int_to_decimal(10, &ten);
  if (sign) SetBit(count, 127, 0);
  SetPow(count, 0);
  while (s21_is_greater_or_equal(*count, ten)) {
    s21_from_int_to_decimal(10, &ten);
    do {
      if (exp != -1) ShiftDecimalLeft(&ten);
      exp++;
    } while (s21_is_less_or_equal(ten, *count) && exp <= 91);
    s21_decimal slag = {0};
    SetBit(&slag, exp - 1, 1);
    s21_add(slag, sum, &sum);
    ShiftDecimalRight(&ten);
    s21_sub(*count, ten, count);
    exp = -1;
    s21_from_int_to_decimal(10, &ten);
  }
  if (sign) SetBit(&sum, 127, 1);
  if (pow_count != 0) SetPow(&sum, pow_count - 1);
  count->bits[0] = sum.bits[0];
  count->bits[1] = sum.bits[1];
  count->bits[2] = sum.bits[2];
  count->bits[3] = sum.bits[3];
}

int DecMulTen(s21_decimal value_1, s21_decimal* res) {
  int arr2[100] = {0}, arr4[100] = {0};
  int error = 0;
  int power = GetExp(value_1);
  int sign = GetBit(&value_1, 127);
  DecimalNull(res);
  for (int j = 3; j <= 99; j++) {
    arr2[j - 2] = GetBit(&value_1, j - 3);
    arr4[j] = GetBit(&value_1, j - 3);
  }
  int occupy = 0;
  for (int j = 1; j <= 95; j++) {
    if (arr2[j] + arr4[j] + occupy == 1) {
      SetBit(res, j, 1);
      occupy = 0;
    } else if (arr2[j] + arr4[j] + occupy == 2)
      occupy = 1;
    else if (arr2[j] + arr4[j] + occupy == 3) {
      SetBit(res, j, 1);
      occupy = 1;
    } else
      occupy = 0;
  }
  SetSign(res, sign);
  SetPow(res, power + 1);
  if (occupy && GetBit(&value_1, 127))
    error = 2;
  else if (occupy)
    error = 1;
  return error;
}

int Normalize(s21_decimal* value_1, s21_decimal* value_2) {
  int power_1 = GetExp(*value_1);
  int power_2 = GetExp(*value_2);
  s21_decimal copy_1 = *value_1;
  s21_decimal copy_2 = *value_2;
  int error = 0;
  while (power_1 != power_2 && error == 0) {
    copy_1 = *value_1;
    copy_2 = *value_2;
    if (power_1 > power_2 && power_2 < 28) {
      error = DecMulTen(*value_2, value_2);
      power_2++;
    } else if (power_1 < power_2 && power_1 < 28) {
      error = DecMulTen(*value_1, value_1);
      power_1++;
    } else if (power_2 > 28) {
      DivisionBy10(value_2);
      power_2--;
    } else if (power_1 > 28) {
      DivisionBy10(value_1);
      power_1--;
    }
  }
  if (error && power_1 > 3 && power_2 > 3) {
    *value_1 = copy_1;
    *value_2 = copy_2;
    power_1 = GetExp(*value_1);
    power_2 = GetExp(*value_2);
    while (power_1 != power_2) {
      if (power_1 < power_2) {
        DivisionBy10(value_2);
        power_2--;
      } else {
        DivisionBy10(value_1);
        power_1--;
      }
    }
    error = 0;
  }
  return error;
}
