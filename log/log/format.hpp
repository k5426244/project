#ifndef __M_FMT_H__
#define __M_FMT_H__

#include <vector>
#include <assert.h>
#include <sstream>
#include <utility>
#include "util.hpp"
#include "level.hpp"
#include "message.hpp"

namespace log
{
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual ~FormatItem() {}
        virtual void format(std::ostream &os, const LogMsg &msg) = 0;
    };
    class MsgFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            os << msg._msg;
        }
    };
    class LevelFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            os << LogLevel::toString(msg._level);
        }
    };
    class NameFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            os << msg._name;
        }
    };
    class ThreadFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            os << msg._tid;
        }
    };
    class TimeFormatItem : public FormatItem
    {
    public:
        TimeFormatItem(const std::string &format = "%H:%M:%S") : _format(format)
        {
            if (format.empty())
                _format = "%H:%M:%S";
        }
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            time_t t = msg._ctime;
            struct tm lt;
            localtime_r(&t, &lt);
            char tmp[128];
            strftime(tmp, sizeof(tmp) - 1, _format.c_str(), &lt);
            os << tmp;
        }
        // virtual void format(std::ostream &os, const LogMsg &msg) 
        // {
        //     os << msg._ctime;
        // }
    private:
        std::string _format;
    };
    class CFileFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            os << msg._file;
        }
    };
    class CLineFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            os << msg._line;
        }
    };
    class TabFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            os << "\t";
        }
    };
    class NLineFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            os << "\n";
        }
    };
    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string &str = "") : _str(str)
        {
        }
        virtual void format(std::ostream &os, const LogMsg &msg)
        {
            os << _str;
        }

    private:
        std::string _str;
    };
    class Formatter
    {
    public:
        using ptr = std::shared_ptr<Formatter>;
        Formatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p] %m%n") : _pattern(pattern)
        {
            assert(parsePattern());
        }
        const std::string pattern()
        {
            return _pattern;
        }
        // void format(std::ostream &os, LogMsg &msg)
        // {
        //     for (auto &it : _items)
        //         it->format(os, msg);
        // }
        std::string format(const LogMsg &msg)
        {
            std::stringstream ss;
            for (auto &it : _items)
                it->format(ss, msg);
            return ss.str();
        }
    private:
        bool parsePattern()
        {
            std::vector<std::pair<std::string, std::string>> fmt_order;
            size_t n = _pattern.size(), pos = 0;
            std::string key, val;
            while (pos < n)
            {
                // 先判断是否为 %
                if (_pattern[pos] != '%')
                {
                    val.push_back(_pattern[pos++]);
                    continue;
                }
                // 再判断是否为 %%
                if (pos + 1 < n && _pattern[pos + 1] == '%')
                {
                    val.push_back('%');
                    pos += 2;
                    continue;
                }
                if (!val.empty())
                    fmt_order.push_back(make_pair(key, val));
                val.clear();
                // 特殊情况处理后,处理格式化字符
                pos += 1;
                if (pos == n)
                {
                    std::cout << "%之后,没有对应的格式化字符!" << std::endl;
                    return false;
                }
                key = _pattern[pos];
                // 判断_pattern[pos]后是否有子串,及{}
                pos += 1;
                if (pos < n &&  _pattern[pos] == '{')
                {
                    pos += 1;
                    while (pos < n && _pattern[pos] != '}')
                        val.push_back(_pattern[pos++]);
                    if (pos == n) // 判断是否找到 }
                    {    
                        std::cout << "子规则{}匹配出错!\n" << std::endl;
                        return false;
                    }
                    pos += 1;
                }
                fmt_order.push_back(make_pair(key, val));
                key.clear(), val.clear();
            }
            for (auto &it : fmt_order)
                _items.push_back(createItem(it.first, it.second));

            return true;
        }
        FormatItem::ptr createItem(const std::string &key, const std::string &val)
        {
            //  %d ⽇期
            //  %T 缩进
            //  %t 线程id
            //  %p ⽇志级别
            //  %c ⽇志器名称
            //  %f ⽂件名
            //  %l ⾏号
            //  %m ⽇志消息
            //  %n 换⾏
            if (key == "d")
                return std::make_shared<TimeFormatItem>(val);
            if (key == "t")
                return std::make_shared<ThreadFormatItem>();
            if (key == "T")
                return std::make_shared<TabFormatItem>();
            if (key == "p")
                return std::make_shared<LevelFormatItem>();
            if (key == "c")
                return std::make_shared<NameFormatItem>();
            if (key == "f")
                return std::make_shared<CFileFormatItem>();
            if (key == "l")
                return std::make_shared<CLineFormatItem>();
            if (key == "m")
                return std::make_shared<MsgFormatItem>();
            if (key == "n")
                return std::make_shared<NLineFormatItem>();
            if (key.empty())
                return std::make_shared<OtherFormatItem>(val);
            std::cout << "没有对应的格式化字符: %" << key << std::endl;
            abort();
            return FormatItem::ptr();
        }

    private:
        std::string _pattern;
        std::vector<FormatItem::ptr> _items;
    };
}

#endif