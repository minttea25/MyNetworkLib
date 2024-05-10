#pragma once

// define specific conditions here

// ex - #define TEST


// Avoid dulplicated expressions
#define NOMINMAX /*For max() and min() in flatbuffers::base.h and minwindef.h*/

#define USE_BIG_MEMORY_BLOCK

#define USE_GLOBAL_JOBQUEUE


#ifdef _DEBUG
#define TEST
#define CHECK_DESTRUCTOR


#define DEBUG
#else

#endif // _DEBUG
