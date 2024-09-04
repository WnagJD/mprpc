#include "mprpcconfig.h"
#include <iostream>


void MprpcConfig::LoadConfigFile(const char* configfile)
{
    FILE* ptr = fopen(configfile,"r");
    if(ptr==nullptr)
    {
        //日志
        std::cout<<"file is not exist!"<<std::endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(ptr))
    {   
        char buf[512]={0};
        fgets(buf, 512, ptr);
        std::string read_buf(buf);
        Trm(read_buf);

        if(read_buf[0]=='#' || read_buf.empty())
        {
            continue;
        }

        int idx = read_buf.find('=');

        if(idx ==-1)
        {
            //配置不合法
            continue;
        }

        std::string key = read_buf.substr(0, idx);
        Trm(key);

        int endidx = read_buf.find('\n');
        std::string value = read_buf.substr(idx+1, endidx-idx-1);
        Trm(value);
        
        m_config_file.insert({key,value});

    }
    fclose(ptr);
}


void MprpcConfig::Trm(std::string& buf)
{
    //去掉字符串前面的空格
    int idx = buf.find_first_not_of(' ');
    if(idx!=-1)
    {
        buf = buf.substr(idx, buf.size()-idx);
    }

    //去掉字符串后面的空格
    int endidx = buf.find_last_not_of(' ');
    if(endidx!=-1)
    {
        buf = buf.substr(0, endidx+1);
    }

}

std::string MprpcConfig::Load(const std::string &key)
{
    auto it = m_config_file.find(key);
    if(it==m_config_file.end())
    {
       return "";
    }
     return it->second;
}