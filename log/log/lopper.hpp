#ifndef __M_LOOPER_H__
#define __M_LOOPER_H__

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include "buffer.hpp"

namespace log
{
    enum class AsyncType
    {
        ASYNC_SAFE,  // 安全状态,表示缓冲区满了则阻塞,避免资源耗尽的风险
        ASYNC_UNSAFE // 不考虑资源耗尽,无限扩容
    };
    class AsyncLopper
    {
    public:
        using Functor = std::function<void(Buffer &Buffer)>;
        using ptr = std::shared_ptr<AsyncLopper>;
        AsyncLopper(const Functor &cb, AsyncType lopper_type = AsyncType::ASYNC_SAFE)
            : _lopper_type(lopper_type), _stop(false), _thread(std::thread(&AsyncLopper::threadEntry, this)), _callBack(cb)
        {
        }
        ~AsyncLopper()
        {
            stop(); // 线程退出需要等待,否则段错误
        }
        void stop()
        {
            _stop = true;
            _cond_con.notify_all(); // 唤醒所有等待线程
            _thread.join();
        }
        void push(const char *data, size_t len)
        {
            if (_stop)
                return;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                // 缓冲区是无限扩容还是固定大小
                if (_lopper_type == AsyncType::ASYNC_SAFE)
                    _cond_pro.wait(lock, [&]()
                                   { return _pro_buf.writeAbleSize() >= len; });
                _pro_buf.push(data, len);
            }
            if (_stop || !_pro_buf.empty())
                _cond_con.notify_one();
        }
    private:
        void threadEntry()
        {
            while (true) // 退出由缓冲区和stop标志共同决定
            {
                // 加锁并拿出数据
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _cond_con.wait(lock, [&]()
                                   { return _stop || !_pro_buf.empty(); });
                    // 退出标志被设置,且生产缓冲区无数据,这时候再退出
                    if (_stop && _pro_buf.empty())
                        break;
                    _con_buf.swap(_pro_buf);
                }
                _cond_pro.notify_all();
                // 处理数据
                _callBack(_con_buf);
                _con_buf.reset();
            }
        }

    private:
        AsyncType _lopper_type;
        std::atomic<bool> _stop;
        Buffer _pro_buf;
        Buffer _con_buf;
        std::mutex _mutex;
        std::condition_variable _cond_pro;
        std::condition_variable _cond_con;
        std::thread _thread;
        Functor _callBack;
    };
}

#endif