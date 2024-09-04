#pragma once
#include <semaphore.h>
#include <string.h>
#include <zookeeper/zookeeper.h>



//封装zookeeper的客户端类
class ZkClient
{
    public:
        ZkClient();
        ~ZkClient();

        //zkclient初始化zkserver连接
        void Start();

        //在zkserver上根据指定的path创建znode节点
        void Create(const char* path, const char* data, int datalen, int state=0);

        //在zkserver上根据指定的path获取znode节点的值
        std::string GetData(const char* path);

    private:
        zhandle_t* m_zhandle;
};
