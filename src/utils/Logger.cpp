#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace {
    std::mutex log_mutex; // mutex for safe logging (thread safe) -> means that only one thread can access the log_mutex at a time
    bool initialized = false; // if the logger has been initialized

    std::string get_timestamp() {
        // aims to get the current time and format it as a string
        auto now = std::chrono::system_clock::now(); // gets the current time
        auto now_c = std::chrono::system_clock::to_time_t(now); // converts the current time to a time_t
        std::stringstream ss; // creates a stringstream object
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S"); // formats the time as a string
        return ss.str(); // returns the formatted time as a string
    }
}

namespace Logger {
    void initialize() {
        std::lock_guard<std::mutex> lock(log_mutex); // locks the log_mutex to prevent race conditions
        if (!initialized) { // if the logger has not been initialized alrdy
            initialized = true; 
        }
    }


    // differents log levels
    void info(const std::string& tag, const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::cout << "[" << get_timestamp() << "] [INFO] [" << tag << "] " << message << std::endl;
    }

    void warning(const std::string& tag, const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::cerr << "[" << get_timestamp() << "] [WARN] [" << tag << "] " << message << std::endl;
    }

    void error(const std::string& tag, const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::cerr << "[" << get_timestamp() << "] [ERROR] [" << tag << "] " << message << std::endl;
    }
} 