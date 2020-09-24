#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Library-level functions.
 * You should use them in the main sections.
 */

uint64_t convertToUint64 (double number) {
    return *((uint64_t *)(&number));
}

bool getBit(uint64_t number, uint8_t bit_num) {
    uint64_t mask;
    mask = 1;
    mask = mask << bit_num;
    return (mask & number) == mask;
}

/**
 * Checkers here:
 */

bool checkForPlusZero(uint64_t number) {
    return number == 0x0000000000000000;
}

bool checkForMinusZero(uint64_t number) {
    return number == 0x8000000000000000;
}

bool checkForPlusInf(uint64_t number) {
    bool sign;
    sign = getBit(number, 63);
    int sum_exp;
    sum_exp = 0;
    for (int i = 52; i < 63; ++i) {
        sum_exp += getBit(number, i);
    }
    int sum_frac;
    sum_frac = 0;
    for (int i = 0; i < 52; ++i) {
        sum_frac += getBit(number, i);
    }
    return sign == 0 && sum_exp == 11 && sum_frac == 0;
}

bool checkForMinusInf (uint64_t number) {
    bool sign;
    sign = getBit(number, 63);
    int sum_exp;
    sum_exp = 0;
    for (int i = 52; i < 63; ++i) {
        sum_exp += getBit(number, i);
    }
    int sum_frac;
    sum_frac = 0;
    for (int i = 0; i < 52; ++i) {
        sum_frac += getBit(number, i);
    }
    return sign == 1 && sum_exp == 11 && sum_frac == 0;
}

bool checkForPlusNormal (uint64_t number) {
    bool sign;
    sign = getBit(number, 63);
    int sum_exp;
    sum_exp = 0;
    for (int i = 52; i < 63; ++i) {
        sum_exp += getBit(number, i);
    }
    return sign == 0 && sum_exp != 0 && sum_exp != 11;
}

bool checkForMinusNormal (uint64_t number) {
    bool sign;
    sign = getBit(number, 63);
    int sum_exp;
    sum_exp = 0;
    for (int i = 52; i < 63; ++i) {
        sum_exp += getBit(number, i);
    }
    return sign == 1 && sum_exp != 0 && sum_exp != 11;

}

bool checkForPlusDenormal (uint64_t number) {
    bool sign;
    sign = getBit(number, 63);
    int sum_exp;
    sum_exp = 0;
    for (int i = 52; i < 63; ++i) {
        sum_exp += getBit(number, i);
    }
    int sum_frac;
    sum_frac = 0;
    for (int i = 0; i < 52; ++i) {
        sum_frac += getBit(number, i);
    }
    return sign == 0 && sum_exp == 0 && sum_frac != 0;
}

bool checkForMinusDenormal (uint64_t number) {
    bool sign;
    sign = getBit(number, 63);
    int sum_exp;
    sum_exp = 0;
    for (int i = 52; i < 63; ++i) {
        sum_exp += getBit(number, i);
    }
    int sum_frac;
    sum_frac = 0;
    for (int i = 0; i < 52; ++i) {
        sum_frac += getBit(number, i);
    }
    return sign == 1 && sum_exp == 0 && sum_frac != 0;
}

bool checkForSignalingNan (uint64_t number) {
    bool sign;
    sign = getBit(number, 63);
    int sum_exp;
    sum_exp = 0;
    for (int i = 52; i < 63; ++i) {
        sum_exp += getBit(number, i);
    }
    int sum_frac;
    sum_frac = 0;
    for (int i = 0; i < 52; ++i) {
        sum_frac += getBit(number, i);
    }
    return sign == 1 && sum_exp == 11 && sum_frac != 0 && getBit(number, 51) != 1;
}

bool checkForQuietNan (uint64_t number) {
    bool sign;
    sign = getBit(number, 63);
    int sum_exp;
    sum_exp = 0;
    for (int i = 52; i < 63; ++i) {
        sum_exp += getBit(number, i);
    }
    return sign == 1 && sum_exp == 11 && getBit(number, 51) == 1;
}

int main() {

    void classify(double number) {
        if (checkForPlusZero(convertToUint64(number))) {
            printf("Plus zero\n");
        }

        else if (checkForMinusZero(convertToUint64(number))) {
            printf("Minus zero\n");
        }

        else if (checkForPlusInf(convertToUint64(number))) {
            printf("Plus inf\n");
        }

        else if (checkForMinusInf(convertToUint64(number))) {
            printf("Minus inf\n");
        }

        else if (checkForPlusNormal(convertToUint64(number))) {
            printf("Plus normal\n");
        }

        else if (checkForMinusNormal(convertToUint64(number))) {
            printf("Minus normal\n");
        }

        else if (checkForPlusDenormal(convertToUint64(number))) {
            printf("Plus Denormal\n");
        }

        else if (checkForMinusDenormal(convertToUint64(number))) {
            printf("Minus Denormal\n");
        }

        else if (checkForSignalingNan(convertToUint64(number))) {
            printf("Signailing NaN\n");
        }

        else if (checkForQuietNan(convertToUint64(number))) {
            printf("Quiet NaN\n");
        }

        else {
            printf("Error.\n");
        }
    }

    return 0;
}