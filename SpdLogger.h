#pragma once
#ifndef SPDLOGGER_H_
#define SPDLOGGER_H_

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <iostream>
#include <memory>
#include "EnumDef.h"

// 日志同时输出到控制台和文件
#define INFO(...)  SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__);         SPDLOG_LOGGER_INFO(spdlog::get("daily_logger"), __VA_ARGS__)
#define WARN(...)  SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__);         SPDLOG_LOGGER_WARN(spdlog::get("daily_logger"), __VA_ARGS__)
#define ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__); 	      SPDLOG_LOGGER_ERROR(spdlog::get("daily_logger"), __VA_ARGS__)

extern void LoggerInit();

extern void LoggerDrop();

#endif /*LOGGER_H_ */

