#include <stdio.h>
#include <assert.h>
#include <misc/testing.h>

struct TestCase* g_head_test_case = nullptr;
struct TestCase* g_tail_test_case = nullptr;

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

    if (g_head_test_case == nullptr)
    {
        g_head_test_case = this;
        this->next = nullptr;
    }
    else
    {
        this->next = g_tail_test_case;
    }

    g_tail_test_case = this;
}

int testing_run_all_cases(const char* argv[], int argc)
{
    int test_count   = 0;
    int test_success = 0;

    TestCase* test_case = g_head_test_case;
    while (test_case != nullptr)
    {
        assert(test_case->name != nullptr);
        assert(test_case->func != nullptr);
        assert(test_case->file != nullptr);
        assert(test_case->line > 0);

        printf("[Testing] Start run test %s at %s:%d\n", test_case->name, test_case->file, test_case->line);
        test_case->func();

        test_case = test_case->next;

        test_count++;
        test_success++;
    }

    printf("[Testing] Run all %d cases, success %d cases\n", test_count, test_success);

    return 0;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
