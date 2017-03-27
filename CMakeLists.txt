# CMake likes us to put this here in case someone tries to read this file with an older version of cmake
cmake_minimum_required(VERSION 2.8) # http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:cmake_minimum_required

project ("web_ui_extension")

option ( USE_CXX_11_ABI "cxx_11 abi" off )

#if ( ${USE_CXX_11_ABI} )
#    SET( CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_CXX11_ABI=0 " )
#  endif()
#  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )

SET( CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_CXX11_ABI=0 -std=c++14" )
SET( CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}  -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -g )
SET( CMAKE_EXPORT_COMPILE_COMMANDS on )

# Other Libraries
find_package( Boost COMPONENTS system thread signals)
find_library( wt wt)
find_library( wthttp wthttp)

add_library( web_ui SHARED web_ui.cpp CommandExecutingButton.cpp ssh_authentication.cpp ssh_remote_execution.cpp SystemTab.cpp ThinclientTab.cpp MonitoringTab.cpp )


target_link_libraries( web_ui
		      ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SIGNALS_LIBRARY}
		      wt wthttp ssh2)


		    add_executable( bin_example main.cpp  )

target_link_libraries( bin_example web_ui )
