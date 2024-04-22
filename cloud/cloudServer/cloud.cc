#include "util.hpp"
#include "config.hpp"
#include "data.hpp"
#include "hot.hpp"
#include "service.hpp"

void fileUtilTest(const std::string &filename)
{
    // cloud::FileUtil fu(filename);
    // std::cout << fu.fileSize() << std::endl;
    // std::cout << fu.lastMTime() << std::endl;
    // std::cout << fu.lastATime() << std::endl;
    // std::cout << fu.fileName() << std::endl;

    // cloud::FileUtil fu(filename);
    // std::string body;
    // fu.getContent(&body);
    // cloud::FileUtil nfu("./hello.txt");
    // nfu.setContent(body);

    // std::string packname = filename + ".lz";
    // cloud::FileUtil fu(filename);
    // fu.compress(packname);
    // cloud::FileUtil pfu(packname);
    // pfu.unCompress("hello.txt");

    cloud::FileUtil fu(filename);
    fu.CreateDirectory();
    std::vector<std::string> arry;
    fu.ScanDirectory(&arry);
    for (auto &a : arry)
        std::cout << a << std::endl;
}
void JsonUtilTest()
{
    const char *name = "小明";
    int age = 19;
    float score[] = {85, 88.5, 99};
    Json::Value root;
    root["姓名"] = name;
    root["年龄"] = age;
    root["成绩"].append(score[0]);
    root["成绩"].append(score[1]);
    root["成绩"].append(score[2]);
    std::string json_str;
    cloud::JsonUtil::Serialize(root, &json_str);
    std::cout << json_str << std::endl;

    Json::Value val;
    cloud::JsonUtil::UnSerialize(json_str, &val);
    std::cout << "姓名：" << val["姓名"].asCString() << std::endl;
    std::cout << "年龄：" << val["年龄"].asInt() << std::endl;
    std::cout << "成绩：";
    for (int i = 0; i < val["成绩"].size(); ++i)
    {
        std::cout << val["成绩"][i] << " ";
    }
    std::cout << std::endl;
}
void ConfigTest()
{
    cloud::Config *config = cloud::Config::GetInstance();
    std::cout << config->GetHotTime() << std::endl;
    std::cout << config->GetServerPort() << std::endl;
    std::cout << config->GetServerIp() << std::endl;
    std::cout << config->GetDownloadPrefix() << std::endl;
    std::cout << config->GetPackFileSuffix() << std::endl;
    std::cout << config->GetPackDir() << std::endl;
    std::cout << config->GetBackDir() << std::endl;
    std::cout << config->GetBackupFile() << std::endl;
}
void DataTest(const std::string &filename)
{
    cloud::DataManager data;
    std::vector<cloud::BackupInfo> arry;
    data.GetAll(&arry);
    for (auto &a : arry)
    {
        std::cout << a.pack_flag << std::endl;
        std::cout << a.fsize << std::endl;
        std::cout << a.mtime << std::endl;
        std::cout << a.atime << std::endl;
        std::cout << a.real_path << std::endl;
        std::cout << a.pack_path << std::endl;
        std::cout << a.url << std::endl;
    }
    // cloud::BackupInfo info;
    // info.NewBackupInfo(filename);
    // std::cout << info.pack_flag << std::endl;
    // std::cout << info.fsize << std::endl;
    // std::cout << info.mtime << std::endl;
    // std::cout << info.atime << std::endl;
    // std::cout << info.real_path << std::endl;
    // std::cout << info.pack_path << std::endl;
    // std::cout << info.url << std::endl;
    // std::cout << "-----------------insert and GetOneByURL----------------" << std::endl;
    // data.Insert(info);
    //cloud::DataManager data;
    // cloud::BackupInfo tmp;
    // data.GetOneByURL("/download/bundle.h", &tmp);
    // std::cout << tmp.pack_flag << std::endl;
    // std::cout << tmp.fsize << std::endl;
    // std::cout << tmp.mtime << std::endl;
    // std::cout << tmp.atime << std::endl;
    // std::cout << tmp.real_path << std::endl;
    // std::cout << tmp.pack_path << std::endl;
    // std::cout << tmp.url << std::endl;
    // std::cout << "-----------------Updata and GetAll----------------" << std::endl;
    // info.pack_flag = true;
    // data.Updata(info);
    // std::vector<cloud::BackupInfo> arry;
    // data.GetAll(&arry);
    // for (auto &a : arry)
    // {
    //     std::cout << a.pack_flag << std::endl;
    //     std::cout << a.fsize << std::endl;
    //     std::cout << a.mtime << std::endl;
    //     std::cout << a.atime << std::endl;
    //     std::cout << a.real_path << std::endl;
    //     std::cout << a.pack_path << std::endl;
    //     std::cout << a.url << std::endl;
    // }
    // std::cout << "-----------------GetOneByPath----------------" << std::endl;
    // data.GetRealPath(filename, &tmp);
    // std::cout << tmp.pack_flag << std::endl;
    // std::cout << tmp.fsize << std::endl;
    // std::cout << tmp.mtime << std::endl;
    // std::cout << tmp.atime << std::endl;
    // std::cout << tmp.real_path << std::endl;
    // std::cout << tmp.pack_path << std::endl;
    // std::cout << tmp.url << std::endl;
}
cloud::DataManager *_data;
void HotTest()
{
    cloud::HotManager hot;
    hot.RunModule();
}
void ServiceTest()
{
    cloud::Service srv;
    srv.RunModule();
}
int main(int argc, char *argv[])
{
    _data = new cloud::DataManager();
    // fileUtilTest(argv[1]);
    //  JsonUtilTest();
    //  ConfigTest();
    // DataTest(argv[1]);
    // HotTest();
    // ServiceTest();
    std::thread thread_hot_manager(HotTest);
    std::thread thread_service(ServiceTest);
    thread_hot_manager.join();
    thread_service.join();
    return 0;
}