#ifndef __M_LOG_H__
#define __M_LOG_H__

#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"
#include "logger.hpp"
#include "lopper.hpp"
#include "buffer.hpp"

namespace log
{
    // 1.提供获取指定日志器的全局接口
    Logger::ptr getLogger(const std::string &name)
    {
        return LoggerManager::getInstance().getLogger(name);
    }
    Logger::ptr rootLogger()
    {
        return LoggerManager::getInstance().rootLogger();
    }
    // 2.使用宏函数对日志器的接口进行代理
    #define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define error(fmt, ...) error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    // 3.提供宏函数,直接通过默认日志器进行日志的标准输出打印
    #define DEBUG(fmt, ...) log::rootLogger()->debug(fmt, ##__VA_ARGS__)
    #define INFO(fmt, ...) log::rootLogger()->info(fmt, ##__VA_ARGS__)
    #define WARN(fmt, ...) log::rootLogger()->warn(fmt, ##__VA_ARGS__)
    #define ERROR(fmt, ...) log::rootLogger()->error(fmt, ##__VA_ARGS__)
    #define FATAL(fmt, ...) log::rootLogger()->fatal(fmt, ##__VA_ARGS__)
}

#endif