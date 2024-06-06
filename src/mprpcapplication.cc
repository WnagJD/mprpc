#include "mprpcapplication.h"
#include <unistd.h>
#include <iostream>

MprpcConfig MprpcApplication::m_config;

void ShowArgHelp()
{
    std::cout<<"format: command -i <configfile.conf>"<<std::endl;
}


void MprpcApplication::Init(int argc, char** argv)
{
    //加载配置文件
    if(argc <2)
    {
        ShowArgHelp();
        exit(EXIT_FAILURE);
    }

    int res =0;
    std::string configfile;
    //getopt一次只解析一个选项，-1表示命令行参数解析完
    while((res=getopt(argc, argv, "i:"))!=-1)
    {
        switch(res)
        {
            case 'i': configfile = optarg;break;
            case ':': ShowArgHelp();exit(EXIT_FAILURE);
            case '?': ShowArgHelp();exit(EXIT_FAILURE);
            default: break;
        }
    }
    //加载配置文件 rpcserverip rpcserverport zookeeperip zookeeperport
    m_config.LoadConfigFile(configfile.c_str());

    std::cout<<"rpcserverip:"<<m_config.Load("rpcserverip")<<std::endl;
    std::cout<<"rpcserverport:"<<m_config.Load("rpcserverport")<<std::endl;
    std::cout<<"zookeeperip:"<<m_config.Load("zookeeperip")<<std::endl;
    std::cout<<"zookeeperport:"<<m_config.Load("zookeeperport")<<std::endl;

}

MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::GetConfig()
{
    return m_config;
}


