/**
 * This file contains a minimal logger that writes one line per message to
 * stderr, and the CHESS_LOG macro to use it:
 *
 *   CHESS_LOG(LogLevel::kInfo) << "Engine made move " << move;
 *
 * Messages below kMinLogLevel are skipped without evaluating their arguments.
 */

#ifndef CHESS_GAME_SRC_UTIL_LOGGER_H_
#define CHESS_GAME_SRC_UTIL_LOGGER_H_

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string_view>

namespace chess {

enum class LogLevel : uint8_t { kDebug, kInfo, kWarning, kError };

constexpr LogLevel kMinLogLevel = LogLevel::kInfo;

[[nodiscard]] constexpr std::string_view LogLevelToString(
    LogLevel const level) {
  switch (level) {
    case LogLevel::kDebug:
      return "DEBUG";
    case LogLevel::kInfo:
      return "INFO";
    case LogLevel::kWarning:
      return "WARNING";
    case LogLevel::kError:
      return "ERROR";
  }
  return "UNKNOWN";
}

// Collects one message and writes it to stderr when destroyed.
class Logger {
 public:
  explicit Logger(LogLevel const level) {
    buffer_ << "[" << LogLevelToString(level) << "] : ";
  }

  Logger(Logger const &) = delete;
  Logger &operator=(Logger const &) = delete;
  Logger(Logger &&) = delete;
  Logger &operator=(Logger &&) = delete;

  ~Logger() {
    buffer_ << '\n';
    std::cerr << buffer_.str();
  }

  template <typename T>
  Logger &operator<<(T const &value) {
    buffer_ << value;
    return *this;
  }

 private:
  std::ostringstream buffer_;
};

// The empty if-branch keeps a following `else` from binding to this `if`.
#define CHESS_LOG(level)                 \
  if ((level) < ::chess::kMinLogLevel) { \
  } else                                 \
    ::chess::Logger(level)

}  // namespace chess

#endif  // CHESS_GAME_SRC_UTIL_LOGGER_H_
