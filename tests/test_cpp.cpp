#define ATTEST_IMPLEMENTATION

#include "../attest.h"
#include <vector>

struct Point { int x, y; bool operator==(const Point& p) const { return x == p.x && y == p.y; } };

REGISTER_TEST(addition)
{
    ATTEST_EQUAL(2+2, 4);
    ATTEST_EQUAL(5, 5);
    ATTEST_EQUAL(3.14, 3.14);
    ATTEST_EQUAL(true, true);
    Point s1{1,2};
    Point s2{1,2};
    ATTEST_EQUAL(s1, s2);
}

REGISTER_TEST(string)
{
    ATTEST_EQUAL("hello", "hello");
}

REGISTER_TEST(vec)
{
    std::vector<int> a{1,2,3};
    std::vector<int> b{1,2,3};
    ATTEST_EQUAL(a,b);
}

REGISTER_TEST(flag)
{
    attest_current_failed = 0;
    ATTEST_EQUAL(2,2);
}