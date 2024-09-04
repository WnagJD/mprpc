#include "mprpcchannel.h"
#include <google/protobuf/descriptor.h>
#include <rpcheader.pb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mprpcapplication.h"
#include <unistd.h>
#include "zookeeperutil.h"






void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
    //根据mthoed的描述符查询到对应的service的描述符，进而查询到更多的有关服务的信息
    const google::protobuf::ServiceDescriptor* serviceptr = method->service();

    //构建传输用的protobuf
    //head_size+head_str(service_name method_name args_size)+args
    std::string service_name = serviceptr->name();
    std::string method_name = method->name();

    std::string args;
    if(!request->SerializeToString(&args))
    {
        // std::cout<<" request serialize error!"<<std::endl;
        controller->SetFailed("request serialize error!");
        return ;
    }
    mprpcs::RpcHeader rpcheader;
    rpcheader.set_servicer_name(service_name.c_str());
    rpcheader.set_method_name(method_name.c_str());
    rpcheader.set_args_size(args.size());

    //head_str
    std::string head_str;
    rpcheader.SerializeToString(&head_str);
    uint32_t head_size = head_str.size();

    //发送的rpc请求信息
    std::string send_buf;

    send_buf.insert(0, std::string((char*)&head_size,4));
    send_buf +=head_str;
    send_buf +=args;

    //打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << head_size << std::endl; 
    std::cout << "rpc_header_str: " << head_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_str: " << args << std::endl; 
    std::cout << "============================================" << std::endl;

    //使用socket Tcp连接远程rpc服务
    int clientfd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    ZkClient client;
    client.Start();
    std::string path = "/" + service_name+ "/"+ method_name;
    std::string ip_port= client.GetData(path.c_str());
    if(ip_port.empty())
    {
        controller->SetFailed("rpc method not exist!");
        return ;
    }

    int idx = ip_port.find_first_of(":");
    if(idx == -1)
    {
        controller->SetFailed("address is invalid!")
        return ;
    }
    std::string ip = ip_port.substr(0,idx);
    std::string port = ip_port.substr(idx+1, ip_port.size() - idx -1);
    serveraddr.sin_port = htons(atoi(ip.c_str()));
    serveraddr.sin_addr.s_addr = inet_addr(port.c_str());

    // serveraddr.sin_port = htons(atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str()));
    // serveraddr.sin_addr.s_addr = inet_addr(MprpcApplication::GetInstance().GetConfig().Load("rpcserverip").c_str());

    if(-1 == connect(clientfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)))
    {
        close(clientfd);
        // std::cout<<"connect rpc server error"<<std::endl;
        controller->SetFailed("connect rpc server error");
        return;
    }

    int send_num = send(clientfd, send_buf.c_str(), send_buf.size(), 0);
    if(send_num == -1)
    {
        close(clientfd);
        // std::cout<<"send rpc request error!"<<std::endl;
        controller->SetFailed("send rpc request error!");
        return ;
    }

    char recv_buf[1024]={0};
    //同步 阻塞
    int recv_num = recv(clientfd,recv_buf,1024,0);
    if(recv_num == -1)
    {
        close(clientfd);
        // std::cout<<"recv rpc response error!"<<std::endl;
        controller->SetFailed("recv rpc response error!");
        return ;
    }

    //从字节数组中解析protocol buffer
    if(!response->ParseFromArray(recv_buf, recv_num))
    {
        close(clientfd);
        // std::cout<<" response parse error!"<<std::endl;
        controller->SetFailed("response parse error!");
        return;
    }

    close(clientfd);
}