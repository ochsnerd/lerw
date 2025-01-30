cmake_minimum_required (VERSION 3.6)

project(lerw)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/include)
# including gtl as a system dependency prevents warnings when compiling it
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/gtl/include)

set(COMMON_SOURCES
    src/config.cpp
)

add_executable(lerw_length src/main_length.cpp ${COMMON_SOURCES})
add_executable(lerw_points src/main_points.cpp ${COMMON_SOURCES})

foreach(target lerw_length lerw_points)
# options from https://github.com/cpp-best-practices/cmake_template
target_compile_options(${target} PUBLIC
			    -Wall
  			    -Wextra # reasonable and standard
       			    -Wshadow # warn the user if a variable declaration shadows one from a parent context
       			    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps
       			    # catch hard to track down memory errors
        		    -Wold-style-cast # warn for c-style casts
        		    -Wcast-align # warn for potential performance problem casts
        		    -Wunused # warn on anything being unused
        		    -Woverloaded-virtual # warn if you overload (not override) a virtual function
        		    -Wpedantic # warn if non-standard C++ is used
        		    -Wconversion # warn on type conversions that may lose data
        		    -Wsign-conversion # warn on sign conversions
        		    -Wnull-dereference # warn if a null dereference is detected
        		    -Wdouble-promotion # warn if float is implicit promoted to double
        		    -Wformat=2 # warn on security issues around functions that format output (ie printf)
			    -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation-Wall -Wextra -Wshadow -Wpedantic)

			    # gcc-specific options
			    -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
        		    -Wduplicated-cond # warn if if / else chain has duplicated conditions
			    -Wduplicated-branches # warn if if / else branches have duplicated code
        		    -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
        		    -Wuseless-cast # warn if you perform a cast to the same type
        		    -Wsuggest-override # warn if an overridden member function is not marked 'override' or 'final'
			    )

target_compile_options(${target} PUBLIC -O3 -march=native)
target_compile_options(${target} PUBLIC -fconcepts-diagnostics-depth=4)

# silence warnings about portability of the gtl
# https://gcc.gnu.org/onlinedocs/gcc-14.1.0/gcc/Warning-Options.html#index-Winterference-size
target_compile_options(${target} PUBLIC -Wno-interference-size)

# silence notes from the pstl-implementation
# https://stackoverflow.com/a/23995391
target_compile_options(${target} PUBLIC -fcompare-debug-second)

target_link_libraries(${target} PRIVATE
  tbb
  boost_program_options)
endforeach()

# testing
find_package(Catch2 3 REQUIRED)
add_executable(tests
	tests/stopper.cpp
	tests/utils.cpp
	tests/generator.cpp
	tests/ldstepper.cpp
	tests/points.cpp
	tests/distributions.cpp
	tests/directions.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

target_compile_options(tests PUBLIC -fconcepts-diagnostics-depth=4)

include(CTest)
include(Catch)
catch_discover_tests(tests)

# nix-build wants an 'install' target
install(TARGETS lerw_length lerw_points DESTINATION .)
