#include "user.pb.h"
#include <string>
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"
// using namespace mprpc;
//将本地服务发布为rpc服务

class UserService:public mprpc::UserServiceRpc
{
    public:
        bool Login(std::string name, std::string pwd)
        {
            std::cout<<"do local login service!"<<std::endl;
            std::cout<<"name"<<name<<" pwd"<<pwd<<std::endl;
            return true;
        }

        void Login(::google::protobuf::RpcController* controller,
                       const ::mprpc::LoginRequest* request,
                       ::mprpc::LoginResponse* response,
                       ::google::protobuf::Closure* done)
        {
            std::string name = request->name();
            std::string pwd = request->pwd();

            bool login_result = Login(name, pwd);

            if(login_result)
            {
                response->set_success(login_result);
                mprpc::ResultCode* result = response->mutable_result();
                result->set_errcode(0);
                result->set_errmessage("");
            }else{
                response->set_success(login_result);
                mprpc::ResultCode*result = response->mutable_result();
                result->set_errcode(-1);
                result->set_errmessage("login failure!");
            }

            done->Run();

        }

};


int main(int argc, char** argv)
{
    LOG_INFO("%s:%s:%d:%s",__FILE__,__FUNCTION__,__LINE__,"exec_userservice!");
    // LOG_INFO("%s","nihao!");
    //使用框架
    //框架的基础类  框架的初始化 加载配置文件
    
    MprpcApplication::Init(argc, argv);


    // //provider 是一个网络服务对象,将userservice服务发布到rpc节点上
    RpcProvider provider;

    provider.NotifyService(new UserService());

    // //启动一个rpc服务发布节点 Run以后，进程进入阻塞状态
    provider.Run();

    return 0;

}