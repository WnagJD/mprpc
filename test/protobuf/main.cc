#include "test.pb.h"
#include <iostream>
#include <string>
using namespace mprpc;

int main()
{
    LoginResponse res;
    res.set_success(false);
    ResultCode* code = res.mutable_result();
    code->set_errcode(1);
    code->set_errmessage("exec failure");

    GetFriendListsResponse resp;
    ResultCode* codel = resp.mutable_result();
    codel->set_errcode(0);
    User *user1 = resp.add_friend_list();
    user1->set_age(22);
    user1->set_name("zhang san");
    user1->set_sex(User::MAN);

    User* user2 = resp.add_friend_list();
    user2->set_age(23);
    user2->set_name("Li si");
    user2->set_sex(User::WOMAN);

    std::cout<<resp.friend_list_size()<<std::endl;

    return 0;
}


int main1()
{
    LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    std::string send_str;

    //序列化
    if(request.SerializeToString(&send_str))
    {
        std::cout<<send_str.c_str()<<std::endl;
    }
    

    //反序列化

    LoginRequest send_re;
    if(send_re.ParseFromString(send_str))
    {
        std::cout<<send_re.name()<<std::endl;
        std::cout<<send_re.pwd()<<std::endl;
    }
    
    return 0;
}