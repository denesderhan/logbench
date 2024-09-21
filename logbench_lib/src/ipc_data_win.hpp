//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstddef>

namespace logbench {
    class ipc_data {

    public:
        ipc_data() noexcept;
        ~ipc_data() noexcept;

        bool initialized() const noexcept;
        void* data() const noexcept;
		std::size_t size() const noexcept;
        static std::size_t page_size() noexcept;
        void create(std::size_t buffer_size);
        void open(std::size_t buffer_size);
        void close() noexcept;
		void remove() noexcept {}
    private:
        void lock_mem_to_phys();

        inline static const TCHAR* resource_name_{ TEXT("logbench_ipc_mem") };
        inline static HANDLE map_file_ptr_{ NULL };
        inline static LPCTSTR buff_ptr_{ NULL };
        inline static std::size_t buffer_size_{ 0 };
    };
}
