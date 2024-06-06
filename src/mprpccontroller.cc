#include "mprpccontroller.h"

MprpcConntroller::MprpcConntroller()
{
    m_failed = false;
    m_errText="";
}

void MprpcConntroller::Reset()
{
    m_failed = false;
    m_errText="";
}

bool MprpcConntroller::Failed() const
{
    return m_failed;
}

std::string MprpcConntroller::ErrorText() const
{
    return m_errText;
}

void MprpcConntroller::SetFailed(const std::string& reason)
{
    m_failed = false;
    m_errText = reason;
}

void MprpcConntroller::StartCancel()
{

}

bool MprpcConntroller::IsCanceled() const
{
    return false;
}
void MprpcConntroller::NotifyOnCancel(google::protobuf::Closure* callback)
{
    
}