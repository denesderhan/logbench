//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once

#ifndef LOGBENCH_API
#if defined(LOGBENCH_SHARED)
	#if defined(_WIN32)
		#if defined(LOGBENCH_EXPORT)
			#define LOGBENCH_API __declspec(dllexport)
		#else
			#define LOGBENCH_API __declspec(dllimport)
		#endif
	#elif defined(__GNUC__) || defined(__clang__)
		#define LOGBENCH_API __attribute__((visibility("default")))
	#else
		#define LOGBENCH_API
	#endif
#else
	#define LOGBENCH_API
#endif
#endif
