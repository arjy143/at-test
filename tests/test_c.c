#define ATTEST_IMPLEMENTATION

#include "../attest.h"
#include <stdbool.h>

struct Point2
{
    int x;
    int y;
};

REGISTER_TEST(addition)
{
    ATTEST_EQUAL(5, 5);
    ATTEST_EQUAL(3.14f, 3.14f);
    ATTEST_EQUAL(true, true);
    struct Point2 s1 = {1,2};
    struct Point2 s2 = {1,2};
    ATTEST_EQUAL(s1, s2);
}

REGISTER_TEST(string)
{
    ATTEST_EQUAL("hello", "hello");
}

REGISTER_TEST(flag)
{
    attest_current_failed = 0;
    ATTEST_EQUAL(2,2);
}