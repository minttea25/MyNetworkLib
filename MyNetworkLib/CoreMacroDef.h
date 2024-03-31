#pragma once

/***********************************
* Values
************************************/


/***********************************
* Utils
************************************/
#define IN
#define OUT

#define NOT_USE

#define NAMESPACE_OPEN(name) namespace name {
#define NAMESPACE_CLOSE }

#define CHECK_NAMESPACE_CLOSE #ifdef NAMESPACE_OPEN                     \
    #ifndef NAMESPACE_CLOSE                                             \
        #error "NAMESPACE_CLOSE is not defined after NAMESPACE_OPEN."   \
    #endif                                                              \
#endif


/*******************************************
* DEBUGGING
********************************************/

#define MESSAGE(text) std::cout << #text << std::endl;

#define SHOW(name, value) {                             \
    std::cout << #name << ": " << value << std::endl;    \
}                                                       \

#define WARN(text) \
{   \
std::cout << "WARNING: " << #text << std::endl;    \
}   \

#define ERR_CODE(code, text) \
{   \
std::cerr << code << ": " << #text << std::endl;    \
}   \

#define ERR(text) \
{   \
std::cerr << #text << std::endl;    \
}   \

#define WSA_ERR(code)   \
{   \
std::cerr << "WSAGetLastError() was " << code << std::endl;    \
}   \


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




