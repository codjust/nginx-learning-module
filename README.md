## Install
```shell
./configure --add-module=nginx-hello-module
make 
make install
```

## Usage
```shell
location /hello {
    hello on;
    return 200;
}
```
test request:
```shell
curl -i 127.0.0.1:8989/hello
output:
hello nginx module, uri: /hello HTTP/1.1
Host
```