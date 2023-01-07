cmake_minimum_required(VERSION 3.0)
project ( tdserver )

add_executable(tdserver 
		./source/main.cpp
		./source/system.cpp
		./source/server.cpp
		./source/queryBuilder.cpp
		./source/fileHandler.cpp
		./source/logger.cpp
		./source/databaseConnector.cpp
		)

target_link_libraries(tdserver OpenSSL::SSL)
target_link_libraries(tdserver ${Boost_LIBRARIES})
target_link_libraries(tdserver ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(tdserver mysqlcppconn)
