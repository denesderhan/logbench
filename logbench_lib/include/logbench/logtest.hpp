//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once

#include <stdexcept>

#include <logbench/version.hpp>
#include <logbench/bench_data.hpp>
#include <logbench/timestamp_nano.hpp>
#include <logbench/pin_thread.hpp>

namespace logbench {
    template<class logger>
    class logtest {
    protected:
        logtest() = delete;
        logtest(const char* test_name) {
            try {
                bench_data_.open();
                bench_data_.get_data(test_data_);
                //FIX THIS fill out_data_ !!! from logger
                bench_data_.get_data(out_data_);
				if (test_data_.bench_version != 
					std::string_view{ LOGBENCHLIB_VERSION })
                {
                    throw std::runtime_error(
                        "Logbench library versions mismatch! ver: " LOGBENCHLIB_VERSION);
                }
                //auto& self = reinterpret_cast<test<logger>>(*this);
				out_data_.test_name = std::string_view{ test_name };

                logger::sys_init(test_data_, out_data_);
                pin_thread::set_pins(test_data_.pin_to_cores);

#ifdef NDEBUG
                out_data_.debug_build = false;
#else
                out_data_.debug_build = true;
#endif
                //default if derived test does not call it
                set_begin_logging_time();
            }
            catch (const std::exception& ex) {
                if (bench_data_.initialized()) {
					out_data_.out_message = std::string_view{ ex.what() };
                    bench_data_.set_data(out_data_);
                }
                throw ex;
            }
        }

    public:

        void set_begin_logging_time() noexcept {
            out_data_.logging_begin_time = timestamp_nano<true>();
        }
        
        ~logtest() {
            logger::sys_stop();
            bench_data_.set_data(out_data_);
        }
       
        bench_data bench_data_;
        test_in_param test_data_;
        test_out_param out_data_;
    };
}
