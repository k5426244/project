#pragma once

#include "data.hpp"
#include <unistd.h>

extern cloud::DataManager *_data;
namespace cloud
{
    class HotManager
    {
    public:
        HotManager()
        {
            Config *config = Config::GetInstance();
            _back_dir = config->GetBackDir();
            _pack_dir = config->GetPackDir();
            _pack_suffix = config->GetPackFileSuffix();
            _hot_time = config->GetHotTime();
            FileUtil tmp1(_back_dir);
            FileUtil tmp2(_pack_dir);
            tmp1.CreateDirectory();
            tmp2.CreateDirectory();
        }
        // 判断文件是否为热点文件，如果是则压缩后放到pack_dir目录中
        bool RunModule()
        {
            while (true)
            {
                // 1.遍历文件目录,获取所有文件名
                FileUtil fu(_back_dir);
                std::vector<std::string> arry;
                fu.ScanDirectory(&arry);
                // 2.遍历判断文件是否是非热点文件
                for (auto &a : arry)
                {
                    if (HotJudge(a) == false)
                        continue;
                    // 3.获取文件备份信息
                    BackupInfo info;
                    if (_data->GetRealPath(a, &info) == false)
                    {
                        // 有一个文件存在，但没有备份信息,视为新增文件
                        info.NewBackupInfo(a);
                    }
                    // 4.对非热点文件进行压缩处理
                    FileUtil tmp(a);
                    tmp.Compress(info.pack_path);
                    // 5.删除源文件，修改备份信息
                    tmp.Remove();
                    info.pack_flag = true;
                    _data->Updata(info);
                }
                sleep(5);
            }
            return true;
        }

    private:
        // 非热点文件返回真，热点文件返回假
        bool HotJudge(const std::string &filename)
        {
            FileUtil fu(filename);
            time_t last_atime = fu.LastATime();
            time_t cur_time = time(NULL);
            if (cur_time - last_atime > _hot_time)
                return true;
            return false;
        }

    private:
        std::string _back_dir;
        std::string _pack_dir;
        std::string _pack_suffix;
        int _hot_time;
    };
}