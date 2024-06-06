#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
#include "logger.h"
#include "mprpccontroller.h"

int main(int argc, char**argv)
{
    //配置rpc框架，加载配置文件
    MprpcApplication::Init(argc, argv);


    //服务消费端使用UserServiceRpc_Stub来进行调用远程rpc方法
    mprpc::UserServiceRpc_Stub stub(new MprpcChannel());

    //组装rpc远程调用LoginRequest
    mprpc::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    mprpc::LoginResponse response;
    MprpcConntroller controller;

    //login低层调用的是MprpcChannel的CallMethod方法
    stub.Login(&controller,&request, &response, nullptr);

    if(response.result().errcode()==0)
    {

        std::cout<<"rpc login response success: "<<response.success()<<std::endl;
    }else{
        std::cout<<"rpc login response error: "<<response.success()<<std::endl;
    }




    return 0;
}