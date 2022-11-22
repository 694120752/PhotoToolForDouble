#include <iostream>
#include <netinet/in.h> //sockaddr_in结构体头文件
#include <string.h> //memset()头文件
#include <assert.h> //assert()头文件
#include <sys/types.h> //open()头文件
#include <sys/stat.h>
#include <fcntl.h>
// #include <sys/sendfile.h> //sendfile()头文件
#include <unistd.h> //close()头文件
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/stat.h>

#include <filesystem>
#include <sys/time.h>
using namespace  std;
#define localFilePath "resource/index.html"

enum RouteType {
    IndexType,
    UploadType,
    ImageType
};
RouteType routeFunc(char *source);
void sendIndexHtml(int conn_fd);
void sendImage(int conn_fd,char *requestChar);
void handleImage(int conn_fd,char *requestChar);
char * currentTime();

int main() {
    int socket_fd; // 服务端socket标识符
    int conn_fd; // 已连接的客户端socket标识符
    int res;
    int len;

    struct sockaddr_in sever_add;
    memset(&sever_add,0,sizeof(sever_add)); //初始化
    // cout << "sever_add结构体的大小 字节？ bit ？"<< sizeof(socket_fd) << " sdsd " << sizeof(sever_add) << endl;

    sever_add.sin_family = PF_INET;
    sever_add.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY 转换过来就是 0.0.0.0 指本机ip （防止多网卡）
    sever_add.sin_port = htons(8081);
    len = sizeof(sever_add);

    // socket()
    // 三个参数 第一个地址族 AF_INETipv4 或者 AF_INET6ipv6
    // 第二个为 socket的类型
    // 第三个为协议
    socket_fd =  socket(AF_INET,SOCK_STREAM,0);
    assert(socket_fd >= 0);

    //bind()
    // bind()函数把一个地址族中的特定地址赋给socket。例如对应AF_INET、AF_INET6就是把一个ipv4或ipv6地址和端口号组合赋给socket。
    // socket 方法只是创建了一个对象 返回了socket的标识 bind会绑定ip协议这些
    // 参考 https://zhuanlan.zhihu.com/p/365478112
    res = bind(socket_fd,(struct sockaddr*)&sever_add,len);
    assert(res != -1);

    //listen()
    res = listen(socket_fd,1);
    while (1)
    {
        cout << currentTime() << "inwhile" << endl;
        struct sockaddr_in client;
        int client_len = sizeof(client);
        // 返回的socket标识符
        conn_fd = accept(socket_fd,(struct sockaddr*)&client,(socklen_t *)&client_len);
        if(conn_fd < 0) cout << "error" << endl;
        else{
            cout << currentTime() <<"start accept" << endl;
            
            // 朋友圈最多为9图 按每张1.5M计算 加上1M信息描述为14.5M  放大一点算20M 应该够用了安卓的像素没那么高 而且微信本身会压缩
            // 后期可以在前端做一个校验 发送的socket总量大于20M的时候 对最大的照片做一次压缩
            char request[1024 * 20]; // 单位是byte

            // 接收来自socket缓冲区的数据，当缓冲区没有数据可取时，recv会一直处于阻塞状态()，直到缓冲区至少又一个字节数据可读取，或者对端关闭，并读取所有数据后返回.
            // 当协议把数据接收完毕，recv函数就把sockfd的接收缓冲区中的数据copy到buff中(注意：协议接收到的数据可能大于buff的长度，所以在这种情况下要调用几次recv函数才能把sockfd的接收缓冲区中的数据copy完。recv函数仅仅是copy数据，真正的接收数据是协议来完成的)。
            ssize_t recvResult = recv(conn_fd,request,1024,0);
            request[strlen(request) + 1] = '\0';

            char buf[200] = "HTTP/1.1 200 ok\r\nconnection: close\r\n\r\n";//HTTP响应
            int s = send(conn_fd,buf,strlen(buf),0);//发送响应
            // 区分路由
            RouteType type = routeFunc(request);
            if (type == IndexType)
            {
                // 基础索引 返回HTML
                sendIndexHtml(conn_fd);
            } else if (type == UploadType) {
                // 应该是带着图片一起来了 返回对应的值
                // 上传图片过来了
                handleImage(conn_fd,request);
            } else if (type == ImageType) {
                sendImage(conn_fd,request);
            }
            
            close(conn_fd); // 关闭本次客户端socket链接
            cout << currentTime() <<"socket closed" << endl;
        }
    }
    return 0;
}

// 增加路由处理逻辑
RouteType routeFunc(char *source) {
    RouteType type = IndexType;
    char indexStr[] = "upload"; // 上传标识
    char imageStr0[] = ".jpg";
    char imageStr1[] = ".jpeg";
    char imageStr2[] = ".JPG";

    if (strstr(source,indexStr)) {
        type = UploadType;
    } else if (strstr(source,imageStr0) || strstr(source,imageStr1) || strstr(source,imageStr2)) {
        type = ImageType;
    }
    
    return type;
}

void sendIndexHtml(int conn_fd) {
    // int fd = open("WWW/index.html",O_RDONLY); // O_RDONLY 以只读的方式打开
    // sendfile(conn_fd,fd,NULL,2048);//零拷贝发送消息体
    // MacOS 下 https://github.com/ttc0419/macos-sendfile-bug/blob/master/server.c
    
    // 发送文件
    struct stat filestat;
    int statResult = stat(localFilePath,&filestat);
    if (statResult != 0) {
        cout << currentTime() << "本地文件(" << localFilePath << ")读取出错:" << endl;
    }
    int fd = open(localFilePath,O_RDONLY);
    int sendFileResult = sendfile(fd,conn_fd,0,&filestat.st_size,NULL,0);
    if (sendFileResult != 0)
    {
        cout << currentTime() << "文件传送失败："<< sendFileResult << endl;
    }
    close(fd); // 关闭文件
}

void sendImage(int conn_fd,char *requestChar) {
    cout << currentTime() << "收到了请求图片的路由" << endl;
}

void handleImage(int conn_fd,char *requestChar) {
    cout << currentTime() <<"我是upload的路由" << endl;

     char buf[200] = "little dai bi";//HTTP响应
    int s = send(conn_fd,buf,strlen(buf),0);//发送响应
    cout << requestChar << endl;
    cout << "没了" << endl;
}

char *currentTime() {
    struct timeval time;
    gettimeofday(&time,NULL);
    static const int MAX_BUFFER_SIZE = 128;
    static char timestamp_str[MAX_BUFFER_SIZE];
    time_t sec = static_cast<time_t>(time.tv_sec);
    int ms = static_cast<int>(time.tv_usec) / 1000;

    struct tm tm_time;
    localtime_r(&sec, &tm_time);
    static const char *formater = "%4d-%02d-%02d %02d:%02d:%02d.%03d";
    int wsize = snprintf(timestamp_str, MAX_BUFFER_SIZE, formater,
                        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, ms);
    timestamp_str[min(wsize, MAX_BUFFER_SIZE - 1)] = '\0';
    return timestamp_str;
}
