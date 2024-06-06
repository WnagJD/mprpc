aux_source_directory(. SRC_LIST)


#添加的是静态库 STATIC  SHARED
add_library(mprpc ${SRC_LIST})
target_link_libraries(mprpc muduo_net muduo_base protobuf  zookeeper_mt pthread)