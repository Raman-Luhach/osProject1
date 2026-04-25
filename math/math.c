#include "../math/math.h"

/* Multiplication using shift-and-add (binary multiplication)
   Same algorithm a CPU's ALU uses internally */
int custom_multiply(int a, int b)
{
    int result = 0;
    int negative = 0;

    if (a == 0 || b == 0)
        return 0;

    if (a < 0)
    {
        negative = !negative;
        a = -a;
    }
    if (b < 0)
    {
        negative = !negative;
        b = -b;
    }

    /* Shift-and-add: for each set bit in b, add a shifted left by that position */
    while (b > 0)
    {
        if (b & 1)
            result = result + a;
        a = a << 1;
        b = b >> 1;
    }

    if (negative)
        return -result;
    return result;
}

/* Division using bit-shift subtraction (binary long division) */
int custom_divide(int a, int b)
{
    int quotient = 0;
    int negative = 0;
    int shift;

    if (b == 0)
        return 0;
    if (a == 0)
        return 0;

    if (a < 0)
    {
        negative = !negative;
        a = -a;
    }
    if (b < 0)
    {
        negative = !negative;
        b = -b;
    }

    /* Find the highest bit position where b << shift <= a */
    for (shift = 0; (b << shift) <= a && shift < 31; shift++)
        ;
    shift--;

    /* Binary long division: try subtracting b<<shift from a for each shift */
    while (shift >= 0)
    {
        if (a >= (b << shift))
        {
            a = a - (b << shift);
            quotient = quotient | (1 << shift);
        }
        shift--;
    }

    if (negative)
        return -quotient;
    return quotient;
}

/* Modulo: a - (a/b)*b using our binary division */
int custom_modulo(int a, int b)
{
    int negative = 0;
    int q;
    int result;

    if (b == 0)
        return 0;

    if (a < 0)
    {
        negative = 1;
        a = -a;
    }
    if (b < 0)
        b = -b;

    q = custom_divide(a, b);
    result = a - custom_multiply(q, b);

    if (negative)
        return -result;
    return result;
}

/* Absolute value using bit manipulation:
   mask = value >> 31 gives all 1s if negative, all 0s if positive
   (value ^ mask) - mask flips and adds 1 for negative numbers */
int custom_abs(int value)
{
    int mask = value >> 31;
    return (value ^ mask) - mask;
}

/* Min without branching using bit manipulation */
int custom_min(int a, int b)
{
    int diff = a - b;
    int sign = (diff >> 31) & 1;
    /* sign is 1 if a < b, 0 if a >= b */
    return b + (diff & (-sign));
}

/* Max without branching */
int custom_max(int a, int b)
{
    int diff = a - b;
    int sign = (diff >> 31) & 1;
    /* sign is 1 if a < b (so b is max), 0 if a >= b (so a is max) */
    return a - (diff & (-sign));
}

int clamp(int value, int min_val, int max_val)
{
    if (value < min_val)
        return min_val;
    if (value > max_val)
        return max_val;
    return value;
}

int is_in_bounds(int x, int y, int min_x, int min_y, int max_x, int max_y)
{
    if (x >= min_x && x <= max_x && y >= min_y && y <= max_y)
        return 1;
    return 0;
}
