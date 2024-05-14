#pragma once

// define specific conditions here

// ex - #define TEST


// Avoid dulplicated expressions
#define NOMINMAX /*For max() and min() in flatbuffers::base.h and minwindef.h*/

#define USE_BIG_MEMORY_BLOCK

#define USE_GLOBAL_JOBQUEUE

#define GLOG_USE_GLOG_EXPORT /*Definition for glog*/
#define GLOG_NO_ABBREVIATED_SEVERITIES /*Definition for glog*/


#ifdef _DEBUG
#define TEST
#define CHECK_DESTRUCTOR


#define DEBUG
#else

#endif // _DEBUG
