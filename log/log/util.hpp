#ifndef __M_UTIL_H__
#define __M_UTIL_H__

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace log
{
    namespace util
    {
        class Date
        {
        public:
            //  获取系统时间
            static size_t timestamp()
            {
                return (size_t)time(nullptr);
            }

            static std::string getTime()
            {
                time_t t = time(nullptr);
                struct tm lt;
                localtime_r(&t, &lt);
                char buffer[128];
                snprintf(buffer, sizeof(buffer), "[%d-%d-%d %d:%d:%d]", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday,
                         lt.tm_hour, lt.tm_min, lt.tm_sec);
                return buffer;
            }
        };
        class File
        {
        public:
            //  检查文件在不在
            static bool exists(const std::string &name)
            {
                struct stat st;
                return stat(name.c_str(), &st) == 0;
            }
            //  获取文件所在路径
            static std::string path(const std::string &name)
            {
                if (name.empty())
                    return ".";
                size_t pos = name.find_last_of("/\\");
                if (pos == std::string::npos)
                    return ".";
                return name.substr(0, pos + 1);
            }
            //  创建目录 /a/b/c/def
            static void create_directory(const std::string &path)
            {
                if (path.empty())
                    return;
                if (exists(path))
                    return;
                size_t pos = 0, idx = 0;
                while (idx < path.size())
                {
                    pos = path.find_first_of("/\\", idx);
                    if (pos == std::string::npos)
                    {
                        mkdir(path.c_str(), 0777);
                        return;
                    }
                    if (pos == idx)
                    {
                        idx = pos + 1;
                        continue;
                    }
                    std::string subdir = path.substr(0, pos);
                    if (subdir == "." || subdir == "..")
                    {
                        idx = pos + 1;
                        continue;
                    }
                    if (exists(subdir))
                    {
                        idx = pos + 1;
                        continue;
                    }
                    mkdir(subdir.c_str(), 0777);
                    idx = pos + 1;
                }
            }
        };
    }
}

#endif