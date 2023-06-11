/*
 * 
 * Copyright 2023 Rishi Challa
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#pragma once

//
// Basic Logger Options:
// 

// set(LoggingMacro "blLog") # Should be set in CMake, provides the name for the logging macro
// set(LoggingInfoMacro "blInfo") # Should be set in CMake, provides the name for the logging macro
// set(LoggingWarnMacro "blWarn") # Should be set in CMake, provides the name for the logging macro
// set(LoggingErrorMacro "blError") # Should be set in CMake, provides the name for the logging macro

// #define BASIC_LOGGER_ENABLE_FILE // Enables logging to log files
// #define BASIC_LOGGER_ENABLE_CONSOLE // Enables logging to console/standard output
// #define BASIC_LOGGER_ENABLE_MULTITHREADING // Enabled thread safety

#ifndef BASIC_LOGGER_LOGGING_FOLDER
#define BASIC_LOGGER_LOGGING_FOLDER "logs/" // The folder to place logs into
#endif
#ifndef BASIC_LOGGER_FILE_FORMAT
#define BASIC_LOGGER_FILE_FORMAT "{:%F---%H-%M-%S.log}" // Format of file names, given to std::format with zoned time
#endif

#include <cstring>
#include <sstream>
#include <string>
#include <string_view>
#include <format>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <mutex>

/**
 * The base filename/relative file path without the source path - https://stackoverflow.com/a/8488201
 */
#ifdef BASIC_LOGGER_WINDOWS
#define __BASIC_LOGGER_FILENAME__ (strrchr("\\" __FILE__, '\\') + 1)
#else
#define __BASIC_LOGGER_FILENAME__ (strrchr("/" __FILE__, '/') + 1)
#endif

// Hardcoded logging macros, custom logging macros are configurable from CMake
#define BASIC_LOGGER_LOG_FUNCTION(x) BasicLogger::log<decltype(x)>(__LINE__, __BASIC_LOGGER_FILENAME__, x)
#define BASIC_LOGGER_INFO_FUNCTION(x) BasicLogger::info<decltype(x)>(__LINE__, __BASIC_LOGGER_FILENAME__, x)
#define BASIC_LOGGER_WARN_FUNCTION(x) BasicLogger::warn<decltype(x)>(__LINE__, __BASIC_LOGGER_FILENAME__, x)
#define BASIC_LOGGER_ERROR_FUNCTION(x) BasicLogger::error<decltype(x)>(__LINE__, __BASIC_LOGGER_FILENAME__, x)
#define BASIC_LOGGER_ASSERT_FUNCTION(x, y) BasicLogger::assert<decltype(y)>(x, __LINE__, __BASIC_LOGGER_FILENAME__, y)

/**
 * Basic Logger Class, in most cases you want to use macros to log rather than calling this class directly.
 */
class BasicLogger {
private:

#ifdef BASIC_LOGGER_ENABLE_FILE
	static inline std::ofstream logFile;
#endif

#ifdef BASIC_LOGGER_ENABLE_MULTITHREADING
	static inline std::mutex logMutex;
#endif

public:

	/**
	 * Initializes the logging library
	 */
	static void init() {
#ifdef BASIC_LOGGER_ENABLE_FILE
		auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		std::string logFolder(BASIC_LOGGER_LOGGING_FOLDER);
		std::string logFileName(std::format(BASIC_LOGGER_FILE_FORMAT, std::chrono::zoned_time{std::chrono::current_zone(), timestamp}));
		if (!std::filesystem::is_directory(logFolder) || !std::filesystem::exists(logFolder)) std::filesystem::create_directory(logFolder);
		logFile.open(logFolder + logFileName);
#endif
	}

