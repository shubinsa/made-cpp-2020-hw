#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

//test
//test2

/**
 * Library-level functions.
 * You should use them in the main sections.
 */

uint64_t convertToUint64 (double number) {
    return *((uint64_t *)(&number));
}

bool getBit (const uint64_t number, const uint8_t index) {
    /// Your code here...
}


/**
 * Checkers here:
 */

bool checkForPlusZero (uint64_t number) {
    /// Your code here.
}

bool checkForMinusZero (uint64_t number) {
    return number == 0x8000000000000000;
}

bool checkForPlusInf (uint64_t number) {
    /// Your code here.
}

bool checkForMinusInf (uint64_t number) {
    /// Your code here.
}

bool checkForPlusNormal (uint64_t number) {
    /// Your code here.
}

bool checkForMinusNormal (uint64_t number) {
    /// Your code here.
}

bool checkForPlusDenormal (uint64_t number) {
    /// Your code here.
}

bool checkForMinusDenormal (uint64_t number) {
    /// Your code here.
}

bool checkForSignalingNan (uint64_t number) {
    /// Your code here.
}

bool checkForQuietNan (uint64_t number) {
    /// Your code here.
}


void classify (double number) {
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
