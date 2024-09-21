//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#if defined(_WIN32)
#include <ipc_data_win.hpp>

#include <system_error>
#include <cassert>
#include <cstdint>
#include <cstddef>
namespace logbench {

    ipc_data::ipc_data() noexcept = default;
    ipc_data::~ipc_data() noexcept {
            close();
    }

    bool ipc_data::initialized() const noexcept {
        return buff_ptr_ != NULL;
    }

    void* ipc_data::data() const noexcept {
        return PVOID(buff_ptr_);
    }

	std::size_t ipc_data::size() const noexcept {
        return buffer_size_;
    }

	std::size_t ipc_data::page_size() noexcept {
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        return sys_info.dwPageSize;
    }

    //FIX THIS move to header
    DWORD low_part(std::size_t num) {
        if constexpr (sizeof(DWORD) >= sizeof(std::size_t)) {
            return num;
        }
        else {
            static_assert(CHAR_BIT == 8);
            constexpr int bit_size{ sizeof(DWORD) * 8 };
            constexpr std::size_t bit_mask{ std::size_t(-1) >> ((sizeof(std::size_t) - sizeof(DWORD)) * 8) };
            return static_cast<DWORD>(num & bit_mask);
        }
    }

    DWORD high_part(std::size_t num) {
        if constexpr (sizeof(DWORD) >= sizeof(std::size_t)) {
            return 0;
        }
        else {
            static_assert(CHAR_BIT == 8);
            constexpr int bit_size{ sizeof(DWORD) * 8 };
            return static_cast<DWORD>(num >> bit_size);
        }
    }

	void ipc_data::create(std::size_t buffer_size) {
        if (buff_ptr_ != NULL)
        {
            throw std::runtime_error("Resource already initialized!");
        }
        map_file_ptr_ = CreateFileMapping(
            INVALID_HANDLE_VALUE,       // use paging file
            NULL,                       // default security
            PAGE_READWRITE,             // read/write access
            high_part(buffer_size),     // maximum object size (high-order DWORD)
            low_part(buffer_size),      // maximum object size (low-order DWORD)
            resource_name_);            // name of mapping object
        
        if (map_file_ptr_ == NULL)
        {
            throw std::system_error{ 
                std::error_code{
                    static_cast<int>(GetLastError()), 
                        std::system_category()
                }, 
                    "Could not create file mapping object!"
            };
        }

        buff_ptr_ = (LPTSTR)MapViewOfFile(
            map_file_ptr_,                  // handle to map object
            FILE_MAP_ALL_ACCESS,            // read/write permission
            0,
            0,
            buffer_size);

        if (buff_ptr_ == NULL)
        {
            CloseHandle(map_file_ptr_);
            throw std::system_error{ 
                std::error_code{
                    static_cast<int>(GetLastError()),
                        std::system_category()
                }, 
                    "Could not map view of file!" 
            };
        }

        buffer_size_ = buffer_size;
        assert(
            reinterpret_cast<std::uintptr_t>(buff_ptr_) % page_size() == 0 
            && "Memory buffer not aligned to page size!"
        );
        lock_mem_to_phys();
	}

    void ipc_data::open(std::size_t buffer_size) {
        if (buff_ptr_ != NULL)
        {
            throw std::runtime_error("Resource already opened!");
        }
        map_file_ptr_ = OpenFileMapping(
            FILE_MAP_ALL_ACCESS,        // read/write access
            FALSE,                      // do not inherit the name
            resource_name_);            // name of mapping object

        if (map_file_ptr_ == NULL)
        {
            throw std::system_error{
                std::error_code{
                    static_cast<int>(GetLastError()), 
                        std::system_category()
                }, 
                    "Could not open file mapping object!"
            };
        }
        buff_ptr_ = (LPTSTR)MapViewOfFile(
            map_file_ptr_,                  // handle to map object
            FILE_MAP_ALL_ACCESS,            // read/write permission
            0,
            0,
            buffer_size);

        if (buff_ptr_ == NULL)
        {
            CloseHandle(map_file_ptr_);
            throw std::system_error{
                std::error_code{
                    static_cast<int>(GetLastError()),
                        std::system_category()
                }, 
                    "Could not map view of file!" 
            };
        }
        buffer_size_ = buffer_size;
    }

    void ipc_data::close() noexcept {
        if (buff_ptr_ != NULL)
        {
            UnmapViewOfFile(buff_ptr_);
            CloseHandle(map_file_ptr_);
        }
        buff_ptr_ = NULL;
        buffer_size_ = 0;
        map_file_ptr_ = NULL;
    }

    void ipc_data::lock_mem_to_phys() {
        SIZE_T min_work_set{ 0 };
        SIZE_T max_work_set{ 0 };
        HANDLE self = GetCurrentProcess();
        if (!GetProcessWorkingSetSize(self, &min_work_set, &max_work_set))
        {
            throw std::system_error{ 
                std::error_code{
                    static_cast<int>(GetLastError()),
                        std::system_category()
                }, 
                    "GetProcessWorkingSetSize failed!" 
            };
        }

        SIZE_T new_min = min_work_set + ((buffer_size_ * 11) / 10);
        SIZE_T new_max = new_min < max_work_set ? max_work_set : new_min;

        if (!SetProcessWorkingSetSize(self, new_min, new_max))
        {
            int error_code = static_cast<int>(GetLastError());
            throw std::system_error{
                std::error_code{
                    error_code, 
                        std::system_category()
                }, 
                    "SetProcessWorkingSetSize failed!" 
            };
        }
        if (!VirtualLock(PVOID(buff_ptr_), buffer_size_))
        {
            throw std::system_error{ 
                std::error_code{
                    static_cast<int>(GetLastError()),
                        std::system_category()
                }, 
                    "VirtualLock failed!" 
            };
        }
    }
}
#endif
