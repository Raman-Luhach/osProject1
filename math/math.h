#ifndef CUSTOM_MATH_H
#define CUSTOM_MATH_H

int custom_multiply(int a, int b);
int custom_divide(int a, int b);
int custom_modulo(int a, int b);
int custom_abs(int value);
int custom_min(int a, int b);
int custom_max(int a, int b);
int clamp(int value, int min_val, int max_val);
int is_in_bounds(int x, int y, int min_x, int min_y, int max_x, int max_y);

#endif
