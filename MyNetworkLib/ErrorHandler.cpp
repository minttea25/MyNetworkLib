#include "pch.h"
#include "ErrorHandler.h"

NetCore::Atomic<DWORD> NetCore::ErrorHandler::_app_last_error = Errors::NONE;