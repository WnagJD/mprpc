#pragma once
#include <string>
#include "lockqueue.h"

//日志级别
enum LogLevel{
    INFO, //普通信息级别
    ERROR //错误信息级别
};
#define LOG_INFO(logmsgformat, ...) \
        do \
        {\
            Logger& logger = Logger::GetInstall();\
            logger.SetLogLevel(INFO);\
            char buf[1024]={0};\
            snprintf(buf,1024,logmsgformat, ##__VA_ARGS__);\
            logger.Log(buf);\
        }while(0) 
    

#define LOG_ERROR(logmsgformat, ...) \
        do \
        {\
            Logger& logger = Logger::GetInstall();\
            logger.SetLogLevel(ERROR);\
            char buf[1024]={0};\
            snprintf(buf,1024,logmsgformat, ##__VA_ARGS__);\
            logger.Log(buf);\
        }while(0)



class Logger
{
    public:
        //获取单例对象
        static Logger& GetInstall();

        //写日志
        void Log(std::string message);

        //设置日志级别
        void SetLogLevel(LogLevel level);

    private:
        LockQueue<std::string> m_lockqueue;
        LogLevel m_loglevel;

        Logger();

        //禁止复制构造函数和移动构造函数
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;


};


