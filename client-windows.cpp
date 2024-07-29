#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;
#define SERVPORT 80
#define SERVER_URL "quan.suning.com"
#define MAX_DATA_SIZE 1024 * 5
const char *httpRequest = "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
const char *get_url = "/getSysTime.do";

int main(int argc, char *argv[])
{
    int err;
    char SendBuf[MAX_DATA_SIZE];
    WORD versionRequired;
    WSADATA wsaData;
    versionRequired = MAKEWORD(2, 2);
    err = WSAStartup(versionRequired, &wsaData); // 协议库的版本信息
    // 通过WSACleanup的返回值来确定socket协议是否启动
    if (!err)
    {
        printf("客户端套接字已经打开!\n");
    }
    else
    {
        printf("客户端套接字打开失败!\n");
        return -1; // 结束
    }
    // 定义客户端套接字,SOCK_STREAM表示TCP协议,AF_INET表示IPv4协议,IPPROTO_TCP表示TCP协议
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        printf("客户端套接字创建失败!\n");
        return -1; // 结束
    }

    // socket编程中，它定义了一个结构体SOCKADDR_IN来存计算机的一些信息，像socket的系统，
    // 端口号，ip地址等信息，这里存储的是服务器端的计算机的信息
    SOCKADDR_IN clientsock_in;
    // 注意这里的inet_addr函数，它将一个字符串转换为一个网络字节序的32位整数
    // 这里的SERVER_URL就是服务器的ip地址
    memset(&clientsock_in, 0, sizeof(clientsock_in));
    // 将域名转换为IP地址
    // inet_addr函数将域名转换为IP地址，如果域名不正确，会返回INADDR_NONE，
    // 这个值在判断的时候会出错，所以这里需要判断一下
    // 将域名转换为IP地址
    struct hostent *host = gethostbyname("quan.suning.com");
    printf("服务器IP地址: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr));
    if (host == NULL)
    {
        perror("gethostbyname");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    memcpy((char *)&clientsock_in.sin_addr.s_addr, host->h_addr, host->h_length);
    // 注意这里的htons函数，它将一个short整数转换为网络字节序
    clientsock_in.sin_family = AF_INET;
    clientsock_in.sin_port = htons(SERVPORT);

    // 前期定义了套接字，定义了服务器端的计算机的一些信息存储在clientsock_in中，
    // 准备工作完成后，然后开始将这个套接字链接到远程的计算机
    // 也就是第一次握手
    printf("连接中 ...\n");
    int r = connect(clientSocket, (SOCKADDR *)&clientsock_in, sizeof(SOCKADDR)); // 开始连接

    if (r == SOCKET_ERROR)
    {
        perror("connect");
        return -1;
    }
    printf("连接成功\n");
    // 发送数据

    // 发送数据之前，先发送HTTP请求
    sprintf(SendBuf, httpRequest, get_url, SERVER_URL);
    if (send(clientSocket, SendBuf, strlen(SendBuf), 0) == -1)
    {
        perror("send");
        return -1;
    }
    memset(SendBuf, 0, MAX_DATA_SIZE);
    // 接收数据
    // 接收数据之前，先接收HTTP响应
    if (recv(clientSocket, SendBuf, MAX_DATA_SIZE, 0) == -1)
    {
        perror("recv");
        return -1;
    }
    // 跳过HTTP响应的头部信息
    char *p = strstr(SendBuf, "\r\n\r\n");
    if (p)
    {
        p += 4; // 跳过HTTP头部信息
    }
    else
    {
        printf("HTTP头部信息出错!\n");
        return -1;
    }
    // 打印接收到的HTTP响应的正文
    printf("服务器端响应: %s", p);
    // 解析Json

    // 关闭套接字
    closesocket(clientSocket);
    // 关闭服务
    WSACleanup();
    return 0;

    // 关闭套接字
    closesocket(clientSocket);
    // 关闭服务
    WSACleanup();
    return 0;
}