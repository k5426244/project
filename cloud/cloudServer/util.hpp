#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <jsoncpp/json/json.h>
#include <experimental/filesystem>
#include <memory>
#include "bundle.h"

namespace cloud
{
    enum Err
    {
        STATERR = 1,  
    };
    namespace fs = std::experimental::filesystem;
    class FileUtil
    {
    public:
        FileUtil(const std::string filename):_filename(filename)
        {}
        bool Remove()
        {
            if (Exists() == false)
                return true;
            remove(_filename.c_str());
            return true;
        }
        int64_t FileSize()
        {
            struct stat _st;
            int n = stat(_filename.c_str(), &_st);
            if (n < 0)
            {
                std::cout << "stat fail!" << std::endl;
                exit(Err::STATERR);
            }
            return _st.st_size;
        }
        time_t LastMTime()
        {
            struct stat _st;
            int n = stat(_filename.c_str(), &_st);
            if (n < 0)
            {
                std::cout << "stat fail!" << std::endl;
                exit(Err::STATERR);
            }
            return _st.st_mtime;
        }
        time_t LastATime()
        {
            struct stat _st;
            int n = stat(_filename.c_str(), &_st);
            if (n < 0)
            {
                std::cout << "stat fail!" << std::endl;
                exit(Err::STATERR);
            }
            return _st.st_atime;
        }
        std::string FileName()
        {
            size_t pos = _filename.find_last_of("/\\");
            if (pos == std::string::npos)
                return _filename;
            return _filename.substr(pos + 1);
        }
        bool GetPosLen(std::string *body, size_t pos, size_t len)
        {
            if (pos + len > FileSize())
            {
                std::cout << "get file len is failed!" << std::endl;
                return false;
            }
            std::ifstream ifs;
            ifs.open(_filename, std::ios::binary);
            if (ifs.is_open() == false)
            {
                std::cout << "read open file failed!" << std::endl;
                return false;
            }
            ifs.seekg(pos, std::ios::beg);
            body->resize(len);
            ifs.read(&(*body)[0], len);
            if (ifs.good() == false)
            {
                std::cout << "get file content failed!" << std::endl;
                ifs.close();
                return false;
            }
            ifs.close();
            return true;
        }
        bool GetContent(std::string *body)
        {
            size_t fsize = FileSize();
            return GetPosLen(body, 0, fsize);
        }
        bool SetContent(const std::string &body)
        {
            std::ofstream ofs;
            ofs.open(_filename, std::ios::binary);
            if (ofs.is_open() == false)
            {
                std::cout << "write open file failed!" << std::endl;
                return false;
            }
            ofs.write(&body[0], body.size());
            if (ofs.good() == false)
            {
                std::cout << "write file content failed!" << std::endl;
                return false;
            }
            ofs.close();
            return true;
        }
        // 压缩
        bool Compress(const std::string &packname)
        {
            std::string body;
            if (GetContent(&body) == false)
            {
                std::cout << "compress get file content failed" << std::endl;
                return false;
            }
            std::string packed = bundle::pack(bundle::LZIP, body);
            // 将压缩的数据存储到压缩包文件
            FileUtil fu(packname);
            if (fu.SetContent(packed) == false)
            {
                std::cout << "compress write packed data failed!" << std::endl;
                return false;
            }
            return true;
        }
        // 解压缩
        bool UnCompress(const std::string &filename)
        {
            std::string body;
            if (GetContent(&body) == false)
            {
                std::cout << "uncompress get file content failed!" << std::endl;
                return false;
            }
            std::string unpacked = bundle::unpack(body);
            FileUtil fu(filename);
            if (fu.SetContent(unpacked) == false)
            {
                std::cout << "uncompress write packed data failed!" << std::endl;
                return false;
            }
            return true;
        }
        bool Exists()
        {
            return fs::exists(_filename);
        }
        // 创建目录
        bool CreateDirectory()
        {
            if (Exists())
                return true;
            return fs::create_directory(_filename); // 测试后有点问题,创建的是文件
        }
        // 遍历目录下的文件
        bool ScanDirectory(std::vector<std::string> *arry)
        {
            for (auto &p : fs::directory_iterator(_filename))
            {
                if (fs::is_directory(p) == true)
                    continue;
                // relative_path 带有路径的文件名
                arry->push_back(fs::path(p).relative_path().string());
            }
            return true;
        }
    private:
        std::string _filename;
    };
    class JsonUtil
    {
    public:
        static bool Serialize(const Json::Value &root, std::string *str)
        {
            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
            std::stringstream ss;
            if (sw->write(root, &ss) != 0)
            {
                std::cout << "json write failed!" << std::endl;
                return false;
            }
            *str = ss.str();
            return true;
        }
        static bool UnSerialize(const std::string &str, Json::Value *root)
        {
            Json::CharReaderBuilder crb;
            std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
            std::string err;
            bool ret = cr->parse(str.c_str(), str.c_str() + str.size(), root, &err);
            if (ret == false)
            {
                std::cout << "parse error: " << err << std::endl;
                return false;
            }
            return true;
        }
    };
}
