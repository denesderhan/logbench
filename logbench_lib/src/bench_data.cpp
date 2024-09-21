//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <logbench/bench_data.hpp>

#include <cstddef>
#include <cstdint>
#include <cassert>

#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>
#include <logbench/test_result_thread.hpp>

#if defined(_WIN32)
#pragma intrinsic(memcpy)
#include <ipc_data_win.hpp>
#else
#include <ipc_data_posix.hpp>
#endif

namespace logbench {
	bench_data::bench_data() noexcept = default;
	bench_data::~bench_data() noexcept = default;
	
	//not thread safe (can only be called once and first)
	void bench_data::create(test_in_param const& data) {
		thread_num_ = data.thread_num;
		log_num_ = data.log_call_per_thread;
		latencies_ = data.save_latency;
		page_size_ = ipc_buffer().page_size();
		set_data_sizes();
		ipc_buffer().create(test_data_size_ + thread_data_size_ * thread_num_);
		memcpy(ipc_buffer().data(), &data, sizeof(test_in_param));
	}

	void bench_data::remove() noexcept {
		ipc_buffer().remove();
	}

	void bench_data::open() {
		ipc_buffer().open(sizeof(test_in_param));
		test_in_param data;
		get_data(data);
		ipc_buffer().close();
		thread_num_ = data.thread_num;
		log_num_ = data.log_call_per_thread;
		latencies_ = data.save_latency;
		page_size_ = ipc_buffer().page_size();
		set_data_sizes();
		ipc_buffer().open(test_data_size_ + thread_data_size_ * thread_num_);
	}

	void bench_data::set_data_sizes() noexcept {
		std::size_t test_data = sizeof(test_in_param) + sizeof(test_out_param);
		test_data_size_ = ((test_data / page_size_) + 1 ) * page_size_;
		if (latencies_) {
			std::size_t thread_unpadded_size = sizeof(test_result_thread) + ((sizeof(std::uint64_t) * 2 + sizeof(std::uint32_t) * 2) * log_num_);
			thread_data_size_ = ((thread_unpadded_size / page_size_) + 1) * page_size_;
		}
		else {
			thread_data_size_ = ((sizeof(test_result_thread) / page_size_) + 1) * page_size_;
		}
	}
	
	void bench_data::get_data(test_in_param& data) const noexcept {
		assert(ipc_buffer().initialized());
		memcpy(
			&data, 
			ipc_buffer().data(), 
			sizeof(test_in_param)
		);
	}
	void bench_data::set_data(test_out_param const& data) noexcept {
		assert(ipc_buffer().initialized());
		memcpy(
			static_cast<char*>(ipc_buffer().data()) + sizeof(test_in_param),
			&data,
			sizeof(test_out_param)
		);
	}
	void bench_data::get_data(test_out_param& data) const noexcept {
		assert(ipc_buffer().initialized());
		memcpy(
			&data,
			static_cast<char*>(ipc_buffer().data()) + sizeof(test_in_param),
			sizeof(test_out_param)
		);
	}
	void bench_data::set_data(unsigned int thread_index, test_result_thread const& data) noexcept {
		assert(ipc_buffer().initialized());
		memcpy(
			thread_data(thread_index), 
			&data, 
			sizeof(test_result_thread)
		);
	}
	void bench_data::get_data(unsigned int thread_index, test_result_thread& data) const noexcept{
		assert(ipc_buffer().initialized());
		memcpy(
			&data, 
			thread_data(thread_index), 
			sizeof(test_result_thread)
		);
	}

	char* bench_data::latency_data(unsigned int thread_index) const noexcept{
		return thread_data(thread_index) + sizeof(test_result_thread);
	}

	bool bench_data::initialized() const
	{
		return ipc_buffer().initialized();
	}

	void bench_data::clear_result()
	{
		assert(ipc_buffer().initialized());
		std::memset(
			static_cast<char*>(ipc_buffer().data()) + sizeof(test_in_param),
			0,
			sizeof(test_out_param)
		);
		for (unsigned int t = 0; t < thread_num_; t++) {
			std::memset(
				thread_data(t),
				0,
				thread_data_size_
			);
		}
	}

	char* bench_data::thread_data(unsigned int thread_index) const noexcept{
		assert(thread_index < thread_num_);
		auto thread_data_ptr = 
			reinterpret_cast<char*>(ipc_buffer().data())
			+ test_data_size_
			+ thread_data_size_ * thread_index;
		return thread_data_ptr;
	}

	ipc_data& bench_data::ipc_buffer() {
		static ipc_data ipc_buffer;
		return ipc_buffer;
	}

	double bench_data::avg_latency_tsc() const {
		std::uint64_t sum_lat{ 0 };
		std::uint64_t sum_log{ 0 };
		for (unsigned int t = 0; t < thread_num_; t++) {
			logbench::test_result_thread t_data;
			get_data(t, t_data);
			// max latency 1'000'000'000 (1sec) max log num : 1'000'000, max threads: 1024 is less than UINT64MAX
			sum_lat += t_data.sum_latency;
			sum_log += t_data.log_num;
		}
		if (sum_log == 0) {
			return 0.0;
		}
		else {
			return double(sum_lat) / double(sum_log);
		}
	}

	std::vector<double> bench_data::get_latencies() const {
		std::vector<double> latencies;
		std::size_t dat_size{ std::size_t(thread_num_) * log_num_ };
		if (dat_size != 0) {
			test_out_param op;
			get_data(op);
			const auto rdtsc_f{ op.rdtsc_freq };
			latencies.reserve(dat_size);
			for (unsigned int t = 0; t < thread_num_; t++) {
				logbench::test_result_thread thr_data;
				get_data(t, thr_data);
				auto dat_ptr = latency_data(t);
				for (int l = 0; l < thr_data.log_num; l++) {
					std::uint64_t start;
					memcpy(
						&start,
						dat_ptr,
						sizeof(std::uint64_t)
					);
					dat_ptr += sizeof(std::uint64_t);
					std::uint64_t stop;
					memcpy(
						&stop,
						dat_ptr,
						sizeof(std::uint64_t)
					);
					dat_ptr += sizeof(std::uint64_t) + 2 * sizeof(std::uint32_t);
					double lat = double(stop - start) / rdtsc_f;
					latencies.push_back(lat);
				}
			}
		}
		return latencies;
	}

	static_assert(sizeof(test_result_thread) % 8 == 0);
}
