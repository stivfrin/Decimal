#include "s21_decimal.h"

int s21_is_equal(s21_decimal value1, s21_decimal value2) {
  int res = 1;
  int sign1 = GetBit(&value1, 127);
  int sign2 = GetBit(&value2, 127);

  if (sign1 == !sign2)
    res = (CheckIsZero(value1) && CheckIsZero(value2)) ? 1 : 0;
  else {
    s21_big_decimal bval1, bval2;
    BDecimalNull(&bval1);
    BDecimalNull(&bval2);
    Normalize(&value1, &value2);
    TransfToBig(&bval1, value1);
    TransfToBig(&bval2, value2);
    for (int t = 6; t >= 0 && res; t--) {
      if (bval1.bits[t] != bval2.bits[t]) {
        res = 0;
      }
    }
  }
  return res;
}

int s21_is_not_equal(s21_decimal value1, s21_decimal value2) {
  return !(s21_is_equal(value1, value2));
}

int s21_is_greater_or_equal(s21_decimal value1, s21_decimal value2) {
  int res = 0;
  if (s21_is_equal(value1, value2) || s21_is_greater(value1, value2)) res = 1;
  return res;
}

int s21_is_greater(s21_decimal value1, s21_decimal value2) {
  int res = 1;
  int sign1 = CheckSign(value1);
  int sign2 = CheckSign(value2);
  if ((sign1 == 1) && (sign2 == 0)) {
    res = 0;
  } else if ((sign1 == 0) && (sign2 == 1)) {
    if (CheckIsZero(value1) == 1 && CheckIsZero(value2) == 1) {
      res = 0;
    } else {
      res = 1;
    }
  } else {
    if (s21_is_less_or_equal(value1, value2) == 1) {
      res = 0;
    }
  }

  return res;
}

int s21_is_less_or_equal(s21_decimal value1, s21_decimal value2) {
  int res = 0;
  if (s21_is_less(value1, value2) || s21_is_equal(value1, value2)) res = 1;
  return res;
}

int s21_is_less(s21_decimal value1, s21_decimal value2) {
  int res = 1;
  int sign1 = CheckSign(value1);
  int sign2 = CheckSign(value2);
  if ((sign1 == 1) && (sign2 == 0)) {
    res = (CheckIsZero(value1) + CheckIsZero(value2) == 2) ? 0 : 1;
  } else if ((sign1 == 0) && (sign2 == 1)) {
    res = 0;
  } else {
    s21_big_decimal bigd1, bigd2;
    BDecimalNull(&bigd1);
    BDecimalNull(&bigd2);
    Normalize(&value1, &value2);
    TransfToBig(&bigd1, value1);
    TransfToBig(&bigd2, value2);
    if (s21_is_equal(value1, value2)) {
      res = 0;
    } else {
      for (int t = 6; t >= 0; t--) {
        if ((sign1 == 1 && bigd1.bits[t] < bigd2.bits[t]) ||
            (sign1 == 0 && bigd1.bits[t] > bigd2.bits[t])) {
          res = 0;
          break;
        } else if ((sign1 == 1 && bigd1.bits[t] > bigd2.bits[t]) ||
                   (sign1 == 0 && bigd1.bits[t] < bigd2.bits[t])) {
          res = 1;
          break;
        }
      }
    }
  }
  return res;
}
