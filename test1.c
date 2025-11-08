#define ATTEST_IMPLEMENTATION

#include "attest.h"
#include <vector>

REGISTER_TEST(addition)
{
    ATTEST_INT_EQUAL(2+2, 4);
}

REGISTER_TEST(string)
{
    ATTEST_STRING_EQUAL("hello", "hello");
}

REGISTER_TEST(vec)
{
    std::vector<int> a{1,2,3};
    std::vector<int> b{1,2,4};
    ATTEST_EQUAL(a,b);
}

REGISTER_TEST(flag)
{
    attest_current_failed = 0;
    ATTEST_INT_EQUAL(2,2);
}