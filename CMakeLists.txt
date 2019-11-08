cmake_minimum_required (VERSION 3.8)

option(FIRDN_FILTER2C "Add C-code generator" OFF)

project (firdn
	LANGUAGES CXX
	)

add_executable(test_firdn
	test.cpp
	firdn.hpp
	)
target_compile_features(test_firdn PUBLIC cxx_std_14)

add_executable(downsample
	downsample.cpp
	firdn.hpp
)
target_compile_features(test_firdn PUBLIC cxx_std_14)
