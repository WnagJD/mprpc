#include "zookeeperutil.h"
#include "mprpcapplication.h"

void global_watcher(zhandle_t* zh, int type, int state,
                    const char* path, void* watherCtx)
{
    if(type == ZOO_SESSION_EVENT)
    {
        if(state == ZOO_CONNECTED_STATE)
        {
            sem_t* sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}
                    
                


ZkClient::ZkClient() : m_zhandle(nullptr)
{
}

~ZkClient::ZkClient()
{
    if(m_zhandle!=nullptr)
    {
        //关闭句柄，释放资源
        zookeeper_close(m_zhandle);
    }
}

void ZkClient::Start()
{
    std::string ip = MprpcApplication::GetInstance()::GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance()::GetConfig().Load("zookeeperport");
    std::string constr = ip+":"+port;

    //使用zookeeper_mt 多线程版本
    //包括三个线程
    //主线程(API调用线程)
    //网络I/O线程 pthread_create poll
    //watcher回调线程 pthread_create 

    m_zhandle = zookeeper_init(constr.c_str(),global_watcher, 30000, nullptr, nullptr, 0);
    if(m_zhandle==nullptr)
    {
        std::cout<<"zookeeper 连接失败"<<std::endl;
        exit(EXIT_FAILURE);
    }

    // zookeeper_init是异步函数,当函数被调用后,当前函数就结束,函数的成功调用，并不意味着连接建立成功
    // 需要等zkserver发回返回消息,被设置的global_watcher回调函数捕获
    // 进而需要使用信号量来进行同步的判定
    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(m_zhandle, &sem);
    sem_wait(&sem);
    std::cout<<"zookeeper_init success!"<<std::endl;
    
}

void ZkClient::Create(const char* path, const char* data, int datalen, int state)
{
    char buf[128]={0};
    int buflen = sizeof(buf);
    int flag;
    flag = zoo_exist(m_zhandle, path, 0,nullptr);
    if(flag == ZNONODE)
    {
        flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, buf, buflen);
        if(flag ==ZOK)
        {
            std::cout<<"create znode:"<<path<<" success!"<<std::endl;
        }
        else{
            std::cout<<"flag:"<<flag<<endl;
            std::cout<<"znode create error...path: "<<path<<std::endl;
            exit(EXIT_FAILURE);
        }
    }

}

std::string ZkClient(const char* path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, bufferlen, nullptr);
    if(flag != ZOK)
    {
        std::cout<<"get znode error...path:"<<path<<std::endl;
        return "";
    }else
    {
        return buffer;
    }
}