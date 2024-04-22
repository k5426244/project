#ifndef __M_SINK_H__
#define __M_SINK_H__

#include <fstream>
#include "util.hpp"
#include "message.hpp"
#include "format.hpp"

namespace log
{
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        LogSink(){};
        virtual ~LogSink(){};
        virtual void log(const char *data, size_t len) = 0;
    };
    // 落地方向:标准输出
    class StdoutSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<StdoutSink>;
        virtual void log(const char *data, size_t len)
        {
            std::cout.write(data, len);
        }
    };
    // 落地方向:指定文件
    class FileSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<FileSink>;
        FileSink(const std::string &filename) : _filename(filename)
        {
            util::File::create_directory(util::File::path(_filename));
            _ofs.open(_filename, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        virtual void log(const char *data, size_t len)
        {
            _ofs.write(data, len);
            if (_ofs.good() == false)
                std::cout << "日志输出文件失败!" << std::endl;
        }
    private:
        std::string _filename;
        std::ofstream _ofs;
    };
    // 落地方向:滚动文件(以大小进行滚动)
    class RollBySizeSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<RollBySizeSink>;
        RollBySizeSink(const std::string &basename, size_t max_fsize)
            : _basename(basename), _max_fsize(max_fsize), _cur_fsize(0), _name_count(0)
        {
            util::File::create_directory(util::File::path(_basename));
        }
        virtual void log(const char *data, size_t len)
        {
            if (_ofs.is_open() == false || _cur_fsize >= _max_fsize)
            {
                _ofs.close();
                std::string name = createNewFile();
                _ofs.open(name, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
                _cur_fsize = 0;
            }
            _ofs.write(data, len);
            assert(_ofs.is_open());
            _cur_fsize += len;
        }
    private:
        std::string createNewFile()
        {
            time_t t = time(nullptr);
            struct tm lt;
            localtime_r(&t, &lt);
            std::string newName = _basename;
            char buffer[128];
            snprintf(buffer, sizeof(buffer), "%d%d%d%d%d%d", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday,
                     lt.tm_hour, lt.tm_min, lt.tm_sec);
            newName += buffer;
            newName += "_" + std::to_string(_name_count);
            ++_name_count;
            newName += ".log";
            return newName;
        }
    private:
        size_t _name_count;
        std::string _basename;
        std::ofstream _ofs;
        size_t _max_fsize; // 记录最大大小,当前文件超过了这个文件大小则切换文件
        size_t _cur_fsize; // 记录当前文件已经写入的数据大小
    };
    class SinkFactory
    {
    public:
        template <class SinkType, class... Args>
        static LogSink::ptr create(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };
}

#endif