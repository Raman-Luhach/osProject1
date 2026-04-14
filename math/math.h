/* ========================================================================== */
/*                          CUSTOM MATH LIBRARY                              */
/*                                                                           */
/*  Provides basic arithmetic and boundary-checking functions.               */
/*  Replaces <math.h> which is FORBIDDEN in this project.                   */
/* ========================================================================== */

#ifndef CUSTOM_MATH_H
#define CUSTOM_MATH_H

/* ---- Arithmetic Operations ---- */

/* Multiplies two integers and returns the result. */
int custom_multiply(int a, int b);

/* Divides a by b. Returns 0 if b is zero (safe division). */
int custom_divide(int a, int b);

/* Returns a % b. Returns 0 if b is zero (safe modulo). */
int custom_modulo(int a, int b);

/* Returns the absolute value of the given integer. */
int custom_abs(int value);

/* ---- Comparison & Clamping ---- */

/* Returns the smaller of two integers. */
int custom_min(int a, int b);

/* Returns the larger of two integers. */
int custom_max(int a, int b);

/* Clamps value to the range [min_val, max_val]. */
int clamp(int value, int min_val, int max_val);

/* ---- Boundary Checking ---- */

/* Returns 1 if (x, y) is within the rectangle defined by                   */
/* [min_x, max_x] x [min_y, max_y]. Returns 0 otherwise.                   */
int is_in_bounds(int x, int y, int min_x, int min_y, int max_x, int max_y);

#endif /* CUSTOM_MATH_H */
