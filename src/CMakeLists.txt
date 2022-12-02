cmake_minimum_required(VERSION 3.0)
project ( tdserver )

add_executable(tdserver 
		main.cpp
		)
target_link_libraries(server OpenSSL::SSL)
target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})