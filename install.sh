#!/bin/bash

# remove old nginx install
rm -rf /usr/local/nginx/

# kill all nginx processes
pkill nginx

# install build dependencies
apt install -y libpcre3 libpcre3-dev zlib1g zlib1g-dev openssl libssl-dev

# if on centos do these
# yum install -y pcre pcre-devel zlib zlib-devel openssl openssl-devel

# pull down nginx
wget https://nginx.org/download/nginx-1.10.3.tar.gz
tar zxf nginx-1.10.3.tar.gz
cd nginx-1.10.3

# add our module as a dependency
./configure --add-module=../ --with-http_ssl_module --with-ipv6

# compile and install
make && make install

# clean up
cd ..
rm -rf nginx-1.10.3/ nginx-1.10.3.tar.gz 
