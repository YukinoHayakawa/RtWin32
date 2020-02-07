#include <Usagi/Experimental/v2/Runtime/Platform/Kernel.hpp>

#include <stdexcept>

#include <Usagi/Utility/Rounding.hpp>
#include <Usagi/Core/Exception.hpp>

#include "Win32.hpp"

namespace usagi::platform::kernel
{
namespace virtual_memory
{
std::size_t page_size()
{
    static std::size_t size = 0;
    if(size == 0)
    {
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        size = sys_info.dwPageSize;
    }
    return size;
}

std::size_t round_up_to_page_size(const std::size_t size_bytes)
{
    return utility::roundUpUnsigned(size_bytes, page_size());
}

// For memory reservation and committing, see:
// https://docs.microsoft.com/en-us/windows/win32/memory/reserving-and-committing-memory

Allocation allocate(const std::size_t size_bytes, const bool commit)
{
    Allocation ret;
    ret.length = size_bytes;

    // https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntallocatevirtualmemory
    const auto status = NtAllocateVirtualMemory(
        GetCurrentProcess(),
        &ret.base_address,
        0,
        &ret.length,
        commit ? MEM_RESERVE | MEM_COMMIT : MEM_RESERVE,
        commit ? PAGE_READWRITE : PAGE_NOACCESS
    );

    // TODO output error messages from NTSTATUS
    if(!NT_SUCCESS(status))
        USAGI_THROW(std::runtime_error(
            "Call to NtAllocateVirtualMemory failed"));

    return ret;
}

void commit(void *ptr, std::size_t size_bytes)
{
    const auto status = NtAllocateVirtualMemory(
        GetCurrentProcess(),
        &ptr,
        0,
        &size_bytes,
        MEM_COMMIT,
        PAGE_READWRITE
    );

    if(!NT_SUCCESS(status))
        USAGI_THROW(std::runtime_error(
            "Call to NtAllocateVirtualMemory failed"));
}

void decommit(void *ptr, std::size_t size_bytes)
{
    const auto status = NtFreeVirtualMemory(
        GetCurrentProcess(),
        &ptr,
        &size_bytes,
        MEM_DECOMMIT
    );

    if(!NT_SUCCESS(status))
        USAGI_THROW(std::runtime_error(
            "Call to NtFreeVirtualMemory failed"));
}

void free(void *ptr, std::size_t size_bytes)
{
    const auto status = NtFreeVirtualMemory(
        GetCurrentProcess(),
        &ptr,
        &size_bytes,
        MEM_RELEASE
    );

    if(!NT_SUCCESS(status))
        USAGI_THROW(std::runtime_error(
            "Call to NtFreeVirtualMemory failed"));
}
}
}

