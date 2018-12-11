#!/bin/bash
GIT_PATH=`pwd`  # Get the location of the mad-header directory
pkg="nginx"

install_dpk() {
    # Check we are using an apt system
    com=`command -v apt`
    if [ "$com" == "" ]; then
        echo "No apt"
        return
    else
        echo "Building debian package"
    fi
    # Enable source repos
    sed -i 's/# deb-src/deb-src/' /etc/apt/sources.list
    # Install deps
    apt-get update
    apt-get install -y gcc $pkg devscripts quilt
    apt-get build-dep -y $pkg
    cd; # Move to the home dir
    apt-get source $pkg  # Get the source package
    cd $pkg*;
    # Add the required line to the configure script
    sed -i "/common_configure_flags := /a --add-module=$GIT_PATH \\\\" ~/$pkg*/debian/rules
    # BUild the new package
    read -p "Ready to build?" a
    dpkg-buildpackage -b
}

raw_make() {
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
    cd ..
    rm -rf nginx-1.14.0/ nginx-1.14.0.tar.gz 
}

install_dpk