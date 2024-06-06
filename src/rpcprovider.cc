#include "rpcprovider.h"
#include <functional>
#include <google/protobuf/descriptor.h>
#include "rpcheader.pb.h"
#include "zookeeperutil.h"


void RpcProvider::NotifyService(google::protobuf::Service * service)
{
    //rpc 需要知道那个服务的那个方法被调用
    //server_name ===> service*
                //===> method_name  mmethod* 
    ServiceInfo info;
    info.m_service = service;
    const google::protobuf::ServiceDescriptor* pserviceDsc =  service->GetDescriptor();
    std::string service_name = pserviceDsc->name();
    
    int m_count = pserviceDsc->method_count();
    for(int i=0;i<m_count;i++)
    {   
        const google::protobuf::MethodDescriptor* pmethodDsc = pserviceDsc->method(i);
        std::string method_name = pmethodDsc->name();
        info.m_methodMap.insert({method_name, pmethodDsc});
    }
    
    m_serviceMap.insert({service_name, info});

}

void RpcProvider::Run()
{
    //提供网络服务
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);
    muduo::net::TcpServer tcpserver(&Loop, address,"RpcProvider");
    tcpserver.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    tcpserver.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));

    tcpserver.setThreadNum(4);

    //将rpcprovider上注册的服务发布到zkserver上
    ZkClient client;
    //连接zkserver服务器
    client.Start();
    for(auto& sp : m_serviceMap)
    {
        std::string service_path = "/" + sp.first;
        //注册永久的根节点
        client.Create(service_path.c_str(),nullptr,0);

        for(auto& mp: sp.second.m_methodMap)
        {
            std::string path = service_path + "/" + mp.first;
            char buffer[128]={0};
            sprintf(buffer,"%s:%d",ip.c_str(), port);
            client.Create(path.c_str(), buffer,strlen(buffer), ZOO_EPHEMERAL);
        }
    }



    tcpserver.start();
    Loop.loop();
}
void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
    }
}

//解析Rpc请求
//rpcprovider 和 rpcconsumer 双方协定好通信的规则
//head_size+head(service_name, method_name+args_size+args
//head_size保证接收到的数据正确的用于反序列化
//使用protobuf反序列化的时候，虽然可以直接用对应的message类的对象进行Parse,但是需要保证所传入的string字段的完整性
//从而正确的解析到对应的对象中去

//head_size(4字节)
//1000 “1000”
//10000 的4个字节表示直接发送
//协定rpc框架的发送rpc请求的时候
//使用的是字节流
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn,
                            muduo::net::Buffer* buffer,
                            muduo::Timestamp timestamp)
{

    std::string read_buf = buffer->retrieveAllAsString();
    int head_size =0;
    read_buf.copy((char*)&head_size,4,0);
    std::string head_str = read_buf.substr(4,head_size);

    mprpcs::RpcHeader rpcheader;

    std::string service_name ;
    std::string method_name;
    uint32_t args_size;
    std::string args;


    if(rpcheader.ParseFromString(head_str))
    {
    
        service_name = rpcheader.servicer_name();
        method_name = rpcheader.method_name();
        args_size = rpcheader.args_size();
        args = read_buf.substr(4+head_size,args_size);
        LOG_INFO("%s:%s:%d:%s",__FILE__,__FUNCTION__,__LINE__,service_name.c_str());
    }else{
        LOG_INFO("head_str:%s",head_str.c_str());
        // std::cout<<"head_str:"<<head_str<<std::endl;
        return ;
    }

    //打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << head_size << std::endl; 
    std::cout << "rpc_header_str: " << head_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_size: " << args_size << std::endl; 
    std::cout << "args_str: " << args << std::endl; 
    std::cout << "============================================" << std::endl;

    //获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end())
    {
        std::cout<<service_name<<"is not exist!"<<std::endl;
        return;
    }

    auto itm = it->second.m_methodMap.find(method_name);
    if(itm == it->second.m_methodMap.end())
    {
        std::cout<<service_name<<":"<<method_name<<"is not exist!"<<std::endl;
        return ;
    }

    google::protobuf::Service* serviceptr = it->second.m_service;
    const google::protobuf::MethodDescriptor* methodptr = itm->second;
    
    //Message 类
    // google::protobuf::Message* request =  serviceptr->GetRequestPrototype(methodptr).New();
    auto request = serviceptr->GetRequestPrototype(methodptr).New();
    if(!request->ParseFromString(args))
    {
        std::cout<<"request parse error!"<<std::endl;
        return;
    }

    
    auto response = serviceptr->GetResponsePrototype(methodptr).New();

    google::protobuf::Closure * done = google::protobuf::NewCallback<RpcProvider, 
                                                                    const muduo::net::TcpConnectionPtr&, 
                                                                    google::protobuf::Message*>
                                                                    (this, &RpcProvider::SendRpcResponse, conn, response);

    
    serviceptr->CallMethod(methodptr, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response)
{
    std::string send_buf;
    if(response->SerializeToString(&send_buf))
    {
        conn->send(send_buf);
    }
    else{
        std::cout<<"serialize error!"<<std::endl;
    }

    //模拟http的短连接 rpc服务器主动断开连接
    conn->shutdown();
}