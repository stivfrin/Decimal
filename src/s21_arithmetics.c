#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = 0;
  DecimalNull(result);
  int error = Normalize(&value_1, &value_2);
  if (!error) {
    int pow_val = GetExp(value_2);
    SetPow(result, pow_val);
    if (CheckSign(value_1) && CheckSign(value_2)) {
      SetSign(result, 1);
      status = Adding(value_1, value_2, result);
      if (status) status++;
    } else if (CheckSign(value_1) != CheckSign(value_2)) {
      if (CheckSign(value_1)) {
        SetSign(&value_1, 0);
        s21_sub(value_2, value_1, result);
      } else {
        SetSign(&value_2, 0);
        s21_sub(value_1, value_2, result);
      }
    } else
      status = Adding(value_1, value_2, result);
  } else
    status = error;
  return status;
}

int Adding(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int occupy = 0;
  for (int i = 0; i < 96; i++) {
    int bit1 = GetBit(&value_1, i);
    int bit2 = GetBit(&value_2, i);
    if (bit1 + bit2 + occupy == 1) {
      SetBit(result, i, 1);
      occupy = 0;
    } else if (bit1 + bit2 + occupy == 2)
      occupy = 1;
    else if (bit1 + bit2 + occupy == 3) {
      SetBit(result, i, 1);
      occupy = 1;
    } else
      occupy = 0;
  }
  return occupy;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = OK;
  DecimalNull(result);
  int error = Normalize(&value_1, &value_2);
  int sign1 = GetBit(&value_1, 127);
  int sign2 = GetBit(&value_2, 127);
  if (!error) {
    int pow_val = GetExp(value_1);
    SetPow(result, pow_val);
    if (sign1 && sign2) {
      if (s21_is_less(value_1, value_2)) {
        Substruction(value_1, value_2, result);
        SetSign(result, 1);
      } else
        Substruction(value_2, value_1, result);
    } else if (sign1 && !sign2) {
      SetSign(result, 1);
      SetSign(&value_1, 0);
      status = Adding(value_1, value_2, result);
      if (status) status++;
    } else if (!sign1 && sign2) {
      SetSign(&value_2, 0);
      status = Adding(value_1, value_2, result);
    } else {
      if (s21_is_less(value_1, value_2)) {
        Substruction(value_2, value_1, result);
        SetSign(result, 1);
      } else
        Substruction(value_1, value_2, result);
    }
  } else
    status = error;

  return status;
}

int Substruction(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal *result) {
  int occupy = 0;
  for (int i = 0; i <= 96; i++) {
    int bit1 = GetBit(&value_1, i);
    int bit2 = GetBit(&value_2, i);
    if (bit1 - bit2 - occupy == 1) {
      SetBit(result, i, 1);
      occupy = 0;
    } else if (bit1 - bit2 - occupy == 0)
      occupy = 0;
    else if (bit1 - bit2 - occupy == -1) {
      occupy = 1;
      SetBit(result, i, 1);
    } else if (bit1 - bit2 - occupy == 3)
      occupy = 1;
  }
  return occupy;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int powFirst = 0;
  int powSecond = 0;
  powFirst = GetExp(value_1);
  powSecond = GetExp(value_2);
  DecimalNull(result);
  s21_decimal tmpRes;
  DecimalNull(&tmpRes);
  int status = 0;
  int shift = 0;
  for (int i = 95; i >= 0; i--) {
    s21_decimal tmpSecond;
    DecimalNull(&tmpSecond);
    tmpSecond = value_2;
    if (GetBit(&value_1, 95 - i)) {
      int t = 95 - i;
      for (int j = 0; j < t; j++) {
        ShiftDecimalLeft(&tmpSecond);
      }
      status = s21_add(tmpSecond, tmpRes, &tmpRes);
    }
    shift++;
  }

  for (int i = 95; i >= 0; i--) {
    SetBit(result, i, GetBit(&tmpRes, i));
  }

  if (CheckSign(value_1) == 1 && CheckSign(value_2) == 1) {
    SetSign(result, 0);
  } else if (CheckSign(value_1) == 0 && CheckSign(value_2) == 0) {
    SetSign(result, 0);
  } else if ((CheckSign(value_1) == 0 && CheckSign(value_2) == 1) ||
             (CheckSign(value_1) == 1 && CheckSign(value_2) == 0)) {
    SetSign(result, 1);
  }

  if ((powFirst + powSecond) > 28 && CheckSign(*result)) status = 2;
  if ((powFirst + powSecond) > 28 && !CheckSign(*result)) status = 1;

  SetPow(result, powFirst + powSecond);

  return status;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int exp = -1, status = 0, pos = 0;
  int power_1 = GetExp(value_1);
  int power_2 = GetExp(value_2);
  if (power_1 < power_2) {
    Normalize(&value_1, &value_2);
    power_1 = GetExp(value_1);
    power_2 = GetExp(value_2);
  }
  int sign_1 = GetBit(&value_1, 127);
  int sign_2 = GetBit(&value_2, 127);
  DecimalNull(result);
  SetPow(&value_1, 0);
  SetPow(&value_2, 0);
  SetSign(&value_1, 0);
  SetSign(&value_2, 0);
  if (s21_is_equal(value_2, *result)) status = 3;
  for (int i = 95; i >= 0; i--)
    if (pos == 0 && GetBit(&value_2, i)) pos = i;
  s21_decimal ten = value_2;
  while (s21_is_greater_or_equal(value_1, ten) && status == 0) {
    do {
      if (exp != -1) ShiftDecimalLeft(&ten);
      exp++;
    } while (s21_is_less_or_equal(ten, value_1) && exp <= 95 - pos);
    s21_decimal slag = {0};
    SetBit(&slag, exp - 1, 1);
    s21_add(slag, *result, result);
    ShiftDecimalRight(&ten);
    s21_sub(value_1, ten, &value_1);
    SetSign(&value_2, 0);
    exp = -1;
    ten = value_2;
  }
  if (sign_1 != sign_2) SetBit(result, 127, 1);
  SetPow(result, power_1 - power_2);
  return status;
}