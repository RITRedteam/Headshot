# This file will write the install.sh file to make the deployment of headshot easier. 
# Simply supply the following argument: version number of nginx, pick from the following package mangers: "yum", "apt", or "package", and the name of the install file you want to create
# written by Jason Howe

import subprocess
import sys


# removes the old nginx and stops the servce
def write_prep():
    prep = "!/bin/bash\n"
    prep += "\n"
    prep += "# remove old nginx install\n"
    prep += "rm -rf /usr/local/nginx/\n"
    prep += "\n"
    prep += "# kill all nginx processes\n"
    prep += "pkill nginx\n"
    return prep


def write_dependencies(packmang):
    dependencies = "# install build dependencies\n"
    if packmang == "yum":
        dependencies += "yum install -y "
        dependencies += "pcre pcre-devel zlib zlib-devel openssl openssl-devel\n"
    elif packmang == "apt":
        dependencies += "apt-get install -y "
        dependencies += "libpcre3 libpcre3-dev zlib1g zlib1g-dev openssl libssl-dev libxslt1-dev libgd-dev libgeoip-dev\n"
    elif packmang == "package":
        dependencies += "pkg install"
        # dependencies += "" // will be updated at a later date
    else: 
        print("not an acceptable package manager")
        return
    return dependencies


def write_wget(vnum):
    wget = "# pull down nginx\n"
    wget += "wget https://nginx.org/download/nginx-" + vnum + ".tar.gz\n"
    wget += "tar zxf nginx-" + vnum + ".tar.gz\n"
    wget += "cd nginx-" + vnum + "\n"
    return wget


def write_malware():
    pass
    # call nginx -V 
    # save the output 
    # split the output, remove all the bad parts, concat the good parts after that 


def write_compilation_cleanup(vnum):
    comp = "# compile and install\n"
    comp += "make && make install\n"
    comp += "\n"
    comp += "# clean up\n"
    comp += "cd ..\n"
    comp += "rm -rf nginx-" + vnum + "/ nginx-" + vnum + ".tar.gz\n"
    return comp


# the main function
def main():
    prep = write_prep()
    print(prep)
    dependencies = write_dependencies("yum")
    print(dependencies)
    wget = write_wget("1.1.1")
    print(wget)
    comp = write_compilation_cleanup("1.1.1")
    print(comp)



# parse system args, error hanlde, then call main 
if __name__ == "__main__":
    main()
