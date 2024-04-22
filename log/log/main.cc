#include "log.hpp"

void test_log(std::string name)
{
    INFO("%s", "测试开始");
    log::Logger::ptr logger = log::LoggerManager::getInstance().getLogger(name);
    DEBUG("%s", "测试日志");
    INFO("%s", "测试日志");
    WARN("%s", "测试日志");
    logger->debug("%s", "测试日志");
    logger->info("%s", "测试日志");
    logger->error("%s", "测试日志");
    logger->warn("%s", "测试日志");
    logger->fatal("%s", "测试日志");
    log::getLogger(name)->debug("%s", "测试日志");
    log::getLogger(name)->info("%s", "测试日志");
    log::getLogger(name)->error("%s", "测试日志");
    // log::getLogger(name)->fatal("%s", "测试日志");
    // log::getLogger(name)->error("%s", "测试日志");
    // log::getLogger(name)->fatal("%s", "测试日志");
    INFO("%s", "测试结束");
}

int main()
{
    std::unique_ptr<log::LoggerBuilder> builder(new log::GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger");
    builder->buildLoggerLevel(log::LogLevel::value::WARN);
    builder->buildFormatter();
    builder->buildLoggerType(log::LoggerType::LOGGER_ASYNC);
    builder->buildSink<log::StdoutSink>();
    builder->buildSink<log::FileSink>("./logfile/async.log");
    builder->buildSink<log::RollBySizeSink>("./logfile/roll_async_size", 1024 * 1024);
    builder->build();
    test_log("async_logger");

    return 0;
}

// void test_log()
// {
// logger中getInstance测试
// log::Logger::ptr logger = log::LoggerManager::getInstance().getLogger("async_logger");
// logger测试
// logger->debug("%s", "测试日志");
// logger->info("%s", "测试日志");
// logger->warn("%s", "测试日志");
// logger->error("%s", "测试日志");
// logger->fatal("%s", "测试日志");
// size_t count = 0;
// while (count < 30000)
// {
//     logger->fatal("测试日志_%d", count++);
// }

//     DEBUG("%s", "测试日志");
//     INFO("%s", "测试日志");
//     WARN("%s", "测试日志");
//     ERROR("%s", "测试日志");
//     FATAL("%s", "测试日志");
//     size_t count = 0;
//     while (count < 30000)
//     {
//         logger->fatal("测试日志_%d", count++);
//     }
// }
// int main()
// {
//     std::unique_ptr<log::LoggerBuilder> builder(new log::GlobalLoggerBuilder());
//     builder->buildLoggerName("async_logger");
//     builder->buildLoggerLevel(log::LogLevel::value::WARN);
//     builder->buildFormatter();
//     builder->buildLoggerType(log::LoggerType::LOGGER_ASYNC);
//     builder->buildEnableUnSafeAsync();
//     builder->buildSink<log::FileSink>("./logfile/async.log");
//     builder->buildSink<log::StdoutSink>();
//     builder->build();
//     test_log();

// logger中GlobalLoggerbuilder测试
// std::unique_ptr<log::LoggerBuilder> builder(new log::GlobalLoggerBuilder());
// uilder->buildLoggerName("async_logger");
// builder->buildLoggerLevel(log::LogLevel::value::WARN);
// builder->buildFormatter("[%c]%m%n");
// builder->buildLoggerType(log::LoggerType::LOGGER_ASYNC);
// builder->buildEnableUnSafeAsync();
// builder->buildSink<log::FileSink>("./logfile/async.log");
// builder->buildSink<log::StdoutSink>();
// builder->build();
// test_log();

// log::Logger::ptr logger = log::LoggerManager::getInstance().getLogger("async_logger");
// logger测试
// logger->debug(__FILE__, __LINE__, "%s", "测试日志");
// logger->info(__FILE__, __LINE__, "%s", "测试日志");
// logger->warn(__FILE__, __LINE__, "%s", "测试日志");
// logger->error(__FILE__, __LINE__, "%s", "测试日志");
// logger->fatal(__FILE__, __LINE__, "%s", "测试日志");
// size_t count = 0;
// while (count < 300000)
// {
//     logger->fatal(__FILE__, __LINE__, "测试日志_%d", count++);
// }
// buffer测试
// std::ifstream ifs("./logfile/test.log", std::ios::binary);
// if (ifs.is_open() == false)
//     return -1;
// ifs.seekg(0, std::ios::end);
// size_t fsize = ifs.tellg();
// ifs.seekg(0, std::ios::beg);
// std::string body;
// body.resize(fsize);
// ifs.read(&body[0], fsize);
// if (ifs.good() == false)
// {
//     std::cout << "read error" << std::endl;
//     return -1;
// }
// ifs.close();

// log::Buffer buffer;
// buffer.push(body.c_str(), body.size());
// std::cout << body << std::endl;
// for (int i = 0; i < body.size(); ++i)
//     buffer.push(&body[i], 1);
// std::ofstream ofs("./logfile/tmp.log", std::ios::binary);
// size_t rsize = buffer.readAbleSize();
// for (int i = 0; i < rsize; ++i)
// {
//     ofs.write(buffer.begin(), 1);
//     buffer.moveReader(1);
// }
// ofs.close();

// Sync测试
// std::string logger_name = "sync_logger";
// log::LogLevel::value limit = log::LogLevel::value::WARN;
// log::Formatter::ptr fmt(new log::Formatter("[%d{%H:%M:%S}][%c][%f:%l][%p]%T%m%n"));
// log::LogSink::ptr stdout_lsp = log::SinkFactory::create<log::StdoutSink>();
// log::LogSink::ptr file_lsp = log::SinkFactory::create<log::FileSink>("./logfile/test.log");
// log::LogSink::ptr roll_lsp = log::SinkFactory::create<log::RollBySizeSink>("./logfile/roll_", 1024 * 1024);
// // std::vector<log::LogSink::ptr> sinks = { stdout_lsp, file_lsp, roll_lsp };
// std::vector<log::LogSink::ptr> sinks = { file_lsp, roll_lsp };
// log::Logger::ptr logger(new log::SyncLogger(logger_name, limit, fmt, sinks));

// Async测试
// std::string logger_name = "async_logger";
// log::LogLevel::value limit = log::LogLevel::value::WARN;
// log::Formatter::ptr fmt(new log::Formatter("[%d{%H:%M:%S}][%c][%f:%l][%p]%T%m%n"));
// log::LogSink::ptr stdout_lsp = log::SinkFactory::create<log::StdoutSink>();
// log::LogSink::ptr file_lsp = log::SinkFactory::create<log::FileSink>("./logfile/async.log");
// log::LogSink::ptr roll_lsp = log::SinkFactory::create<log::RollBySizeSink>("./logfile/roll_", 1024 * 1024);
// std::vector<log::LogSink::ptr> sinks = { stdout_lsp, file_lsp, roll_lsp };
// std::vector<log::LogSink::ptr> sinks = { file_lsp };
// log::Logger::ptr logger(new log::SyncLogger(logger_name, limit, fmt, sinks));
// log::Logger::ptr logger = log::LoggerManager::getInstance().getLogger("async_logger");
// logger测试
// logger->debug(__FILE__, __LINE__, "%s", "测试日志");
// logger->info(__FILE__, __LINE__, "%s", "测试日志");
// logger->warn(__FILE__, __LINE__, "%s", "测试日志");
// logger->error(__FILE__, __LINE__, "%s", "测试日志");
// logger->fatal(__FILE__, __LINE__, "%s", "测试日志");
// size_t count = 0;
// while (count < 300000)
// {
//     logger->fatal(__FILE__, __LINE__, "测试日志_%d", count++);
// }
// builder测试
// std::unique_ptr<log::LoggerBuilder> builder(new log::LocalLoggerBuild());
// builder->buildLoggerName("sync_logger");
// builder->buildLoggerLevel(log::LogLevel::value::WARN);
// builder->buildLoggerType(log::LoggerType::LOGGER_SYNC);
// builder->buildFormatter();
// builder->buildSink<log::StdoutSink>();
// builder->buildSink<log::FileSink>("./logfile/test.log");
// builder->buildSink<log::RollBySizeSink>("./logfile/roll_", 1024 * 1024);
// log::Logger::ptr logger = builder->build();

// log::LogMsg msg(log::LogLevel::value::INFO, 53, "main.cc", "root", "格式化功能测试...");
// log::Formatter fmt;
// std::string str = fmt.format(msg);
// log::LogSink::ptr time_lsp = log::SinkFactory::create<RollByTimeSize>("./logfile/time", TimeGap::GAP_SECOND);
// int n = 5;
// while (n--)
// {
//     time_lsp->log(str.c_str(), str.size());
//     sleep(1);
// }
// log::LogSink::ptr stdout_lsp = log::SinkFactory::create<log::StdoutSink>();
// log::LogSink::ptr file_lsp = log::SinkFactory::create<log::FileSink>("./logfile/test.log");
// log::LogSink::ptr roll_lsp = log::SinkFactory::create<log::RollBySizeSink>("./logfile/roll_", 1024 * 1024);
// stdout_lsp->log(str.c_str(), str.size());
// file_lsp->log(str.c_str(), str.size());
// size_t cursize = 0, count = 0;
// while (cursize < 1024 * 1024 * 10)
// {
//     std::string tmp = std::to_string(count++);
//     tmp += str;
//     roll_lsp->log(tmp.c_str(), tmp.size());
//     cursize += tmp.size();
// }

// std::string name = "test0";
// std::cout << log::util::Date::now() << std::endl;
// std::cout << log::util::File::exists(name) << std::endl;
// std::string path = "../afd/b/c/def";
// log::util::File::create_directory(path);

//     return 0;
// }