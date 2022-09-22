#include <assert.h>
#include <Misc/Testing.h>

struct TestCase* gHeadTestCase = nullptr;
struct TestCase* gTailTestCase = nullptr;

TestCase::TestCase(const char* name, TestCaseFn* func)
    : name(name)
    , func(func)
{
    assert(name != nullptr);
    assert(func != nullptr);

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
    TestCase* testCase = gHeadTestCase;
    while (testCase != nullptr)
    {
        assert(testCase->name != nullptr);
        assert(testCase->func != nullptr);

        testCase->func();

        testCase = testCase->next;
    }

    return 0;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
