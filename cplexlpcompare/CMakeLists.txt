
add_executable (cplexlpcompare lpcompare.cpp Bound.cpp Constraint.cpp LPModel.cpp Split.cpp Term.cpp) 

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

add_definitions("-DBOOST_ALL_NO_LIB")

math(EXPR PLATFORM_BITS "8*${CMAKE_SIZEOF_VOID_P}")

if(PLATFORM_BITS STREQUAL 32)
	message(WARNING "64-bit build is suggested for working with large LP files.")
endif(PLATFORM_BITS STREQUAL 32)

message(WARNING "CMake cannot universally detect if boost address mode is 32-bit of 64-bit. Program will not compile if there is a mismatch between the boost libs and cplexlpcompare project.")

find_package(Boost 1.55.0 REQUIRED COMPONENTS iostreams regex program_options filesystem system) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})     
	target_link_libraries(cplexlpcompare ${Boost_LIBRARIES})
endif()

include_directories (${LPCOMPARE_SOURCE_DIR}/cplexlpcompare) 
