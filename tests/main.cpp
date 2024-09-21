//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#define CATCH_CONFIG_RUNNER
#include <catch2/catch_all.hpp>

int main(int argc, char* argv[]) {
	// global setup...
	//SetConsoleOutputCP(CP_UTF8);

	int result = Catch::Session().run(argc, argv);

	// global clean-up...

	return result;
}

