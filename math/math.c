/* ========================================================================== */
/*                          CUSTOM MATH LIBRARY                              */
/*                                                                           */
/*  Implementation of basic arithmetic and boundary-checking functions.      */
/*  All functions guard against invalid inputs (e.g., division by zero).     */
/* ========================================================================== */

#include "../math/math.h"

/* -------------------------------------------------------------------------- */
/*  custom_multiply                                                          */
/*  Returns the product of a and b.                                          */
/* -------------------------------------------------------------------------- */
int custom_multiply(int a, int b)
{
    return a * b;
}

/* -------------------------------------------------------------------------- */
/*  custom_divide                                                            */
/*  Returns a / b.  If b is zero, returns 0 to prevent crashes.             */
/* -------------------------------------------------------------------------- */
int custom_divide(int a, int b)
{
    if (b == 0)
        return 0;
    return a / b;
}

/* -------------------------------------------------------------------------- */
/*  custom_modulo                                                            */
/*  Returns a % b.  If b is zero, returns 0 to prevent crashes.             */
/* -------------------------------------------------------------------------- */
int custom_modulo(int a, int b)
{
    if (b == 0)
        return 0;
    return a % b;
}

/* -------------------------------------------------------------------------- */
/*  custom_abs                                                               */
/*  Returns the absolute value of the given integer.                         */
/* -------------------------------------------------------------------------- */
int custom_abs(int value)
{
    if (value < 0)
        return -value;
    return value;
}

/* -------------------------------------------------------------------------- */
/*  custom_min                                                               */
/*  Returns the smaller of two integers.                                     */
/* -------------------------------------------------------------------------- */
int custom_min(int a, int b)
{
    if (a < b)
        return a;
    return b;
}

/* -------------------------------------------------------------------------- */
/*  custom_max                                                               */
/*  Returns the larger of two integers.                                      */
/* -------------------------------------------------------------------------- */
int custom_max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

/* -------------------------------------------------------------------------- */
/*  clamp                                                                    */
/*  Restricts 'value' to the range [min_val, max_val].                      */
/*  If value < min_val, returns min_val.                                     */
/*  If value > max_val, returns max_val.                                     */
/*  Otherwise, returns value unchanged.                                      */
/* -------------------------------------------------------------------------- */
int clamp(int value, int min_val, int max_val)
{
    return custom_max(min_val, custom_min(value, max_val));
}

/* -------------------------------------------------------------------------- */
/*  is_in_bounds                                                             */
/*  Checks if the point (x, y) lies within the rectangle defined by:        */
/*    min_x <= x <= max_x   AND   min_y <= y <= max_y                       */
/*  Returns 1 if inside bounds, 0 otherwise.                                */
/* -------------------------------------------------------------------------- */
int is_in_bounds(int x, int y, int min_x, int min_y, int max_x, int max_y)
{
    if (x >= min_x && x <= max_x && y >= min_y && y <= max_y)
        return 1;
    return 0;
}
