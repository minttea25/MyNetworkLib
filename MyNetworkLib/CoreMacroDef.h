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

#define ABSTRACT /*It means abstract class.*/
#define PURE_VIRTUAL /*Pure virtuals. It means this function is pure virtual.*/

#define IGNORED /*It means ignorable variable which is not used.*/
#define DISCARD /*It means the return value is not used.*/

#define NAMESPACE_OPEN(name) namespace name { /*Start namespace*/
#define NAMESPACE_CLOSE } /*End namespace*/

/*******************************************
* Lock
********************************************/

#define _USE_LOCK Lock __lock /*Single lock*/
#define USE_LOCK(name) Lock __##name##Lock /*Specific lock*/
#define _READ_LOCK ReadLock __r_lock(__lock) /*Single lock*/
#define READ_LOCK(name) ReadLock __r_lock(__##name##Lock) /*Specific lock*/
#define _WRITE_LOCK WriteLock __w_lock(__lock) /*Single lock*/
#define WRITE_LOCK(name) WriteLock __w_lock(__##name##Lock) /*Specific lock*/

#define _USE_COMMON_LOCK Mutex __mutex /*Base lock with mutex*/
#define _LOCK_GUARD LockGuard __guard(__mutex) /*Base lock with mutex*/


/*******************************************
* DEBUGGING
********************************************/

#ifdef TEST
#define MESSAGE(text) std::cout << #text << std::endl
#else
#define MESSAGE(text)
#endif // TEST

#ifdef CHECK_DESTRUCTOR
#define DESTRUCTOR(name) std::cout << '~' << #name << std::endl
#else
#define DESTRUCTOR(name)
#endif // CHECK_DESTRUCTOR


#define PRINT(text, value) std::cout << #text << value << std::endl

#define WARN(text) std::cout << "WARNING: " << #text << std::endl

#define ERR_CODE_(code) std::cerr << "Error Code: " << code << std::endl

#define ERR_CODE(code, text)  std::cerr << code << ": " << #text << std::endl

#define WSA_ERR(code) std::cerr << "WSAGetLastError() was " << code << std::endl


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




