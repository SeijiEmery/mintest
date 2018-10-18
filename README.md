#Mintest

This is a small, and extremely minimalistic unittesting library for C that is capable of testing asserts and segfaults via unix signal handlers.

At the moment, it includes some very basic testing macros

	ASSERT_EQ(a, b)
	ASSERT_THROWS(expr)
	ASSERT_SEGV(expr)
	ASSERT_NOTHROW(expr)
	
And macros for setting up and running unittests

	UNITTEST(test_name) {
		// tests...
	}
	
	RUN_TEST(test_name) 	// use within unittest
	RUN_ROOT_TEST(test_name) // use within main()


It also provides colored text output via ansi escape codes, so make sure that you're using a terminal capable of displaying those.

To build and run the example:

	cd examples
	make run