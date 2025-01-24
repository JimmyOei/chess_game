#ifndef LOGGER_H
#define LOGGER_H

#define LOG_LEVEL LogLevel::DEBUG

#include <iostream>
#include <sstream>

/**
 * @brief Enum class for the log levels
 */
enum LogLevel
{
  DEBUG,
  INFO,
  WARNING,
  ERROR
};

class logger
{
public:
  /**
   * @brief constructor for the logger
   * 
   * @param logLevel log level to log at
   */
  logger(LogLevel logLevel = LogLevel::ERROR)
  {
    _buffer << logLevel << " : ";
  }

  /**
   * @brief operator<< overload for the logger
   *        logs the value to the buffer
   * 
   * @param value value to log
   * @return logger& reference to the logger
   */
  template <typename T>
  logger &operator<<(T const &value)
  {
    _buffer << value;
    return *this;
  }

  /**
   * @brief destructor for the logger, logs the buffer to stderr
   */
  ~logger()
  {
    _buffer << std::endl;
    std::cerr << _buffer.str();
  }

private:
  std::ostringstream _buffer;
};

#define log(level)       \
  if (level >= LOG_LEVEL) \
  logger(level)

#endif