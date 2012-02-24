#ifndef asserts_h_INCLUDED
#define asserts_h_INCLUDED

/* Copied from QP-nano. */
#define ASSERT_COMPILE(test_) \
    extern char assert_compile[(test_) ? 1 : -1]

#endif