	/**
	 * Logs a message without severity
	 * 
	 * @tparam T Type of logged message
	 * @param message The message
	 */
	template <typename T> static void log(const T& message) {
#if defined(BASIC_LOGGER_ENABLE_CONSOLE) || defined(BASIC_LOGGER_ENABLE_FILE)
		auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		std::string formattedTime = std::format("{:%I:%M %p}", std::chrono::zoned_time{std::chrono::current_zone(), timestamp});
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::stringstream consoleMessage;
		consoleMessage << "\033[38;5;241m[\033[38;5;87m"
			<< formattedTime
			<< "\033[38;5;241m] \033[38;5;249m: "
			<< message
			<< "\033[0m";
		std::string consoleMessageStr = consoleMessage.str();
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		std::stringstream fileMessage;
		fileMessage << "[" << formattedTime << "] : " << message;
		std::string fileMessageStr = fileMessage.str();
#endif

#ifdef BASIC_LOGGER_ENABLE_MULTITHREADING
		std::lock_guard lock(logMutex);
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::cout << consoleMessageStr << std::endl;
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		logFile << fileMessageStr << std::endl;
#endif
	}

	/**
	 * Logs a message without severity
	 * 
	 * @tparam T Type of logged message
	 * @param lineNumber The line number printed from
	 * @param filename The file name printed from
	 * @param message The message
	 */
	template <typename T> static void log(int lineNumber, std::string_view filename, const T& message) {
#if defined(BASIC_LOGGER_ENABLE_CONSOLE) || defined(BASIC_LOGGER_ENABLE_FILE)
		auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		std::string formattedTime = std::format("{:%I:%M %p}", std::chrono::zoned_time{std::chrono::current_zone(), timestamp});
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::stringstream consoleMessage;
		consoleMessage << "\033[38;5;241m[\033[38;5;87m"
			<< formattedTime
			<< "\033[38;5;241m] \033[38;5;241m[\033[38;5;85m"
			<< filename
			<< "\033[38;5;241m:\033[38;5;86m"
			<< lineNumber
			<< "\033[38;5;241m] \033[38;5;249m: "
			<< message
			<< "\033[0m";
		std::string consoleMessageStr = consoleMessage.str();
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		std::stringstream fileMessage;
		fileMessage << "[" << formattedTime << "] [" << filename << ":" << lineNumber << "] : " << message;
		std::string fileMessageStr = fileMessage.str();
#endif

#ifdef BASIC_LOGGER_ENABLE_MULTITHREADING
		std::lock_guard lock(logMutex);
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::cout << consoleMessageStr << std::endl;
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		logFile << fileMessageStr << std::endl;
#endif
	}
	
	/**
	 * Logs a message with info severity
	 * 
	 * @tparam T Type of logged message
	 * @param message The message
	 */
	template <typename T> static void info(const T& message) {
#if defined(BASIC_LOGGER_ENABLE_CONSOLE) || defined(BASIC_LOGGER_ENABLE_FILE)
		auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		std::string formattedTime = std::format("{:%I:%M %p}", std::chrono::zoned_time{std::chrono::current_zone(), timestamp});
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::stringstream consoleMessage;
		consoleMessage << "\033[38;5;241m[\033[38;5;87m"
			<< formattedTime
			<< "\033[38;5;241m] [\033[38;5;116mINFO\033[38;5;241m] \033[38;5;249m: \033[38;5;116m"
			<< message
			<< "\033[0m";
		std::string consoleMessageStr = consoleMessage.str();
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		std::stringstream fileMessage;
		fileMessage << "[" << formattedTime << "] [INFO] : " << message;
		std::string fileMessageStr = fileMessage.str();
#endif

#ifdef BASIC_LOGGER_ENABLE_MULTITHREADING
		std::lock_guard lock(logMutex);
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::cout << consoleMessageStr << std::endl;
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		logFile << fileMessageStr << std::endl;
#endif
	}

