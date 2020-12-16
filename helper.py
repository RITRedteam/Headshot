# This file will write the install.sh file to make the deployment of headshot easier. 
# Simply supply the following argument: version number of nginx, pick from the following package mangers: "yum", "apt", or "package", and the name of the install file you want to create
# written by Jason Howe/P0nt14c

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
    output = subprocess.check_output("/usr/sbin/nginx -V")
    output = str(output, sys.getdefaultencoding())
    lines = 0
    opt_count = 0
    options = []
    result = "# add our module as a dependency \n./configure --add-module=../ --user=root "
    for line in output:
        # this is the line that has the args we need
        if lines == 3:
            options = line.split()
            # get rid of the "configure arguments:" thing 
            for element in options:
                if opt_count >= 2:
                    # dynamic configs will break headshot, so we exluced them
                    if "dynamic" not in element:
                        result += opt_count
                        result += " "
        
        lines = lines + 1
    result += "\n"
    return result
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


def verify_ver(version):
    # add our module as a dependency
    a = version.split(".")
    if len(a) != 3:
        return False
    for x in a:
        if not x.isdigit():
            return False
    return True


def verify_packman(package):
    accepatble = ["yum", "apt", "package"]
    if package not in accepatble:
        return False
    return True

# the main function
def test():
    prep = write_prep()
    print(prep)
    dependencies = write_dependencies("yum")
    print(dependencies)
    wget = write_wget("1.1.1")
    print(wget)
    malware = write_malware()
    print(malware)
    comp = write_compilation_cleanup("1.1.1")
    print(comp)


def main(version, packmang, file):
    prep = write_prep()
    dependencies = write_dependencies(packmang)
    wget = write_wget(version)
    malware = write_malware()
    comp = write_compilation_cleanup(version)
    
    try: 
        file = open(file, "w")
    except FileNotFoundError:
        print("File not Found")
        return False

    file.write(prep)
    file.write(dependencies)
    file.write(wget)
    file.write(malware)
    file.write(comp)
    file.close()
    return True


# parse system args, error hanlde, then call main 
if __name__ == "__main__":
    arg_len = len(sys.argv)
    file = "install.sh"
    if arg_len == 2:
        if sys.argv[2] == "test":
            print("Beginning Test: \n")
            test()
    if arg_len >= 3:
        if verify_ver(sys.argv[2]):
            if verify_packman(sys.argv[3]):
                if arg_len == 4:
                    file = sys.argv[4]
                if main(sys.argv[2], sys.argv[3], file):
                    print("Program completed. Check %s for your script", file)
                else:
                    print("Program failed. Please fix errors and try again")
            else: 
                print("Not an acceptable package manager. Please try again")
        else:
            print("Not a properly formatted version number")
    else:
        print("Not an accepatable number of arguments.")
        print("Usage: python3 helper.py test OR python3 helper.py <x.y.z> <packagemanager> <OPT:filename>")
    
