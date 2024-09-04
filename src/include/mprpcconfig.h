#pragma once

#include <string>
#include <unordered_map>

class MprpcConfig
{
    public:
        //构造函数
        
        //加载解析配置文件
        void LoadConfigFile(const char* config_file);

        //查看配置选项---键值对
        std::string Load(const std::string &key);
    
    private:
        std::unordered_map<std::string, std::string> m_config_file;
        //去掉字符串前后的空字符
        void Trm(std::string& src_buf);
};