	/**
	 * Logs a message with info severity
	 * 
	 * @tparam T Type of logged message
	 * @param lineNumber The line number printed from
	 * @param filename The file name printed from
	 * @param message The message
	 */
	template <typename T> static void info(int lineNumber, std::string_view filename, const T& message) {
#if defined(BASIC_LOGGER_ENABLE_CONSOLE) || defined(BASIC_LOGGER_ENABLE_FILE)
		auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		std::string formattedTime = std::format("{:%I:%M %p}", std::chrono::zoned_time{std::chrono::current_zone(), timestamp});
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::stringstream consoleMessage;
		consoleMessage << "\033[38;5;241m[\033[38;5;87m"
			<< formattedTime
			<< "\033[38;5;241m] \033[38;5;241m[\033[38;5;85m"
			<< filename
			<< "\033[38;5;241m:\033[38;5;86m"
			<< lineNumber
			<< "\033[38;5;241m] [\033[38;5;116mINFO\033[38;5;241m] \033[38;5;249m: \033[38;5;116m"
			<< message
			<< "\033[0m";
		std::string consoleMessageStr = consoleMessage.str();
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		std::stringstream fileMessage;
		fileMessage << "[" << formattedTime << "] [" << filename << ":" << lineNumber << "] [INFO] : " << message;
		std::string fileMessageStr = fileMessage.str();
#endif

#ifdef BASIC_LOGGER_ENABLE_MULTITHREADING
		std::lock_guard lock(logMutex);
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::cout << consoleMessageStr << std::endl;
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		logFile << fileMessageStr << std::endl;
#endif
	}

	/**
	 * Logs a message with warning severity
	 * 
	 * @tparam T Type of logged message
	 * @param message The message
	 */
	template <typename T> static void warn(const T& message) {
#if defined(BASIC_LOGGER_ENABLE_CONSOLE) || defined(BASIC_LOGGER_ENABLE_FILE)
		auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		std::string formattedTime = std::format("{:%I:%M %p}", std::chrono::zoned_time{std::chrono::current_zone(), timestamp});
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::stringstream consoleMessage;
		consoleMessage << "\033[38;5;241m[\033[38;5;87m"
			<< formattedTime
			<< "\033[38;5;241m] [\033[38;5;220mWARNING\033[38;5;241m] \033[38;5;249m: \033[38;5;220m"
			<< message
			<< "\033[0m";
		std::string consoleMessageStr = consoleMessage.str();
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		std::stringstream fileMessage;
		fileMessage << "[" << formattedTime << "] [WARNING] : " << message;
		std::string fileMessageStr = fileMessage.str();
#endif

#ifdef BASIC_LOGGER_ENABLE_MULTITHREADING
		std::lock_guard lock(logMutex);
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::cout << consoleMessageStr << std::endl;
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		logFile << fileMessageStr << std::endl;
#endif
	}

	/**
	 * Logs a message with warning severity
	 * 
	 * @tparam T Type of logged message
	 * @param lineNumber The line number printed from
	 * @param filename The file name printed from
	 * @param message The message
	 */
	template <typename T> static void warn(int lineNumber, std::string_view filename, const T& message) {
#if defined(BASIC_LOGGER_ENABLE_CONSOLE) || defined(BASIC_LOGGER_ENABLE_FILE)
		auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		std::string formattedTime = std::format("{:%I:%M %p}", std::chrono::zoned_time{std::chrono::current_zone(), timestamp});
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::stringstream consoleMessage;
		consoleMessage << "\033[38;5;241m[\033[38;5;87m"
			<< formattedTime
			<< "\033[38;5;241m] \033[38;5;241m[\033[38;5;85m"
			<< filename
			<< "\033[38;5;241m:\033[38;5;86m"
			<< lineNumber
			<< "\033[38;5;241m] [\033[38;5;220mWARNING\033[38;5;241m] \033[38;5;249m: \033[38;5;220m"
			<< message
			<< "\033[0m";
		std::string consoleMessageStr = consoleMessage.str();
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		std::stringstream fileMessage;
		fileMessage << "[" << formattedTime << "] [" << filename << ":" << lineNumber << "] [WARNING] : " << message;
		std::string fileMessageStr = fileMessage.str();
#endif

#ifdef BASIC_LOGGER_ENABLE_MULTITHREADING
		std::lock_guard lock(logMutex);
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::cout << consoleMessageStr << std::endl;
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		logFile << fileMessageStr << std::endl;
#endif
	}

