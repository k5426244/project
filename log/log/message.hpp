#ifndef __M_MSG_H__
#define __M_MSG_H__

#include <memory>
#include <thread>
#include "util.hpp"
#include "level.hpp"

namespace log
{
    struct LogMsg
    {
        using ptr = std::shared_ptr<LogMsg>;
        size_t _line;           // 行号
        size_t _ctime;          // 时间
        std::thread::id _tid;   // 线程ID
        std::string _name;      // 日志器名称
        std::string _file;      // 文件名
        std::string _msg;       // 日志消息
        LogLevel::value _level; // 日志等级
        LogMsg() {}
        LogMsg(const LogLevel::value &level, const size_t &line, const std::string &file, const std::string &name, const std::string &msg)
            : _name(name), _file(file), _msg(msg), _level(level), _line(line), _ctime(util::Date::timestamp()), _tid(std::this_thread::get_id())
        {
        }
    };
}

#endif