#include "logger.h"

std::shared_ptr<spdlog::logger> logger = nullptr;
std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink = nullptr;

void InitLogger() {
  if (logger == nullptr) {
    file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
    logger = std::make_shared<spdlog::logger>("execution_logger", file_sink);
    spdlog::register_logger(logger);
  }
}

void ShutDownLogger() {
  if (file_sink != nullptr) {
    spdlog::shutdown();
    file_sink = nullptr;
  }
}