	/**
	 * Logs a message with error severity
	 * 
	 * @tparam T Type of logged message
	 * @param message The message
	 */
	template <typename T> static void error(const T& message) {
#if defined(BASIC_LOGGER_ENABLE_CONSOLE) || defined(BASIC_LOGGER_ENABLE_FILE)
		auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		std::string formattedTime = std::format("{:%I:%M %p}", std::chrono::zoned_time{std::chrono::current_zone(), timestamp});
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::stringstream consoleMessage;
		consoleMessage << "\033[38;5;241m[\033[38;5;87m"
			<< formattedTime
			<< "\033[38;5;241m] [\033[38;5;198mERROR\033[38;5;241m] \033[38;5;249m: \033[38;5;198m"
			<< message
			<< "\033[0m";
		std::string consoleMessageStr = consoleMessage.str();
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		std::stringstream fileMessage;
		fileMessage << "[" << formattedTime << "] [ERROR] : " << message;
		std::string fileMessageStr = fileMessage.str();
#endif

#ifdef BASIC_LOGGER_ENABLE_MULTITHREADING
		std::lock_guard lock(logMutex);
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::cout << consoleMessageStr << std::endl;
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		logFile << fileMessageStr << std::endl;
#endif
	}

	/**
	 * Logs a message with error severity
	 * 
	 * @tparam T Type of logged message
	 * @param lineNumber The line number printed from
	 * @param filename The file name printed from
	 * @param message The message
	 */
	template <typename T> static void error(int lineNumber, std::string_view filename, const T& message) {
#if defined(BASIC_LOGGER_ENABLE_CONSOLE) || defined(BASIC_LOGGER_ENABLE_FILE)
		auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
		std::string formattedTime = std::format("{:%I:%M %p}", std::chrono::zoned_time{std::chrono::current_zone(), timestamp});
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::stringstream consoleMessage;
		consoleMessage << "\033[38;5;241m[\033[38;5;87m"
			<< formattedTime
			<< "\033[38;5;241m] \033[38;5;241m[\033[38;5;85m"
			<< filename
			<< "\033[38;5;241m:\033[38;5;86m"
			<< lineNumber
			<< "\033[38;5;241m] [\033[38;5;198mERROR\033[38;5;241m] \033[38;5;249m: \033[38;5;198m"
			<< message
			<< "\033[0m";
		std::string consoleMessageStr = consoleMessage.str();
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		std::stringstream fileMessage;
		fileMessage << "[" << formattedTime << "] [" << filename << ":" << lineNumber << "] [ERROR] : " << message;
		std::string fileMessageStr = fileMessage.str();
#endif

#ifdef BASIC_LOGGER_ENABLE_MULTITHREADING
		std::lock_guard lock(logMutex);
#endif

#ifdef BASIC_LOGGER_ENABLE_CONSOLE
		std::cout << consoleMessageStr << std::endl;
#endif
#ifdef BASIC_LOGGER_ENABLE_FILE
		logFile << fileMessageStr << std::endl;
#endif
	}

	/**
	 * Asserts a condition or statement being true/truthy
	 * 
	 * @tparam T The type of message logged if the assertion should fail
	 * @param assertion The assertion which to check,
	 * @param message The message to log if the assertion should fail
	 */
	template <typename T> static void assert(bool assertion, const T& message) {
		if (assertion) return;
		std::stringstream stream;
		stream << "Assertion Failed - " << message;
		error(stream.str());
	}

	/**
	 * Asserts a condition or statement being true/truthy
	 * 
	 * @tparam T The type of message logged if the assertion should fail
	 * @param assertion The assertion which to check,
	 * @param lineNumber The line number this assertion was called from
	 * @param filename The filename this assertion was called from
	 * @param message The message to log if the assertion should fail
	 */
	template <typename T> static void assert(bool assertion, int lineNumber, std::string_view filename, const T& message) {
		if (assertion) return;
		std::stringstream stream;
		stream << "Assertion Failed - " << message;
		error(lineNumber, filename, stream.str());
	}
};
