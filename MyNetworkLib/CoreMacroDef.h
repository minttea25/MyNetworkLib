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
* Logging
********************************************/

#define __make_error_code_stream(code, name) "[E" << code << "] " << name

#ifndef NO_USE_GLOG
#define __NETCORE_LOG_BASE(severity) LOG(severity)
#else
#define __NETCORE_LOG_BASE(severity) std::cerr << #severity << ": "
#endif // !NO_USE_GLOG

#define __NETCORE_LOG_INFO(log) __NETCORE_LOG_BASE(INFO) << #log
#define __NETCORE_LOG_WARNING(log) __NETCORE_LOG_BASE(WARNING) << #log
#define __NETCORE_LOG_ERROR(log) __NETCORE_LOG_BASE(ERROR) << #log
#define __NETCORE_LOG_FATAL(log) __NETCORE_LOG_BASE(FATAL) << #log

#define __NETCORE_LOG_MSG(text) __NETCORE_LOG_BASE(INFO) << #text <<'\n';
#define __NETCORE_LOG_VALUE(text, value) __NETCORE_LOG_BASE(INFO) << #text << value;

#ifndef _DEBUG

#define __NETCORE_CODE_ERROR(code)  __NETCORE_LOG_BASE(ERROR) << "Error: " << code;
#define __NETCORE_CODE_ERROR_STR(code, str)  __NETCORE_LOG_BASE(ERROR) << "Error " << code << ": " << str;
#define __NETCORE_CODE_ERROR_TEXT(code, text)  __NETCORE_LOG_BASE(ERROR) << "Error " << code << ": " << #text;

#else

#define __NETCORE_CODE_ERROR(code)  \
{   \
const char* name = ErrorHandler::GetErrorName(code);    \
__NETCORE_LOG_BASE(ERROR) << __make_error_code_stream(code, name);  \
}   \

#define __NETCORE_CODE_ERROR_STR(code, str)  \
{   \
const char* name = ErrorHandler::GetErrorName(code);    \
__NETCORE_LOG_BASE(ERROR) << __make_error_code_stream(code, name) << '\n' << str; \
}   \

#define __NETCORE_CODE_ERROR_TEXT(code, text)  \
{   \
const char* name = ErrorHandler::GetErrorName(code);    \
__NETCORE_LOG_BASE(ERROR) << __make_error_code_stream(code, name) << '\n' << #text; \
}   \

#endif // !_DEBUG


#define __NETCORE_LOG_WSA_ERROR(code) __NETCORE_LOG_BASE(ERROR) << "WSAGetLastError() was " << code << '\n'


/*******************************************
* DEBUGGING
********************************************/

#ifdef TEST
#define MESSAGE(text) std::cerr << #text << std::endl
#define PRINT(text, value) std::cerr << #text << value << std::endl
#define PRINT_(code) std::cerr << code << std::endl
#define WARN(text) std::cerr << "WARNING: " << #text << std::endl

#else
#define MESSAGE(text)
#define PRINT(text, value) 
#define PRINT_(code) 
#define WARN(text) 

#endif // TEST

#ifdef CHECK_DESTRUCTOR
#define DESTRUCTOR(name) std::cerr << '~' << #name << std::endl
#define DESTRUCTOR_(name, value_name, value) std::cerr << '[' << #value_name << ": " << value << "] ~" << #name << std::endl

#else
#define DESTRUCTOR(name)
#define DESTRUCTOR_(name, value_name, value)

#endif // CHECK_DESTRUCTOR





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




