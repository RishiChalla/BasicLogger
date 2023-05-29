# Basic Logger
A single file header library for basic logging necessities. Provides ANSI color output for standard out and log file logging. Options and usage shown below.

> Note - This library makes heavy use of C++20 features such as `std::filesystem`, `std::format`, and timezone related `std::chrono` operations. Ensure you use C++20.

## Usage
To use the library, simply add it using `add_subdirectory` with cmake:
```cmake
add_subdirectory(dependencies/BasicLogger) # In this case the library is cloned under `dependencies/BasicLogger`
target_link_libraries(TargetName BasicLogger)
target_compile_definitions(TargetName PUBLIC BASIC_LOGGER_ENABLE_FILE) # Enables logging to log files
target_compile_definitions(TargetName PUBLIC BASIC_LOGGER_ENABLE_CONSOLE) # Enables logging to standard output
target_compile_definitions(TargetName PUBLIC BASIC_LOGGER_ENABLE_MULTITHREADING) # Enables thread safety
```

To log something, simply initialize the library then use a logging macro or log function:
```C++

#include <BasicLogger/BasicLogger.hpp>

int main() {
	BasicLogger::init();

	// The below prints the line number and file name
	blLog("Hello World!");
	blInfo("Information!");
	blWarn("Warning!");
	blError("Error!");

	// The below prints without the line number or file name
	BasicLogger::log("Hello World!");
	BasicLogger::info("Information!");
	BasicLogger::warn("Warning!");
	BasicLogger::error("Error!");
}

```

## Options
The logging macros themselves can be renamed via cmake:
```cmake
# Name of logging macros
set(LoggingMacro "blLog")
set(LoggingInfoMacro "blInfo")
set(LoggingWarnMacro "blWarn")
set(LoggingErrorMacro "blError")
```

The following macro definitions are available as options:
```c++
#define BASIC_LOGGER_ENABLE_FILE // Enables logging to log files
#define BASIC_LOGGER_ENABLE_CONSOLE // Enables logging to console/standard output
#define BASIC_LOGGER_ENABLE_MULTITHREADING // Enabled thread safety
#define BASIC_LOGGER_LOGGING_FOLDER "logs/" // The folder to place logs into
#define BASIC_LOGGER_FILE_FORMAT "{:%F---%H-%M-%S.log}" // Format of file names, given to std::format with zoned time
```
