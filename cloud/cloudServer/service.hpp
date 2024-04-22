#pragma once

#include "data.hpp"
#include "httplib.h"

extern cloud::DataManager *_data;
namespace cloud
{
    class Service
    {
    public:
        Service()
        {
            Config *config = Config::GetInstance();
            _server_ip = config->GetServerIp();
            _server_port = config->GetServerPort();
            _download_prefix = config->GetDownloadPrefix();
        }
        bool RunModule()
        {
            _server.Post("/upload", Upload);
            _server.Get("/listshow", ListShow);
            _server.Get("/", ListShow);
            std::string download_url = _download_prefix + "(.*)";
            _server.Get(download_url, Download);
            _server.listen(_server_ip.c_str(), _server_port);
            return true;
        }

    private:
        static void Upload(const httplib::Request &req, httplib::Response &rsp)
        {
            // post /upload 文件数据在正文中
            auto ret = req.has_file("file"); // 判断有没有上传的文件区域
            if (ret == false)
            {
                rsp.status = 400;
                return;
            }
            const auto &file = req.get_file_value("file");
            // file.filename//文件名称 file.content//文件数据
            std::string back_dir = Config::GetInstance()->GetBackDir();
            std::string realpath = back_dir + FileUtil(file.filename).FileName();
            FileUtil fu(realpath);
            fu.SetContent(file.content); // 将数据写入文件中
            BackupInfo info;
            info.NewBackupInfo(realpath); // 组织备份的文件信息
            _data->Insert(info);          // 向数据管理模块添加备份的文件信息
            return;
        }
        static void ListShow(const httplib::Request &req, httplib::Response &rsp)
        {
            // 1.获取所有的文件备份信息
            std::vector<BackupInfo> arry;
            _data->GetAll(&arry);
            // 2. 根据所有的备份信息，组织html文件数据
            std::stringstream ss;
            ss << "<html><head><title>Download</title></head>";
            ss << "<body><h1>Download</h1><table>";
            for (auto &a : arry)
            {
                ss << "<tr>";
                std::string filename = FileUtil(a.real_path).FileName();
                ss << "<td><a href='" << a.url << "'>" << filename << "</a></td>";
                ss << "<td align='right'>" << std::ctime(&a.mtime) << "</td>";
                ss << "<td align='right'>" << a.fsize / 1024 << "k</td>";
                ss << "</tr>";
            }
            ss << "</table></body></html>";
            rsp.body = ss.str();
            rsp.set_header("Content-Type", "text/html");
            rsp.status = 200;
            return;
        }
        static void Download(const httplib::Request &req, httplib::Response &rsp)
        {
            // 1.获取客户端请求的资源路径path req.path
            // 2.根据资源路径，获取文件备份信息
            BackupInfo info;
            _data->GetOneByURL(req.path, &info);
            // 3.判断文件是否被压缩，如果被压缩，要先解压缩
            if (info.pack_flag == true)
            {
                FileUtil fu(info.pack_path);
                fu.UnCompress(info.real_path);
                // 4.删除压缩包，修改备份信息(已经没有被压缩)
                fu.Remove();
                info.pack_flag = false;
                _data->Updata(info);
            }

            bool retrans = false;
            std::string old_etag;
            if (req.has_header("If-Range"))
            {
                old_etag = req.get_header_value("If-Range");
                // 有If-Range字段且这个字段的值与请求文件的最新etag一致则符合断点续传
                if (old_etag == GetETag(info))
                    retrans = true;
            }
            // 4.读取文件数据，放入rsp.body中
            FileUtil fu(info.real_path);
            if (retrans == false)
            {
                fu.GetContent(&rsp.body);
                // 5.设置响应头部字段：ETag,  Accept-Ranges:bytes
                rsp.set_header("Accept-Rangs", "bytes");
                rsp.set_header("ETag", GetETag(info));
                rsp.set_header("Content-Type", "application/octet-stream"); // octet-stream二进制流
                rsp.status = 200;
            }
            else
            {
                // httplib内部实现了对于区间请求也就是断点续传请求的处理
                // 只需要我们用户将文件所有数据读取到rsp.body中，它内部会自动根据请求区间，从body中取出指定区间数据进行响应
                //  std::string  range = req.get_header_val("Range"); bytes=start-end
                fu.GetContent(&rsp.body);
                rsp.set_header("Accept-Ranges", "bytes");
                rsp.set_header("ETag", GetETag(info));
                rsp.set_header("Content-Type", "application/octet-stream");
                // rsp.set_header("Content-Range", "bytes start-end/fsize");
                rsp.status = 206; // 区间请求响应 206
            }
        }
        static std::string GetETag(const BackupInfo &info)
        {
            // etg : filename-fsize-mtime
            FileUtil fu(info.real_path);
            std::string etag = fu.FileName();
            etag += "-";
            etag += std::to_string(info.fsize);
            etag += "-";
            etag += std::to_string(info.mtime);
            return etag;
        }
    private:
        std::string _server_ip;
        uint16_t _server_port;
        std::string _download_prefix;
        httplib::Server _server;
    };
}