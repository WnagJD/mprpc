#pragma once
#include <google/protobuf/service.h>


class MprpcConntroller: public google::protobuf::RpcController
{
    public:
        MprpcConntroller();
        void Reset();
        bool Failed() const;
        std::string ErrorText() const;
        void SetFailed(const std::string& reason);

        void StartCancel();
        bool IsCanceled() const;
        void NotifyOnCancel(google::protobuf::Closure* callback);
    
    private:
        //rpc 方法执行过程中的状态
        bool m_failed;
        //rpc 方法执行过程中产生的错误信息
        std::string m_errText;

};