#pragma once

typedef void TestCaseFn(void);

struct TestCase
{
    const char*     name;
    TestCaseFn*     func;
    TestCase*       next;

                    TestCase(const char* name, TestCaseFn* func);
};

/// TEST_CASE()
/// Define a unit test case
/// @param: name - Name of the test case
#define TEST_CASE(name)                                                 \
    static void TEST_CASE_FN_ ## __LINE__(void);                        \
    static TestCase TEST_CASE_ ## __LINE__ = TestCase(name, function);  \
    void TEST_CASE_FN_ ## __LINE__(void)

/// Run all unit test cases
int Testing_RunAllCases(const char* argv[], int argc);

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
