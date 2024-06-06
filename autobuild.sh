#!/bin/bash


#脚本文件中的任何一个命令执行失败时,立即停止脚本文件的执行
set -e


#&& 逻辑与运算  只有当前一个命令的返回结果是0(代表执行成功)时,后续的命令才会执行
rm -rf `pwd`/build/*
cd `pwd`/build &&
    cmake .. &&
    make

cd ..
cp -r `pwd`/src/include `pwd`/lib