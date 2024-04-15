#pragma once

/***********************************
* Values
************************************/

constexpr auto UNSIGNED_INVALID = 0;

/***********************************
* Utils
************************************/
#define IN /*Parameter IN*/
#define OUT /*Parameter OUT*/

#define ABSTRACT /*This is abstract class.*/
#define PURE_VIRTUAL = 0 /*Pure virtuals. It needs virtual keyword at front.*/

#define NOT_USE /*Not-in-use variable.*/

#define _USE_LOCK Lock _lock
#define USE_LOCK(name) Lock _##name##Lock
#define _READ_LOCK ReadLock __r_lock(_lock)
#define READ_LOCK(name) ReadLock __r_lock(_##name##Lock)
#define _WRITE_LOCK WriteLock __w_lock(_lock)
#define WRITE_LOCK(name) WriteLock __w_lock(_##name##Lock)

#define _USE_COMMON_LOCK Mutex _mutex
#define _LOCK_GUARD LockGuard _guard(_mutex)

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

#ifdef TEST
#define MESSAGE(text) std::cout << #text << std::endl;
#else
#define MESSAGE(text)
#endif // TEST


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
std::cerr << "Error: " << #text << std::endl;    \
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




