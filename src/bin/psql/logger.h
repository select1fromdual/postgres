#pragma once
#include <memory>

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/ostr.h>
#include "spdlog/spdlog.h"

extern std::shared_ptr<spdlog::logger> logger;
extern std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink;

void InitLogger();
void ShutDownLogger();


#define PSQL_LOG_TRACE(...) logger->trace(__VA_ARGS__)
#define PSQL_LOG_DEBUG(...) logger->debug(__VA_ARGS__)
#define PSQL_LOG_INFO(...) logger->info(__VA_ARGS__)
#define PSQL_LOG_WARN(...) logger->warn(__VA_ARGS__)
#define PSQL_LOG_ERROR(...) logger->error(__VA_ARGS__)
