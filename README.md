### 计网大作业

##### 进度

- [x] 处理 GET/POST 请求
- [x] 上传和下载文件
- [x] 持久连接，分块传输
- [ ] 管道
- [ ] 使用 openssl，支持 https
- [x] libevent

##### 编译方法

gcc config.c httpd.c utils.c -levent -o httpd

##### 运行方法

在 httpd 可运行文件所在的目录下新建文件夹 files，随便放一些目录文件
然后./httpd
