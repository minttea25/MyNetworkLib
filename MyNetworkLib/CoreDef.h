#pragma once

// define specific conditions here

// Avoid dulplicated expressions
#define NOMINMAX /*For max() and min() in flatbuffers::base.h and minwindef.h*/

#define USE_BIG_MEMORY_BLOCK

#define USE_GLOBAL_JOB_SERIALIZER

#ifdef _DEBUG
#define TEST
#define CHECK_DESTRUCTOR
//#define NO_USE_GLOG

#define DEBUG
#else

#endif // _DEBUG
