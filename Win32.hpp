#pragma once

/*
 * This file includes the Windows header and undef any macros conflicting with
 * our codes.
 */

#pragma warning(disable: 4005) // macro redefinition

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#define DEVICE_TYPE DWORD
typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;
#include "ntos.h"
