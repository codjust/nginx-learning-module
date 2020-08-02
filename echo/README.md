## Introduce
开发一个echo，content阶段的模块，回显

## Install
```shell
./configure --add-module=echo
make 
make install
```

## Usage
```shell
location /hello {
    echo "hello echo module";
}
```
test request:
```shell
curl -i 127.0.0.1:8989/echo
output:
hello echo module
```