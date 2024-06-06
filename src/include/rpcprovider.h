#pragma once
#include <google/protobuf/service.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include "mprpcapplication.h"
#include <unordered_map>
#include "logger.h"

class RpcProvider
{
    public:
        void Run();
        void NotifyService(google::protobuf::Service * service);

    private:
        muduo::net::EventLoop Loop;
        void onConnection(const muduo::net::TcpConnectionPtr& conn);
        void onMessage(const muduo::net::TcpConnectionPtr&,
                            muduo::net::Buffer*,
                            muduo::Timestamp);
        
        void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message* );
        
        struct ServiceInfo{
            google::protobuf::Service* m_service;
            std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;
        };
        
        std::unordered_map<std::string, RpcProvider::ServiceInfo> m_serviceMap;

};