#pragma once

#include <mutex>
#include "util.hpp"

namespace cloud
{
    #define CONFIG_FILE "./cloud.conf"
    class Config
    {
    public:
        static Config *GetInstance()
        {
            if (_instance == nullptr)
            {
                _mutex.lock();
                if (_instance == nullptr)
                    _instance = new Config();
                _mutex.unlock();
            }
            return _instance;
        }
        int GetHotTime()
        {
            return _hot_time;
        }
        int GetServerPort()
        {
            return _server_port;
        }
        std::string GetServerIp()
        {
            return _server_ip;
        }
        std::string GetDownloadPrefix()
        {
            return _download_prefix;
        }
        std::string GetPackFileSuffix()
        {
            return _packfile_suffix;
        }
        std::string GetPackDir()
        {
            return _pack_dir;
        }
        std::string GetBackDir()
        {
            return _back_dir;
        }
        std::string GetBackupFile()
        {
            return _backup_file;
        }

    private:
        Config()
        {
            ReadConfigFile();
        }
        bool ReadConfigFile()
        {
            FileUtil fu(CONFIG_FILE);
            std::string body;
            if (fu.GetContent(&body) == false)
            {
                std::cout << "load config file failed!" << std::endl;
                return false;
            }
            Json::Value root;
            if (JsonUtil::UnSerialize(body, &root) == false)
            {
                std::cout << "parse config file failed!" << std::endl;
                return false;
            } 
            _hot_time = root["hot_time"].asInt();
            _server_port = root["server_port"].asInt();
            _server_ip = root["server_ip"].asString();
            _download_prefix = root["download_prefix"].asString();
            _packfile_suffix = root["packfile_suffix"].asString();
            _pack_dir = root["pack_dir"].asString();
            _back_dir = root["back_dir"].asString();
            _backup_file = root["backup_file"].asString();
            return true;
        }
    private:
        static Config *_instance;
        static std::mutex _mutex;
        int _hot_time;                // 30
        int _server_port;             // 9090
        std::string _server_ip;       //"8.137.102.243"
        std::string _download_prefix; // "/download"
        std::string _packfile_suffix; // ".lz"
        std::string _pack_dir;        //"./packdir"
        std::string _back_dir;        //"./backdir"
        std::string _backup_file;     //"./cloud.dat"
    };
    Config* Config::_instance = nullptr;
    std::mutex Config::_mutex;
}
