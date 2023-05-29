cmake_minimum_required(VERSION 3.0.0)
project(BasicLogger VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(BasicLogger INTERFACE)
target_include_directories(BasicLogger INTERFACE include/)

# Name of logging macros
set(LoggingMacro "blLog")
set(LoggingInfoMacro "blInfo")
set(LoggingWarnMacro "blWarn")
set(LoggingErrorMacro "blError")

# Logging Macros
target_compile_definitions(BasicLogger INTERFACE ${LoggingMacro}=BASIC_LOGGER_LOG_FUNCTION)
target_compile_definitions(BasicLogger INTERFACE ${LoggingInfoMacro}=BASIC_LOGGER_INFO_FUNCTION)
target_compile_definitions(BasicLogger INTERFACE ${LoggingWarnMacro}=BASIC_LOGGER_WARN_FUNCTION)
target_compile_definitions(BasicLogger INTERFACE ${LoggingErrorMacro}=BASIC_LOGGER_ERROR_FUNCTION)

# OS Macro
if(WIN32)
	target_compile_definitions(BasicLogger INTERFACE BASIC_LOGGER_WINDOWS)
endif()
