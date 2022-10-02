#pragma once

typedef void TestCaseFn(void);

struct TestCase
{
    const char*     name;
    TestCaseFn*     func;

    const char*     file;
    int             line;
    
    TestCase*       next;

                    TestCase(const char* name, TestCaseFn* func, const char* file, int line);
};

/// TEST_CASE()
/// Define a unit test case
/// @param: name - Name of the test case
#define TEST_CASE(name)																					\
    static void TEST_CASE_FN_ ## __LINE__(void);														\
    static TestCase TEST_CASE_ ## __LINE__(name, TEST_CASE_FN_ ## __LINE__, __FILE__, __LINE__);		\
    void TEST_CASE_FN_ ## __LINE__(void)

/// Run all unit test cases
int Testing_RunAllCases(const char* argv[], int argc);

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
