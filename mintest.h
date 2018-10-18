//
// A minimalistic unittesting framework that uses signal handling +
// longjmp to detect + test assertions + segfaults.
//
// See examples/example.c for usage.
//
// Feel free to extend by adding ASSERT_**** macros.
//

#ifndef __MINTEST_FRAMEWORK__
#define __MINTEST_FRAMEWORK__
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>


static int     g_lastCaughtSignal = 0;
static jmp_buf g_exceptionHandler;

static void handleSignal (int sig) {
    g_lastCaughtSignal = sig;
    longjmp(g_exceptionHandler, sig);
}
static void catchSignal (int sig) {
    if (signal(sig, handleSignal) == SIG_ERR) {
        fprintf(stderr, "An error occured while setting up signal handler %d\n", sig);
        exit(-1);
    }
}
static void setupSignalHandlers () {
    catchSignal(SIGABRT);
    catchSignal(SIGSEGV);
}

typedef struct Test Test;
struct Test {
    int passed;
    int failed;
};

#define SET_COLOR(color) "\033[" #color "m"
#define SET_RED SET_COLOR(31)
#define SET_GREEN SET_COLOR(32)
#define SET_YELLOW SET_COLOR(33)
#define SET_CYAN SET_COLOR(36)
#define SET_CLEAR "\033[0m"
#define CLEAR_EOL SET_CLEAR "\n"

#define LOG_ASSERT_FAIL(args...) do { \
    fprintf(stderr, SET_RED "Test failed: %s:%d:\t", __FILE__, __LINE__); \
    fprintf(stderr, args); \
    fprintf(stderr, CLEAR_EOL); \
} while (0)

#define LOG_ASSERT_INFO(args...) do { \
    fprintf(stderr, SET_CYAN); \
    fprintf(stderr, args); \
    fprintf(stderr, CLEAR_EOL); \
} while (0)

#define ASSERT_EQ(a,b) \
    if ((a) == (b)) { \
        ++test->passed; \
    } else { \
        ++test->failed; \
        LOG_ASSERT_FAIL(#a " == " #b); \
    } \

#define ASSERT_NE(a,b) \
    if ((a) != (b)) { \
        ++test->passed; \
    } else { \
        ++test->failed; \
        LOG_ASSERT_FAIL(#a " != " #b); \
    } \

#define ASSERT_NOTHROW(expr) \
    if (setjmp(g_exceptionHandler)) { \
        ++test->failed; \
        LOG_ASSERT_FAIL("Unexpected error (signal): %d", g_lastCaughtSignal); \
    } else { \
        (expr); \
        ++test->passed; \
    }

#define ASSERT_THROWS(sig, expr) \
    if (setjmp(g_exceptionHandler)) { \
        if (sig == g_lastCaughtSignal) { \
            ++test->passed; \
        } else { \
            ++test->failed; \
            LOG_ASSERT_FAIL("Expected signal %d, got %d", sig, g_lastCaughtSignal); \
        } \
    } else { \
        (expr); \
        ++test->failed; \
        LOG_ASSERT_FAIL("Expected error, got none"); \
    }

#define ASSERT_THROWN(expr) ASSERT_THROWS(SIGABRT, (expr))
#define ASSERT_SEGV(expr) ASSERT_THROWS(SIGSEGV, (expr))

void runTest (Test* outer, void (*testFcn)(Test*), const char* testName) {
    printf(SET_YELLOW "Running test '%s'" CLEAR_EOL, testName);
    Test inner; inner.passed = inner.failed = 0;
    testFcn(&inner);

    printf(inner.failed ? SET_RED : SET_GREEN);
    printf("Ran test '%s': %d / %d test(s) passed" CLEAR_EOL, 
        testName, inner.passed, inner.passed + inner.failed);
    inner.failed ? ++outer->failed : ++outer->passed;
}
#define RUN_TEST(testFcn) \
    runTest(test, &unittest__##testFcn, #testFcn)

#define UNITTEST(name) \
static void unittest__##name (Test* test)


#define ASSERT_SHOULD_PASS LOG_ASSERT_INFO(SET_GREEN "This should pass");
#define ASSERT_SHOULD_FAIL LOG_ASSERT_INFO(SET_RED "This should fail");

//
// Unittest the testing framework
//

UNITTEST(sanity) {
    ASSERT_SHOULD_PASS ASSERT_EQ(2 + 2, 4);
    ASSERT_SHOULD_FAIL ASSERT_EQ(2 + 2, 5);

    ASSERT_SHOULD_PASS ASSERT_NE(2 + 2, 5);
    ASSERT_SHOULD_FAIL ASSERT_NE(2 + 2, 4);

    ASSERT_SHOULD_PASS ASSERT_NOTHROW(assert(2 + 2 == 4));
    ASSERT_SHOULD_FAIL ASSERT_NOTHROW(assert(2 + 2 == 5));

    ASSERT_SHOULD_PASS ASSERT_THROWN(assert(2 + 2 != 4));
    ASSERT_SHOULD_FAIL ASSERT_THROWN(assert(2 + 2 != 5));

    int  value = 0;
    int* valid = &value;
    int* invalid = NULL;

    ASSERT_SHOULD_PASS ASSERT_SEGV(*invalid = 10);
    ASSERT_SHOULD_PASS ASSERT_SEGV((*invalid == 10) ? value = 1 : 0);
    ASSERT_SHOULD_FAIL ASSERT_SEGV(*valid = 10);
    ASSERT_SHOULD_FAIL ASSERT_SEGV((*valid == 10) ? value = 1 : 0);
}

static void setupUnittestLibrary () {
    LOG_ASSERT_INFO("Setting up unittest framework -- ignore this output --");
    setupSignalHandlers();
    // run initial tests - don't include in checked test results
    Test _test; _test.passed = _test.failed = 0;
    Test* test = &_test;
    RUN_TEST(sanity);
    LOG_ASSERT_INFO("Finished setting up unittest framework -- ignore above output --\n\n");
}

#define RUN_ROOT_TEST(testFcn) \
    do { \
        setupUnittestLibrary(); \
        Test _test; _test.passed = _test.failed = 0; \
        Test* test = &_test; \
        RUN_TEST(testFcn); \
        exit(test->failed ? -1 : 0); \
    } while (0)

#endif //__MINTEST_FRAMEWORK__
