cmake_minimum_required(VERSION 3.0.0)
project(BasicLogger VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(BasicLogger INTERFACE)
target_include_directories(BasicLogger INTERFACE include/)

# Name of logging macros
set(LoggingMacro "blLog" CACHE STRING "The name of the Logging Macro")
set(LoggingInfoMacro "blInfo" CACHE STRING "The name of the Logging Info Macro")
set(LoggingWarnMacro "blWarn" CACHE STRING "The name of the Logging Warn Macro")
set(LoggingErrorMacro "blError" CACHE STRING "The name of the Logging Error Macro")
set(LoggingAssertMacro "blAssert" CACHE STRING "The name of the Logging Assertion Macro")

# Logging Macros
target_compile_definitions(BasicLogger INTERFACE ${LoggingMacro}=BASIC_LOGGER_LOG_FUNCTION)
target_compile_definitions(BasicLogger INTERFACE ${LoggingInfoMacro}=BASIC_LOGGER_INFO_FUNCTION)
target_compile_definitions(BasicLogger INTERFACE ${LoggingWarnMacro}=BASIC_LOGGER_WARN_FUNCTION)
target_compile_definitions(BasicLogger INTERFACE ${LoggingErrorMacro}=BASIC_LOGGER_ERROR_FUNCTION)
target_compile_definitions(BasicLogger INTERFACE ${LoggingAssertMacro}=BASIC_LOGGER_ASSERT_FUNCTION)

# OS Macro
if(WIN32)
	target_compile_definitions(BasicLogger INTERFACE BASIC_LOGGER_WINDOWS)
endif()
