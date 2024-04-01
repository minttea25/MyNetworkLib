#include "pch.h"
#include "ErrorHandler.h"

NetCore::Atomic<DWORD> NetCore::ErrorHandler::_error = Errors::NONE;