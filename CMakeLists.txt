cmake_minimum_required( VERSION 3.6 )

project( bryllite-platform )

function( trace msg )
	string( ASCII 27 Esc )
	set( CLEAR "${Esc}[m")
	set( LightGreen "${Esc}[1;32m" )

	message( STATUS "${LightGreen}${msg}${CLEAR}" )
endfunction( trace )

set( VERSION_MAJOR 0 )
set( VERSION_MINOR 1 )
set( VERSION_PATCH 0 )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_EXTENSIONS ON )
set( CXX_STANDARD_REQUIRED ON )
set( CMAKE_VERBOSE_MAKEFILE true )

# folder names
set( BRC_APPS_NAME apps )
set( BRC_LIBS_NAME libs )
set( BRC_BIN_NAME bin )
set( BRC_BUILD_NAME build )
set( BRC_TEST_NAME tests )
set( BRC_COMMON_NAME bryllite-common )
set( BRC_CORE_NAME bryllite-core )

set( BRC_ROOT_PATH ${PROJECT_SOURCE_DIR} )
set( BRC_LIBS_PATH ${BRC_ROOT_PATH}/${BRC_LIBS_NAME} )
set( BRC_APPS_PATH ${BRC_ROOT_PATH}/${BRC_APPS_NAME} )
set( BRC_BUILD_PATH ${BRC_ROOT_PATH}/${BRC_BUILD_NAME} )
set( BRC_BIN_PATH ${BRC_ROOT_PATH}/${BRC_BIN_NAME} )
set( BRC_TEST_PATH ${BRC_ROOT_PATH}/${BRC_TEST_NAME} )
set( BRC_COMMON_PATH ${BRC_ROOT_PATH}/${BRC_COMMON_NAME} )
set( BRC_CORE_PATH ${BRC_ROOT_PATH}/${BRC_CORE_NAME} )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BRC_BIN_PATH} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BRC_LIBS_PATH} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BRC_LIBS_PATH} )

#set(CMAKE_CXX_FLAGS_RELEASE "/MT")
#set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
#set(CMAKE_C_FLAGS_RELEASE "/MT")
#set(CMAKE_C_FLAGS_DEBUG "/MTd")

configure_file(
	${BRC_COMMON_PATH}/version.hpp.in
	${BRC_COMMON_PATH}/version.hpp
)

# Boost 1.66+ required
set( Boost_USE_STATIC_LIBS ON )
set( Boost_USE_MULTITHREAD ON )
find_package( Boost 1.66 REQUIRED COMPONENTS
	system
	filesystem
	date_time
	regex
	program_options
	unit_test_framework
)
if( Boost_FOUND )
	include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
	add_definitions( -DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE )
endif()

# OpenSSL 1.1.0c+ required
set( OPENSSL_USE_STATIC_LIBS FALSE )
set( OPENSSL_MSVC_STATIC_RT FALSE )
find_package( OpenSSL REQUIRED )
if ( OPENSSL_FOUND )
	include_directories( ${OPENSSL_INCLUDE_DIR} )
endif()

if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0501)
	add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
endif(WIN32)

# include header path
include_directories( ${BRC_COMMON_NAME} )
include_directories( ${BRC_CORE_NAME} )

link_directories( ${BRC_LIBS_PATH} )

# subdirectories
#add_subdirectory( ${BRC_COMMON_NAME} )
#add_subdirectory( ${BRC_CORE_NAME} )
add_subdirectory( ${BRC_APPS_NAME} )
add_subdirectory( ${BRC_TEST_NAME} )
