//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#ifndef _WIN32
#include <ipc_data_posix.hpp>

#include <cstddef>
#include <cstdint>
#include <system_error>
#include <cassert>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace logbench {

    ipc_data::ipc_data() noexcept = default;
    ipc_data::~ipc_data() noexcept {
    	close();
		if (fd_ != -1) {
			::close(fd_);
		}
    }

	void ipc_data::remove() noexcept {
		shm_unlink(resource_name_);
	}

    bool ipc_data::initialized() const noexcept {
        return buff_ptr_ != nullptr;
    }

    void* ipc_data::data() const noexcept {
        return buff_ptr_;
    }

	std::size_t ipc_data::size() const noexcept {
        return buffer_size_;
    }

	std::size_t ipc_data::page_size() noexcept {
        return sysconf(_SC_PAGESIZE);
    }
	    
	void ipc_data::create(std::size_t buffer_size) {
        if (buff_ptr_ != nullptr)
        {
            throw std::runtime_error("Resource already initialized!");
        }

		//open file descriptor
		const int fd = shm_open(resource_name_, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (fd == -1)
		{
			throw std::system_error{
				std::make_error_code(std::errc::io_error),
					"Could not create file mapping object!"
			};
		}
		fd_ = fd;

		//set size
		const int res = ftruncate(fd_, buffer_size);
		if (res == -1)
		{
			throw std::system_error{
				std::make_error_code(std::errc::io_error),
					"Could not create file mapping object!"
			};
		}

		//map mem
		buff_ptr_ = mmap(NULL, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
		if (buff_ptr_ == MAP_FAILED)
		{
			buff_ptr_ = nullptr;
			throw std::system_error{
				std::make_error_code(std::errc::io_error),
					"Could not create file mapping object!"
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
        if (buff_ptr_ != nullptr)
        {
            throw std::runtime_error("Resource already opened!");
        }
		if (fd_ == -1) {
			const int fd = shm_open(resource_name_, O_RDWR , S_IRUSR | S_IWUSR);
			if (fd == -1)
			{
				throw std::system_error{
					std::make_error_code(std::errc::io_error),
						"Could not create file mapping object!"
				};
			}
			fd_ = fd;
		}
		//map mem
		buff_ptr_ = mmap(NULL, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
		if (buff_ptr_ == MAP_FAILED)
		{
			buff_ptr_ = nullptr;
			throw std::system_error{
				std::make_error_code(std::errc::io_error),
					"Could not create file mapping object!"
			};
		}
        buffer_size_ = buffer_size;
    }

    void ipc_data::close() noexcept {
        if (buff_ptr_ != nullptr)
        {
			munmap(buff_ptr_, buffer_size_);
        }
        buff_ptr_ = nullptr;
        buffer_size_ = 0;
    }

    void ipc_data::lock_mem_to_phys() {
		mlock(buff_ptr_, buffer_size_);
    }
}
#endif
