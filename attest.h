#ifndef ATTEST_H
#define ATEST_H

#ifdef __cplusplus
#include <iostream>
#include <sstream>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
    extern "C" {
#endif

/*  
    Attest - a minimal unit testing framework for C and C++
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

//  C assertion macros
#define REGISTER_TEST(name) \
        static void name(void); \
        static attest_testcase_t name##_case = {#name, name, NULL}; \
        static void __attribute__((constructor)) register_##name(void) \
        { \
            attest_register(#name, name, __FILE__, __LINE__); \
        } \
        static void name(void)

#define ATTEST_TRUE(condition) do \
        { \
            if (!(condition)) \
            { \
                fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_TRUE(%s)\n", __FILE__, __LINE__, #condition); \
                attest_current_failed = 1; \
                return; \
            } \
        } while (0)

#define ATTEST_INT_EQUAL(a, b) do \
        { \
            if (!((a) == (b))) \
            { \
                fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_INT_EQUAL(%s, %s) - found %lld vs %lld\n", __FILE__, __LINE__, #a, #b, (long long)(a), (long long)(b)); \
                attest_current_failed = 1; \
                return; \
            } \
        } while (0)

#define ATTEST_STRING_EQUAL(a, b) do \
        { \
            if (strcmp((a), (b)) != 0) \
            { \
                fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_STRING_EQUAL(%s, %s)\n", __FILE__, __LINE__, #a, #b); \
                attest_current_failed = 1; \
                return; \
            } \
        } while (0)

#define ATTEST_NOT_EQUAL(a, b) do \
        { \
            if (((a) == (b))) \
            { \
                fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_NOT_EQUAL(%s, %s) - found both to be equal\n", __FILE__, __LINE__, #a, #b, (long long)(a), (long long)(b)); \
                attest_current_failed = 1; \
                return; \
            } \
        } while (0)


void attest_register(const char* name, attest_func_t func, const char* file, int line);
int run_all_tests(const char* filter, int quiet);

#ifdef ATTEST_IMPLEMENTATION

//below is a data structure to keep track of all tests
static attest_testcase_t* attest_head = NULL;

//the below variable is used to keep track of if the current test failed
static int attest_current_failed = 0;

void attest_register(const char* name, attest_func_t func, const char* file, int line)
{
    attest_testcase_t* t = (attest_testcase_t*)malloc(sizeof(attest_testcase_t));
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
            printf("\033[33m[RUN] %s\033[0m -> ", t->name);
        }

        fflush(stdout);

        int before = failed;
        t->func();

        if (attest_current_failed == 0)
        {
            if (!quiet)
            {
                printf("\033[32m[PASS]\033[0m\n");
                
            }
            passed++;
        }
        else
        {
            printf("\033[31m%s failed.\033\n", t->name);     
            failed++;
            attest_current_failed = 0;
        }
        
    }

    //if at least 1 test fails then the run will return 1, otherwise 0
    printf("\033[34m\n=====Summary=====\033[0m\n\033[32m%d\033[0m passed and \033[31m%d\033[0m failed / \033[34m%d\033[0m total\n", passed, failed, passed+failed);
    return failed? 1 : 0;
}

//default main if one is not provided
int main(int argc, char** argv)
{
    //no filter by default
    const char* filter = NULL;
    int quiet = 0;
    int list_only = 0;

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
        else if (strncmp(argv[i], "--list", 9) == 0)
        {
            list_only = 1;
        }
    }
    
    if (list_only)
    {
        printf("List of registered tests:\n");
        for (attest_testcase_t* t = attest_head; t; t = t->next)
        {
            printf("%s\n", t->name);
        }
        return 0;
    }
    return run_all_tests(filter, quiet);
}

#endif //ATTEST_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif

//  C++ template based assertions
#ifdef __cplusplus
template <typename T>
inline void attest_equal(const T& a, const T& b, const char* a_str, const char* b_str, const char* file, int line)
{
    if (!((a) == (b)))
    {
        fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: C++ ATTEST_EQUAL(%s, %s) - found %lld vs %lld\n", __FILE__, __LINE__, a_str, b_str, a, b); \
        attest_current_failed = 1;
    }
}

#define ATTEST_EQUAL(a, b) attest_equal(a, b, #a, #b, __FILE__, __LINE__)
#endif

#endif //ATTEST_H