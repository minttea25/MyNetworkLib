#pragma once

/***********************************
* Values
************************************/



/***********************************
* Utils
************************************/
#define IN
#define OUT

#define NAMESPACE_OPEN(name) namespace name {
#define NAMESPACE_CLOSE }

#define CHECK_NAMESPACE_CLOSE #ifdef NAMESPACE_OPEN                     \
    #ifndef NAMESPACE_CLOSE                                             \
        #error "NAMESPACE_CLOSE is not defined after NAMESPACE_OPEN."   \
    #endif                                                              \
#endif

#define MESSAGE(text) std::cout << #text << std::endl;
#define MSG(str)            \
{   \
    std::cout << str << std::endl;\
}   \

#define SHOW(name, value) {                             \
    std::cout << #name << ": " << value << std::endl;    \
}                                                       \


/*******************************************
* Crash Check
********************************************/

// Note: __analysis_assume does not work at runtime actually.
#define CRASH(text)                \
{                                       \
    unsigned int* crash = nullptr;      \
    __analysis_assume(crash != nullptr);\
    *crash = 0xDEADBEEF;                \
}                                       \

#define ASSERT_CRASH(condition)         \
{                                       \
    if (!(condition))                   \
    {                                   \
        CRASH("ASSERT_CRASH occurred.");\
        __analysis_assume(condition);   \
    }                                   \
}                                       \

#define ABORT(condition)                \
{                                       \
    if (!(condition))                   \
    {                                   \
        __analysis_assume(condition);   \
        std::abort();                   \
    }                                   \
}                                       \




