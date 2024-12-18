// src/platform_config.h

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

// Conditional compilation for different platforms
// Include the JSON library
#if defined(_WIN32) || defined(_WIN64)
    #include <nlohmann/json.hpp>
    #define OS_NAME "Windows"
#elif defined(__APPLE__) || defined(__MACH__)
    #include "../json.hpp" // Include for macOS
    #define OS_NAME "macOS"
#elif defined(__linux__)
    #include "json.hpp" // Include for Linux
    #define OS_NAME "Linux"
#else
    #define OS_NAME "Unknown platform"
#endif

// Avoid using 'using namespace' in headers
// Users can use 'using json = nlohmann::json;' in implementation files if needed

#endif // PLATFORM_CONFIG_H
