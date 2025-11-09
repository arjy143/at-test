#ifndef ATTEST_H
#define ATEST_H

// #ifdef __cplusplus
// #include <iostream>
// #include <sstream>
// #endif

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

//helper to find float absolute value
static inline double float_abs(double x)
{
    if (x < 0)
    {
        return -x;
    }
    else
    {
        return x;
    }
}

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

#define ATTEST_FALSE(condition) do \
        { \
            if ((condition)) \
            { \
                fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_FALSE(%s)\n", __FILE__, __LINE__, #condition); \
                attest_current_failed = 1; \
                return; \
            } \
        } while (0)

//generic C implementation of equals check
#define ATTEST_EQUAL(a, b) do \
        { \
            int is_equal = 0; \
            if ((void*)(a) == (void*)(b)) \
            { \
                is_equal = 1; \
            } \
            else if (sizeof(a) == sizeof(char*) && sizeof(b) == sizeof(char*)) \
            { \
                const char* a_string = (const char*)(a); \
                const char* b_string = (const char*)(b); \
                if (a_string && b_string && strcmp(a_string, b_string) == 0) \
                { \
                    is_equal = 1; \
                } \
            } \
            else if (memcmp(&(a), &(b), sizeof(a)) == 0) \
            { \
                is_equal = 1; \
            } \
            else if ((a) == (b)) \
            { \
                is_equal = 1; \
            } \
            if (!is_equal) \
            { \
                fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_EQUAL(%s, %s)\n", __FILE__, __LINE__, #a, #b); \
                attest_current_failed = 1; \
                return; \
            } \
        } while (0)

//generic C implementation of equals check
#define ATTEST_NOT_EQUAL(a, b) do \
        { \
            int is_equal = 0; \
            if ((void*)(a) == (void*)(b)) \
            { \
                is_equal = 1; \
            } \
            else if (sizeof(a) == sizeof(char*) && sizeof(b) == sizeof(char*)) \
            { \
                const char* a_string = (const char*)(a); \
                const char* b_string = (const char*)(b); \
                if (a_string && b_string && strcmp(a_string, b_string) == 0) \
                { \
                    is_equal = 1; \
                } \
            } \
            else if (memcmp(&(a), &(b), sizeof(a)) == 0) \
            { \
                is_equal = 1; \
            } \
            else if ((a) == (b)) \
            { \
                is_equal = 1; \
            } \
            if (is_equal) \
            { \
                fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_NOT_EQUAL(%s, %s)\n", __FILE__, __LINE__, #a, #b); \
                attest_current_failed = 1; \
                return; \
            } \
        } while (0)

        #define ATTEST_LESS_THAN(a, b) do { \
    if (!((a) < (b))) { \
        fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_LESS_THAN(%s, %s) failed\n", \
                __FILE__, __LINE__, #a, #b); \
        attest_current_failed = 1; \
        return; \
    } \
} while(0)

#define ATTEST_GREATER_THAN(a, b) do { \
    if (!((a) > (b))) { \
        fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_GREATER_THAN(%s, %s) failed\n", \
                __FILE__, __LINE__, #a, #b); \
        attest_current_failed = 1; \
        return; \
    } \
} while(0)

#define ATTEST_EQUAL_WITHIN_TOLERANCE(a, b, tolerance) do { \
    if (!(abs_double((double)(a) - (double)(b)) <= (double)(tolerance))) { \
        fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_EQUAL_WITHIN_TOLERANCE(%s, %s) failed\n", \
                __FILE__, __LINE__, #a, #b); \
        attest_current_failed = 1; \
        return; \
    } \
} while(0)


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

#include <type_traits>
#include <string>
#include <vector>

template <typename T>
std::string attest_to_string(const T& x) 
{
    //memory dump
    const unsigned char* p = reinterpret_cast<const unsigned char*>(&x);
    std::string out = "{";
    for (size_t i = 0; i < sizeof(x); ++i) 
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%02X", p[i]);
        out += buf;
        if (i + 1 != sizeof(x))
        {
            out += " ";
        }     
    }
    out += "}";
    return out;
}

//primitive printing
inline std::string attest_to_string(int x) { return std::to_string(x); }
inline std::string attest_to_string(double x) { return std::to_string(x); }
inline std::string attest_to_string(bool x) { return x ? "true" : "false"; }
inline std::string attest_to_string(const std::string& s) { return "\"" + s + "\""; }

//pretty printing vectors
template <typename T>
std::string attest_to_string(const std::vector<T>& v) 
{
    std::string out = "[";
    for (size_t i = 0; i < v.size(); ++i) 
    {
        out += attest_to_string(v[i]);
        if (i + 1 != v.size()) 
        {
            out += ", ";
        }
    }
    out += "]";
    return out;
}


//helper struct to check if == operator exists
template <typename T, typename U>
struct has_equal
{
    private:
        template <typename A, typename B>
        static auto test(int) -> decltype(std::declval<A>() == std::declval<B>(), std::true_type());
        
        template <typename, typename>
        static std::false_type test(...);
    
    public:
        static constexpr bool value = decltype(test<T, U>(0))::value;
};

//general equality check
template <typename T, typename U>
inline typename std::enable_if<has_equal<T,U>::value, bool>::type
attest_equal_implementation(const T& a, const U& b) 
{
    return a == b;
}

//for mem comparison
template <typename T, typename U>
inline typename std::enable_if<!has_equal<T,U>::value && std::is_trivially_copyable<T>::value && std::is_same<T,U>::value, bool>::type
attest_equal_implementation(const T& a, const U& b) 
{
    if (sizeof(T) != sizeof(U)) 
    {
        return false;
    }
    return memcmp(&a, &b, sizeof(T)) == 0;
}

template <typename T>
inline typename std::enable_if<std::is_trivially_copyable<T>::value, bool>::type
attest_equal_implementation(const T& a, const T& b) 
{
    return memcmp(&a, &b, sizeof(T)) == 0;
}

//for strings
inline bool attest_equal_implementation(const char* a, const char* b) 
{
    if (!a || !b) return a == b;
    return strcmp(a, b) == 0;
}

//entry point
template <typename T, typename U>
inline void attest_equal(const T& a, const U& b,
                         const char* a_str, const char* b_str,
                         const char* file, int line)
{
    //should automatically deduce the types
    bool is_equal = false;

    is_equal = attest_equal_implementation(a, b);
    
    if (!is_equal)
    {
        fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_EQUAL(%s, %s) failed\n", file, line, attest_to_string(a).c_str(), attest_to_string(b).c_str());
        attest_current_failed = 1;
    }
}

template <typename T, typename U>
inline void attest_not_equal(const T& a, const U& b,
                         const char* a_str, const char* b_str,
                         const char* file, int line)
{
    //should automatically deduce the types
    bool is_equal = false;

    is_equal = attest_equal_implementation(a, b);
    
    if (is_equal)
    {
        fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ATTEST_NOT_EQUAL(%s, %s) failed\n", file, line, a_str, b_str);
        attest_current_failed = 1;
    }
}

#define ATTEST_EQUAL(a, b) attest_equal(a, b, #a, #b, __FILE__, __LINE__)
#define ATTEST_NOT_EQUAL(a, b) attest_not_equal(a, b, #a, #b, __FILE__, __LINE__)
// #define ATTEST_LESS_THAN(a, b) attest_less_than(a, b, #a, #b, __FILE__, __LINE__)
// #define ATTEST_GREATER_THAN(a, b) attest_greater_than(a, b, #a, #b, __FILE__, __LINE__)
// #define ATTEST_EQUAL_WITHIN_TOLERANCE(a, b, tolerance) attest_equal_within_tolerance(a, b, tolerance, #a, #b, __FILE__, __LINE__)

#endif

#endif //ATTEST_H