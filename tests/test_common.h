#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
            return -1; \
        } \
    } while (0)

#endif
