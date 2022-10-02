#include <stdio.h>
#include <assert.h>
#include <Misc/Testing.h>

struct TestCase* gHeadTestCase = nullptr;
struct TestCase* gTailTestCase = nullptr;

TestCase::TestCase(const char* name, TestCaseFn* func, const char* file, int line)
    : name(name)
    , func(func)
    , file(file)
    , line(line)
{
    assert(name != nullptr);
    assert(func != nullptr);
    assert(file != nullptr);
    assert(line > 0);

    if (gHeadTestCase == nullptr)
    {
        gHeadTestCase = this;
        this->next = nullptr;
    }
    else
    {
        this->next = gTailTestCase;
    }

    gTailTestCase = this;
}

int Testing_RunAllCases(const char* argv[], int argc)
{
    int testCount   = 0;
    int testSuccess = 0;

    TestCase* testCase = gHeadTestCase;
    while (testCase != nullptr)
    {
        assert(testCase->name != nullptr);
        assert(testCase->func != nullptr);
        assert(testCase->file != nullptr);
        assert(testCase->line > 0);

        printf("[Testing] Start run test %s at %s:%d\n", testCase->name, testCase->file, testCase->line);
        testCase->func();

        testCase = testCase->next;

        testCount++;
        testSuccess++;
    }

    printf("[Testing] Run all %d cases, success %d cases\n", testCount, testSuccess);

    return 0;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
