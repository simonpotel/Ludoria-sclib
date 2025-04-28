#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>

namespace Logger {
    void initialize();
    void info(const std::string& tag, const std::string& message);
    void warning(const std::string& tag, const std::string& message);
    void error(const std::string& tag, const std::string& message);
}

#endif 