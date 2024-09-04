#pragma  once
#include "mprpcconfig.h"

//单例对象
class MprpcApplication
{
    public:
        //框架初始化
        static void Init(int argc, char** argv);
        static MprpcApplication& GetInstance();
        static MprpcConfig& GetConfig();
    private:

        static MprpcConfig m_config;
        //默认构造函数
        MprpcApplication(){}
        //禁用复制构造函数
        MprpcApplication(const MprpcApplication&) = delete;
        //禁用移动构造函数
        MprpcApplication(MprpcApplication&&) =delete;

};