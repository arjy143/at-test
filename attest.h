#ifndef ATTEST_H
#define ATEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
    extern "C" {
#endif

/*  Attest - a minimal unit testing framework for C and C++
*/

typedef void (*attest_func_t)(void);

//representation of a testcase
typedef struct attest_testcase
{
    const char* name;
    attest_func_t func;
    struct attest_testcase *next;
} attest_testcase_t;

extern attest_testcase_t *attest_head;

//macro for registering tests
#define REGISTER_TEST(name) \
        static void name(void); \
        static attest_testcase_t name##_case = {#name, name, NULL}; \
        __attribute((constructor)) static void register_##name(void) \
        { \
            name##_case.next = attest_head; \
            attest_head = &name##_case; \
        } \
        static void name(void)

//basic assertion macros

#define ATTEST_TRUE(condition) do \
        { \
            if (!(condition)) \
            { \
                fprintf(stderr, "[FAIL] %s:%d: ASSERT_TRUE(%s)\n", __FILE__, __LINE__, #condition); \
                exit(1); \
            } \
        } while (0)

#define ATTEST_EQ(a, b) do \
        { \
            if (!((a) == (b))) \
            { \
                fprintf(stderr, "[FAIL] %s:%d: ASSERT_EQ(%s, %s) - found %lld vs %lld\n", __FILE__, __LINE__, #a, #b, (long long)(a), (long long)(b)); \
                exit(1); \
            } \
        } while (0)

#define ATTEST_STR_EQ(a, b) do \
        { \
            if (strcmp((a), (b)) != 0) \
            { \
                fprintf(stderr, "[FAIL] %s:%d: ASSERT_STR_EQ(%s, %s)\n", __FILE__, __LINE__, #a, #b); \
                exit(1); \
            } \
        } while (0)

int run_all_tests(const char* filter);

#ifdef ATTEST_IMPLEMENTATION

attest_testcase_t* attest_head = NULL;

//currently it would only work on linux
//TODO: figure out how to make it os independent
int run_all_tests(const char* filter)
{
    int passed = 0;
    int failed = 0;
    for (attest_testcase_t* t = attest_head; t; t = t->next)
    {
        if (filter && !strstr(t->name, filter))
        {
            continue;     
        }

        printf("[RUN] %s\n", t->name);
        fflush(stdout);

        //get another process to run the test
        int pid = fork();
        if (pid == 0)
        {
            t->func();
            exit(0);
        }
        else
        {
            //check if test has passed or not
            int status;
            wait(&status);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            {
                printf("Passed.\n");
                passed++;
            }
            else
            {
                printf("Failed.\n";)
            }
        }
    }

    //if at least 1 test fails then the run will return 1, otherwise 0
    printf("\n=====Summary=====\n%d passed, %d failed\n", passed, failed);
    return failed > 0 ? 1 : 0;
}

//default main if one is not provided
int main(int argc, char** argv)
{
    //no filter by default
    const char* filter = NULL;

    if (argc > 1 && strncmp(argv[1], "--filter=", 9) == 0)
    {
        filter = argv[1] + 9;
    }
    return run_all_tests(filter);
}

#endif //ATTEST_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif

#endif //ATTEST_H