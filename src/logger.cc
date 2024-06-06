#include "logger.h"
#include <thread>
#include <time.h>
#include <iostream>


//写日志
void Logger::Log(std::string message)
{
    m_lockqueue.Push(message);
}

Logger::Logger()
{
    
    std::thread log_thread([&] 
    {
        //构建匿名的函数
        for(;;)
        {
            time_t te = time(nullptr);
            struct tm* logtm = localtime(&te);

            //构建日志文件名    
            char filename[512]={0};
            sprintf(filename, "%d-%d-%d", logtm->tm_year+1900, logtm->tm_mon+1, logtm->tm_mday);

            FILE * fileptr = fopen(filename,"a+");
            if(fileptr==nullptr)
            {
                std::cout<<"logfile:"<< filename <<"open error!"<<std::endl;
                exit(EXIT_FAILURE);
            }

            std::string message = m_lockqueue.Pop();


            //加上时间
            char buf[128]={0};
            sprintf(buf,"%d-%d-%d=>>[%s]", logtm->tm_hour,logtm->tm_min,
                                    logtm->tm_sec,
                                    (m_loglevel == INFO ? "INFO" : "ERROR"));
            message.insert(0,std::string(buf));
            message.append("\n");

            fputs(message.c_str(), fileptr);
            fclose(fileptr);
        }

    });

    //设置线程分离
    log_thread.detach();
}

//设置日志级别
void::Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = level;
}

//获取单例对象
Logger& Logger::GetInstall()
{
    static Logger logger;
    return logger;
}