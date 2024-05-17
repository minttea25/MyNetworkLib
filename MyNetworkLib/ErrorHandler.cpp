#include "pch.h"
#include "ErrorHandler.h"

std::atomic<DWORD> NetCore::ErrorHandler::_app_last_error = Errors::NONE;

#ifdef _DEBUG
std::map<DWORD, const char*> NetCore::ErrorHandler::_errors;
#endif // _DEBUG

