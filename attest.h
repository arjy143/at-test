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
    const char* file;
    int line;
    struct attest_testcase *next;
} attest_testcase_t;

extern attest_testcase_t *attest_head;

//macro for registering tests
#define REGISTER_TEST(name) \
        static void name(void); \
        static attest_testcase_t name##_case = {#name, name, NULL}; \
        static void __attribute__((constructor)) register_##name(void) \
        { \
            attest_register(#name, name, __FILE__, __LINE__); \
        } \
        static void name(void)

//basic assertion macros

#define ATTEST_TRUE(condition) do \
        { \
            if (!(condition)) \
            { \
                fprintf(stderr, "[FAIL] %s:%d: ASSERT_TRUE(%s)\n", __FILE__, __LINE__, #condition); \
                return; \
            } \
        } while (0)

#define ATTEST_EQ(a, b) do \
        { \
            if (!((a) == (b))) \
            { \
                fprintf(stderr, "[FAIL] %s:%d: ASSERT_EQ(%s, %s) - found %lld vs %lld\n", __FILE__, __LINE__, #a, #b, (long long)(a), (long long)(b)); \
                return; \
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

void attest_register(const char* name, attest_func_t func, const char* file, int line);
int run_all_tests(const char* filter, int quiet);

#ifdef ATTEST_IMPLEMENTATION

attest_testcase_t* attest_head = NULL;
//static attest_testcase_t* tests = NULL;

void attest_register(const char* name, attest_func_t func, const char* file, int line)
{
    attest_testcase_t* t = malloc(sizeof(attest_testcase_t));
    t-> name = name;
    t->func = func;
    t->file = file;
    t->line = line;
    t->next = attest_head;
    attest_head = t;
}

//run tests inside process itself instead of making a new one
int run_all_tests(const char* filter, int quiet)
{
    int passed = 0;
    int failed = 0;
    for (attest_testcase_t* t = attest_head; t; t = t->next)
    {
        if (filter && !strstr(t->name, filter))
        {
            continue;     
        }
        if (!quiet)
        {
            printf("[RUN] %s\n", t->name);
        }

        fflush(stdout);

        int before = failed;
        t->func();

        if (before == failed)
        {
            if (!quiet)
            {
                printf("[PASS] %s\n", t->name);
                passed++;
            }
            else
            {
                printf("[FAIL] %s\n", t->name);
                failed++;
            }
        }
    }

    //if at least 1 test fails then the run will return 1, otherwise 0
    printf("\n=====Summary=====\n%d passed and %d failed / %d total\n", passed, failed, passed+failed);
    return failed? 1 : 0;
}

//default main if one is not provided
int main(int argc, char** argv)
{
    //no filter by default
    const char* filter = NULL;
    int quiet = 0;
    
    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "--filter=", 9) == 0)
        {
            filter = argv[1] + 9;
        }
        else if (strncmp(argv[i], "--quiet", 9) == 0)
        {
            quiet = 1;
        }
    }

    return run_all_tests(filter, quiet);
}

#endif //ATTEST_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif

#endif //ATTEST_H