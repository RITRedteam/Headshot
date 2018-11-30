#!/bin/bash

# remove old nginx install
rm -rf /usr/local/nginx/

# kill all nginx processes
pkill nginx

# install build dependencies
yum install -y pcre pcre-devel zlib zlib-devel openssl openssl-devel

# pull down nginx
wget https://nginx.org/download/nginx-1.14.0.tar.gz
tar zxf nginx-1.14.0.tar.gz
cd nginx-1.14.0

# add our module as a dependency
./configure --add-module=../

# compile and install
make && make install

# clean up
rm -rf nx-1.14.0/ nginx-1.14.0.tar.gz 
