#include "SpdLogger.h"

void LoggerInit()
{
	// 指定日志文件大小
//	auto logger = spdlog::rotating_logger_mt("daily_logger", "logs/log.txt", 1024 * 1024 * 1024, 1, true);

	// 指定日志创建时间，2：30am
	auto logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30);

	// warn级别以上的日志持久化，防止日志丢失
	logger->flush_on(spdlog::level::warn);

	//日志每3秒向磁盘持久化一次
	spdlog::flush_every(std::chrono::seconds(3));

	// 设置默认logger
	auto console = spdlog::stdout_color_mt("console");
	spdlog::set_default_logger(console);

	//设置日志输出级别
	spdlog::set_level(spdlog::level::info);

	//设置日志格式
	spdlog::set_pattern("%Y-%m-%d %H:%M:%S:%e [%l][%s:%#] %v");
}


void LoggerDrop()
{
	spdlog::drop_all();
}

