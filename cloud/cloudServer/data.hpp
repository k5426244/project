#pragma once

#include <unordered_map>
#include <pthread.h>
#include "util.hpp"
#include "config.hpp"

namespace cloud
{
    class BackupInfo
    {
    public:
        bool pack_flag;
        size_t fsize;
        time_t mtime;
        time_t atime;
        std::string real_path;
        std::string pack_path;
        std::string url;

        bool NewBackupInfo(const std::string &realpath)
        {
            FileUtil fu(realpath);
            if (fu.Exists() == false)
            {
                std::cout << "new backupinfo file not exists!" << std::endl;
                return false;
            }
            Config *config = Config::GetInstance();
            std::string packdir = config->GetPackDir();
            std::string packsuffix = config->GetPackFileSuffix();
            std::string download_prefix = config->GetDownloadPrefix();
            this->pack_flag = false;
            this->fsize = fu.FileSize();
            this->atime = fu.LastMTime();
            this->real_path = realpath;
            // ./packdir/a.txt   ->   ./packdir/a.txt.lz
            this->pack_path = packdir + fu.FileName() + packsuffix;
            // ./a.txt   ->   /download/a.txt 所有文件默认在download
            this->url = download_prefix + fu.FileName();
            return true;
        }
    };
    class DataManager
    {
    public:
        DataManager()
        {
            _backup_file = Config::GetInstance()->GetBackupFile();
            pthread_rwlock_init(&_rwlock, nullptr); //初始化读写锁
            InitLoad();
        }
        ~DataManager()
        {
            pthread_rwlock_destroy(&_rwlock); //销毁读写锁
        }
        // 加载数据
        bool InitLoad()
        {
            // 1.将数据文件中的数据读取出来
            FileUtil fu(_backup_file);
            if (fu.Exists() == false)
                return true;
            std::string body;
            fu.GetContent(&body);
            // 2.反序列化
            Json::Value root;
            JsonUtil::UnSerialize(body, &root);
            // 3.将反序列化的数据添加到table中
            for (int i = 0; i < root.size(); ++i)
            {
                BackupInfo info;
                info.pack_flag = root[i]["pack_flag"].asBool();
                info.fsize = root[i]["fsize"].asInt64();
                info.atime = root[i]["atime"].asInt64();
                info.mtime = root[i]["mtime"].asInt64();
                info.pack_path = root[i]["pack_path"].asString();
                info.real_path = root[i]["real_path"].asString();
                info.url = root[i]["url"].asString();
                Insert(info);
            }
            return true;
        }
        bool Insert(const BackupInfo &info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _table[info.url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }
        bool Updata(const BackupInfo &info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _table[info.url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }
        // 通过URL寻找info
        bool GetOneByURL(const std::string &url, BackupInfo *info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it = _table.find(url);
            if (it == _table.end())
            {
                pthread_rwlock_unlock(&_rwlock);
                return false;
            }
            *info = it->second;
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        // 通过real_path寻找info
        bool GetRealPath(const std::string &realpath, BackupInfo *info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            for (auto &it : _table)
            {
                if (it.second.real_path == realpath)
                {
                    *info = it.second;
                    pthread_rwlock_unlock(&_rwlock);
                    return true;
                }
            }
            pthread_rwlock_unlock(&_rwlock);
            return false;
        }
        // 获取所有的info信息
        bool GetAll(std::vector<BackupInfo> *arry)
        {
            pthread_rwlock_wrlock(&_rwlock);
            for (auto &it : _table)
            {
                arry->push_back(it.second);
            }
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        // 数据持久化 更新cloud.dat数据
        bool Storage()
        {
            // 1.获取所有数据
            std::vector<BackupInfo> arry;
            GetAll(&arry);
            // 2.添加到Json::Value
            Json::Value root;
            for (int i = 0; i < arry.size(); ++i)
            {
                Json::Value item;
                item["pack_flag"] = arry[i].pack_flag;
                item["fsize"] = (Json::Int64)arry[i].fsize;
                item["atime"] = (Json::Int64)arry[i].atime;
                item["mtime"] = (Json::Int64)arry[i].mtime;
                item["real_path"] = arry[i].real_path;
                item["pack_path"] = arry[i].pack_path;
                item["url"] = arry[i].url;
                root.append(item);
            }
            // 3.对Json::Value序列化
            std::string body;
            JsonUtil::Serialize(root, &body);
            // 4.写文件cloud.dat
            FileUtil fu(_backup_file);
            fu.SetContent(body);
            return true;
        }
    private:
        std::string _backup_file; // cloud.dat
        pthread_rwlock_t _rwlock;
        std::unordered_map<std::string, BackupInfo> _table;
    };
}