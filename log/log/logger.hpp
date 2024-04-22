#ifndef __M_LOGGER_H__
#define __M_LOGGER_H__

#include <mutex>
#include <atomic>
#include <functional>
#include <stdarg.h>
#include <unordered_map>
#include "sink.hpp"
#include "level.hpp"
#include "lopper.hpp"

namespace log
{
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string &logger_name, LogLevel::value limit_level, Formatter::ptr formatter, std::vector<LogSink::ptr> &sinks)
            : _logger_name(logger_name), _limit_level(limit_level), _formatter(formatter), _sinks(sinks.begin(), sinks.end())
        {
        }
        const std::string& name()
        {
            return _logger_name;
        }
        void debug(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if (LogLevel::value::DEBUG < _limit_level)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!!" << std::endl;
                return;
            }
            serialize(LogLevel::value::DEBUG, file, line, res);
            free(res);
            va_end(ap);
        }
        void info(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if (LogLevel::value::INFO < _limit_level)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!!" << std::endl;
                return;
            }
            serialize(LogLevel::value::INFO, file, line, res);
            free(res);
            va_end(ap);
        }
        void warn(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if (LogLevel::value::WARN < _limit_level)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!!" << std::endl;
                return;
            }
            serialize(LogLevel::value::WARN, file, line, res);
            free(res);
            va_end(ap);
        }
        void error(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if (LogLevel::value::ERROR < _limit_level)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!!" << std::endl;
                return;
            }
            serialize(LogLevel::value::ERROR, file, line, res);
            free(res);
            va_end(ap);
        }
        void fatal(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if (LogLevel::value::FATAL < _limit_level)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!!" << std::endl;
                return;
            }
            serialize(LogLevel::value::FATAL, file, line, res);
            free(res);
            va_end(ap);
        }

    protected:
        void serialize(LogLevel::value level, const std::string &file, size_t line, char *str)
        {
            LogMsg msg(level, line, file, _logger_name, str);
            // format格式化消息_V1
            // std::stringstream ss;
            // _formatter->format(ss, msg);
            // log(ss.str().c_str(), ss.str().size())
            // V2
            std::string s = _formatter->format(msg);
            log(s.c_str(), s.size());
        }
        virtual void log(const char *data, size_t len) = 0;

    protected:
        std::mutex _mutex;
        std::string _logger_name;
        std::atomic<LogLevel::value> _limit_level;
        Formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };
    // 设置同步/异步日志器
    class SyncLogger : public Logger
    {
    public:
        using ptr = std::shared_ptr<SyncLogger>;
        SyncLogger(const std::string &logger_name, LogLevel::value limit_level, Formatter::ptr formatter, std::vector<LogSink::ptr> sinks)
        :Logger(logger_name, limit_level, formatter, sinks)
        {}
    private:
        virtual void log(const char *data, size_t len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_sinks.empty())
                return;
            for (auto &sink : _sinks)
                sink->log(data, len);
        }
    };
    class AsyncLogger : public Logger
    {
    public:
        using ptr = std::shared_ptr<AsyncLogger>;
        AsyncLogger(const std::string &logger_name, LogLevel::value limit_level, Formatter::ptr formatter, std::vector<LogSink::ptr> sinks, AsyncType lopper_type)
        :Logger(logger_name, limit_level, formatter, sinks), _lopper(std::make_shared<AsyncLopper>(std::bind(&AsyncLogger::realLog, this, std::placeholders::_1), lopper_type))
        {}
        void log(const char *data, size_t len)
        {
            _lopper->push(data, len);
        }
        // 实际落地函数
        void realLog(Buffer &buf)
        {
            if (_sinks.empty())
                return;
            for (auto &sink : _sinks)
                sink->log(buf.begin(), buf.readAbleSize());
        }
    private:
        AsyncLopper::ptr _lopper;
    };
    enum class LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC
    };
    // 将不同类型日志器的创建放到同一个日志器建造者类中完成
    class LoggerBuilder
    {
    public:
        LoggerBuilder():_logger_type(LoggerType::LOGGER_SYNC), _limit_level(LogLevel::value::DEBUG), _lopper_type(AsyncType::ASYNC_SAFE)
        {}
        void buildLoggerType(LoggerType type)
        {
            _logger_type = type;
        }
        void buildEnableUnSafeAsync()
        {
            _lopper_type = AsyncType::ASYNC_UNSAFE;
        }
        void buildLoggerName(const std::string &name)
        {
            _logger_name = name;
        }
        void buildLoggerLevel(LogLevel::value level)
        {
            _limit_level = level;
        }
        void buildFormatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p] %m%n")
        {
            _formatter = std::make_shared<Formatter>(pattern);
        }
        void buildFormatter(const Formatter::ptr &formatter)
        {
            _formatter = formatter;
        }
        template<class SinkType, class ...Args>
        void buildSink(Args &&...args)
        {
            LogSink::ptr psink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }
        virtual Logger::ptr build() = 0;
    protected:
        AsyncType _lopper_type;
        LoggerType _logger_type;
        std::string _logger_name;
        LogLevel::value _limit_level;
        Formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };
    // 派生出具体的创造者类
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build()
        {
            assert(_logger_name.empty() == false); // 必须有日志器名称
            if (_formatter.get() == nullptr)
                _formatter = std::make_shared<Formatter>();
            if (_sinks.empty())
                buildSink<StdoutSink>();
            if (_logger_type == LoggerType::LOGGER_ASYNC)
            {
                return std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _lopper_type);
            }
            return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
        }
    };
    
    class LoggerManager
    {
    public:
        static LoggerManager& getInstance()
        {
            // 当静态局部变量在没有构造完成之前，其他的线程进入就会阻塞
            static LoggerManager eton;
            return eton;
        }
        void addLogger(Logger::ptr &logger)
        {
            if (hasLogger(logger->name()))
                return;
            std::unique_lock<std::mutex> lock(_mutex);
            _loggers.insert(std::make_pair(logger->name(), logger));
        }
        bool hasLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if (it == _loggers.end())
                return false;
            return true;
        }
        Logger::ptr getLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if (it == _loggers.end())
                return Logger::ptr();
            return it->second;
        }
        Logger::ptr rootLogger()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _root_logger;
        }
    private:
        LoggerManager()
        {
            std::unique_ptr<log::LoggerBuilder> builder(new log::LocalLoggerBuilder());
            builder->buildLoggerName("root");
            // builder->buildLoggerType(log::LoggerType::LOGGER_SYNC);
            _root_logger = builder->build();
            _loggers.insert(std::make_pair("root", _root_logger));
        }
        LoggerManager(const LoggerManager&) = delete;
        LoggerManager &operator=(const LoggerManager&) = delete;
    private:
        std::mutex _mutex;
        Logger::ptr _root_logger; // 默认日志器
        std::unordered_map<std::string, Logger::ptr> _loggers;
    };
    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build()
        {
            assert(_logger_name.empty() == false); // 必须有日志器名称
            if (_formatter.get() == nullptr)
                _formatter = std::make_shared<Formatter>();
            if (_sinks.empty())
                _sinks.push_back(std::make_shared<StdoutSink>());
            Logger::ptr logger;
            if (_logger_type == LoggerType::LOGGER_ASYNC)
                logger = std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _lopper_type);
            else
                logger = std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
            LoggerManager::getInstance().addLogger(logger);
            return logger;
        }
    };
}

#endif