#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <limits.h>
#include <math.h>
#include <stdio.h>

#define OK 0
#define CONVERTATION_ERROR 1
#define s21_TRUE 1
#define s21_FALSE 0
#define MAX_DECIMAL 79228162514264337593543950335
#define MIN_DECIMAL 1e-28
#define MAX_DEC powf(2.0f, 96.0f) - 1.0
#define MIN_DEC -powf(2.0f, 96.0f) + 1.0
#define MINUS_SIGN 2147483648

// structs

typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef struct {
  unsigned int bits[8];
} s21_big_decimal;

// compairs

int s21_is_less(s21_decimal value1, s21_decimal value2);
int s21_is_less_or_equal(s21_decimal value1, s21_decimal value2);
int s21_is_greater(s21_decimal value1, s21_decimal value2);
int s21_is_greater_or_equal(s21_decimal value1, s21_decimal value2);
int s21_is_equal(s21_decimal value1, s21_decimal value2);
int s21_is_not_equal(s21_decimal value1, s21_decimal value2);

// another funcs

int s21_floor(s21_decimal value, s21_decimal *result);  // --
int s21_round(s21_decimal value, s21_decimal *result);  // --
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

// arithmetic

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// convertation funcs

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

// sub funcs

int Normalize(s21_decimal *value_1, s21_decimal *value_2);
int DecMulTen(s21_decimal value_1, s21_decimal *res);
int Substruction(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int CheckSign(s21_decimal value1);
int CheckIsZero(s21_decimal);
int GetBit(s21_decimal *count, int index);
int GetExp(s21_decimal bit);
int FindFloatExp(float count);
int Adding(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
void TransfToBig(s21_big_decimal *bvalue1, s21_decimal value1);
void SetBit(s21_decimal *count, int index, int bit);
void SetSign(s21_decimal *count, int sign);
void SetPow(s21_decimal *count, int pow);
void DecimalNull(s21_decimal *count);
void DivisionBy10(s21_decimal *count);
void ShiftDecimalLeft(s21_decimal *count);
void ShiftDecimalRight(s21_decimal *count);
void BDecimalNull(s21_big_decimal *value);

#endif