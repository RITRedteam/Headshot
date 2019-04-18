#!/bin/bash

# remove old nginx install
rm -rf /usr/local/nginx/

# kill all nginx processes
pkill nginx

# install build dependencies
apt install -y libpcre3 libpcre3-dev zlib1g zlib1g-dev openssl libssl-dev libxslt1-dev libgd-dev libgeoip-dev

# if on centos do these
# yum install -y pcre pcre-devel zlib zlib-devel openssl openssl-devel

# pull down nginx
wget https://nginx.org/download/nginx-1.10.3.tar.gz
tar zxf nginx-1.10.3.tar.gz
cd nginx-1.10.3

# add our module as a dependency
./configure --add-module=../ --user=root --with-cc-opt='-g -O2 -fPIE -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2' --with-ld-opt='-Wl,-Bsymbolic-functions -fPIE -pie -Wl,-z,relro -Wl,-z,now' --prefix=/usr/share/nginx --conf-path=/etc/nginx/nginx.conf --http-log-path=/var/log/nginx/access.log --error-log-path=/var/log/nginx/error.log --lock-path=/var/lock/nginx.lock --pid-path=/run/nginx.pid --http-client-body-temp-path=/var/lib/nginx/body --http-fastcgi-temp-path=/var/lib/nginx/fastcgi --http-proxy-temp-path=/var/lib/nginx/proxy --http-scgi-temp-path=/var/lib/nginx/scgi --http-uwsgi-temp-path=/var/lib/nginx/uwsgi --with-debug --with-pcre-jit --with-ipv6 --with-http_ssl_module --with-http_stub_status_module --with-http_realip_module --with-http_auth_request_module --with-http_addition_module --with-http_dav_module --with-http_geoip_module --with-http_gunzip_module --with-http_gzip_static_module --with-http_image_filter_module --with-http_v2_module --with-http_sub_module --with-http_xslt_module --with-stream --with-stream_ssl_module --with-mail --with-mail_ssl_module --with-threads

# compile and install
make && make install

# clean up
cd ..
rm -rf nginx-1.10.3/ nginx-1.10.3.tar.gz 
