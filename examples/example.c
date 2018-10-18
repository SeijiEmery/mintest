#include "mintest.h"
#include <assert.h>

// Test cases

UNITTEST(testSuite1) {
    int a = 10, b = 12;     // change any of these to see test failures
    ASSERT_EQ(a + 2, b);
    ASSERT_THROWN(assert(false));
    ASSERT_NOTHROW(assert(true));
}
UNITTEST(testSuite2) {
    int* foo = NULL;        // change any of these to see test failures
    ASSERT_SEGV(*foo = 10);

    int bar = 12;
    foo = &bar;
    ASSERT_NOTHROW(*foo = 13);
    ASSERT_EQ(bar, 13);
}

// Run all the testcases

UNITTEST(main) {
    RUN_TEST(testSuite1);
    RUN_TEST(testSuite2);
}

// Invoke via main like this

int main (int argc, const char** argv) {
    RUN_ROOT_TEST(main);
    return 0;
}
