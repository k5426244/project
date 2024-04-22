#ifndef __M_BUF_H__
#define __M_BUF_H__

#include <vector>
#include <cassert>
#include "util.hpp"

namespace log
{
    #define DEFAULT_BUFFER_SIZE (1 * 1024 * 1024)
    #define THRESHOLD_BUFFER_SIZE (8 * 1024 * 1024)
    #define INCREMENT_BUFFER_SIZE (1 * 1024 * 1024)
    class Buffer
    {
    public:
        Buffer():_buffer(DEFAULT_BUFFER_SIZE), _reader_idx(0), _writer_idx(0)
        {}
        // 向缓冲区写入数据
        void push(const char *data, size_t len)
        {
            // 检查是否需要扩容
            ensureEnoughSize(len);
            std::copy(data, data + len, &_buffer[_writer_idx]);
            moveWriter(len);
        }
        size_t writeAbleSize()
        {
            return _buffer.size() - _writer_idx;
        }
        // 返回可读数据的起始位置 
        const char* begin()
        {
            return &_buffer[_reader_idx];
        }
        // 返回可读入数据的长度
        size_t readAbleSize()
        {
            return _writer_idx - _reader_idx;
        }
        void moveReader(size_t len)
        {
            assert(len <= readAbleSize());
            _reader_idx += len;
        }
        // 重置读写位置，初始化缓冲区
        void reset()
        {
            _reader_idx = 0;
            _writer_idx = 0;
            // _buffer.clear();
        }
        void swap(Buffer &buffer)
        {
            _buffer.swap(buffer._buffer);
            std::swap(_reader_idx, buffer._reader_idx);
            std::swap(_writer_idx, buffer._writer_idx);
            // buffer._reader_idx = _reader_idx;
            // buffer._writer_idx = _writer_idx;
            // reset();
        }
        // 判断缓冲区是否为空
        bool empty()
        {
            return (_reader_idx == _writer_idx); 
        }
    private:
        // 扩容
        void ensureEnoughSize(size_t len)
        {
            if (len < writeAbleSize())
                return;
            size_t new_size = 0;
            // 扩容的大小与阈值有关，小于阈值翻倍增长，大于阈值线性增长
            if (_buffer.size() < THRESHOLD_BUFFER_SIZE)
                new_size = _buffer.size() * 2 + len;
            else 
                new_size = _buffer.size() + INCREMENT_BUFFER_SIZE + len;
            _buffer.resize(new_size);
        }
        void moveWriter(size_t len)
        {
            assert((len + _writer_idx) <= _buffer.size());
            _writer_idx += len;
        }
    private:
        std::vector<char> _buffer;
        size_t _reader_idx;
        size_t _writer_idx;
    };
}

#endif