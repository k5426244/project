#ifndef __M_LEVEL_H__
#define __M_LEVEL_H__

#include <iostream>

namespace log
{
    class LogLevel
    {
    public:
        enum class value
        {
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };
        static const char *toString(LogLevel::value l)
        {
            switch (l)
            {
            case LogLevel::value::DEBUG:
                return "DEBUG";
            case LogLevel::value::INFO:
                return "INFO";
            case LogLevel::value::WARN:
                return "WARN";
            case LogLevel::value::ERROR:
                return "ERROR";
            case LogLevel::value::FATAL:
                return "FATAL";
            case LogLevel::value::OFF:
                return "OFF";
            default:
                return "UNKNOW";
            }
            return "UNKNOW";
        }
    };
}

#endif