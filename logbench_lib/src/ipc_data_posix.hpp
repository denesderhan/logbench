//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstddef>
#include <sys/mman.h>

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
		void remove() noexcept;
    private:
        void lock_mem_to_phys();

        inline static const char* resource_name_{ "/logbench_ipc_mem" };
        inline static void* buff_ptr_{ nullptr };
        inline static std::size_t buffer_size_{ 0 };
		inline static int fd_{ -1 };
    };
}
