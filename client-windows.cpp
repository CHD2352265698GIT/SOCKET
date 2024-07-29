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
    err = WSAStartup(versionRequired, &wsaData); // Э���İ汾��Ϣ
    // ͨ��WSACleanup�ķ���ֵ��ȷ��socketЭ���Ƿ�����
    if (!err)
    {
        printf("�ͻ����׽����Ѿ���!\n");
    }
    else
    {
        printf("�ͻ����׽��ִ�ʧ��!\n");
        return -1; // ����
    }
    // ����ͻ����׽���,SOCK_STREAM��ʾTCPЭ��,AF_INET��ʾIPv4Э��,IPPROTO_TCP��ʾTCPЭ��
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        printf("�ͻ����׽��ִ���ʧ��!\n");
        return -1; // ����
    }

    // socket����У���������һ���ṹ��SOCKADDR_IN����������һЩ��Ϣ����socket��ϵͳ��
    // �˿ںţ�ip��ַ����Ϣ������洢���Ƿ������˵ļ��������Ϣ
    SOCKADDR_IN clientsock_in;
    // ע�������inet_addr����������һ���ַ���ת��Ϊһ�������ֽ����32λ����
    // �����SERVER_URL���Ƿ�������ip��ַ
    memset(&clientsock_in, 0, sizeof(clientsock_in));
    // ������ת��ΪIP��ַ
    // inet_addr����������ת��ΪIP��ַ�������������ȷ���᷵��INADDR_NONE��
    // ���ֵ���жϵ�ʱ����������������Ҫ�ж�һ��
    // ������ת��ΪIP��ַ
    struct hostent *host = gethostbyname("quan.suning.com");
    printf("������IP��ַ: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr));
    if (host == NULL)
    {
        perror("gethostbyname");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    memcpy((char *)&clientsock_in.sin_addr.s_addr, host->h_addr, host->h_length);
    // ע�������htons����������һ��short����ת��Ϊ�����ֽ���
    clientsock_in.sin_family = AF_INET;
    clientsock_in.sin_port = htons(SERVPORT);

    // ǰ�ڶ������׽��֣������˷������˵ļ������һЩ��Ϣ�洢��clientsock_in�У�
    // ׼��������ɺ�Ȼ��ʼ������׽������ӵ�Զ�̵ļ����
    // Ҳ���ǵ�һ������
    printf("������ ...\n");
    int r = connect(clientSocket, (SOCKADDR *)&clientsock_in, sizeof(SOCKADDR)); // ��ʼ����

    if (r == SOCKET_ERROR)
    {
        perror("connect");
        return -1;
    }
    printf("���ӳɹ�\n");
    // ��������

    // ��������֮ǰ���ȷ���HTTP����
    sprintf(SendBuf, httpRequest, get_url, SERVER_URL);
    if (send(clientSocket, SendBuf, strlen(SendBuf), 0) == -1)
    {
        perror("send");
        return -1;
    }
    memset(SendBuf, 0, MAX_DATA_SIZE);
    // ��������
    // ��������֮ǰ���Ƚ���HTTP��Ӧ
    if (recv(clientSocket, SendBuf, MAX_DATA_SIZE, 0) == -1)
    {
        perror("recv");
        return -1;
    }
    // ����HTTP��Ӧ��ͷ����Ϣ
    char *p = strstr(SendBuf, "\r\n\r\n");
    if (p)
    {
        p += 4; // ����HTTPͷ����Ϣ
    }
    else
    {
        printf("HTTPͷ����Ϣ����!\n");
        return -1;
    }
    // ��ӡ���յ���HTTP��Ӧ������
    printf("����������Ӧ: %s", p);
    // ����Json

    // �ر��׽���
    closesocket(clientSocket);
    // �رշ���
    WSACleanup();
    return 0;

    // �ر��׽���
    closesocket(clientSocket);
    // �رշ���
    WSACleanup();
    return 0;
}