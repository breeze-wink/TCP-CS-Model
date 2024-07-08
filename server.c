#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

typedef char RQT_PDU[20];
typedef char REP_PDU[8] ;

int sigint_flag = 0;

void sigint_handler(int signum)
{
    printf("[srv] SIGINT is coming!\n");
    sigint_flag = 1;
}

void set_signal_handler()
{
    struct sigaction SIGPIPE_act, SIGINT_act;
    memset(&SIGPIPE_act, 0, sizeof(SIGPIPE_act));
    memset(&SIGINT_act, 0, sizeof(SIGINT_act));

    sigemptyset(&SIGPIPE_act.sa_mask);
    sigemptyset(&SIGINT_act.sa_mask);

    SIGPIPE_act.sa_handler = SIG_IGN;
    SIGINT_act.sa_handler = sigint_handler;

    if (sigaction(SIGPIPE, &SIGPIPE_act, NULL) == -1)
    {
        perror("Failed to sigaction for SIGPIPE");
        exit(1);
    }

    if (sigaction(SIGINT, &SIGINT_act, NULL) == -1)
    {
        perror("Failed to sigaction for SIGINT");
        exit(1);
    }
}

int64_t htonll(int64_t value) 
{
    // 检查主机是否为大端
    static const int num = 1;
    if (*(const char*)&num == 1) // 小端
    { 
        return ((int64_t)htonl(value & 0xFFFFFFFF) << 32) | htonl((uint32_t)(value >> 32));
    } 
    else // 大端
    { 
        return value;
    }
}

int64_t ntohll(int64_t value) 
{
    return htonll(value);
}

void handle_PDU(RQT_PDU rqt, REP_PDU res)
{
    int32_t OP  = *((int32_t*)rqt);
    int64_t OP1 = *((int64_t*)(rqt + 4));
    int64_t OP2 = *((int64_t*)(rqt + 12));

    OP = ntohl(OP);
    OP1 = ntohll(OP1);
    OP2 = ntohll(OP2);

    int64_t ans = 0;

    switch (OP)
    {
        case 0x0001:
            ans = OP1 + OP2;
            printf("[rqt_res] %ld + %ld = %ld\n", OP1, OP2, ans);
            break;
        case 0x0002:
            ans = OP1 - OP2;
            printf("[rqt_res] %ld - %ld = %ld\n", OP1, OP2, ans);
            break;
        case 0x0004:
            ans = OP1 * OP2;
            printf("[rqt_res] %ld * %ld = %ld\n", OP1, OP2, ans);
            break;
        case 0x0008:
            ans = OP1 / OP2;
            printf("[rqt_res] %ld / %ld = %ld\n", OP1, OP2, ans);
            break;
        case 0x0010:
            ans = OP1 % OP2;
            printf("[rqt_res] %ld %% %ld = %ld\n", OP1, OP2, ans);
            break;
        default:
            break;
    }

    ans = htonll(ans);
    memcpy(res, &ans, sizeof(ans));

}

void srv_task(int connfd, struct sockaddr_in client_addr)
{
    char* client_ip_addr = inet_ntoa(client_addr.sin_addr);
    uint16_t client_port = ntohs(client_addr.sin_port);

    printf("[srv] client[%s:%hu] is accepted!\n", client_ip_addr, client_port);

    RQT_PDU rqt = {0};
    REP_PDU ans = {0};


    while (1)
    {
        int len = read(connfd, rqt, sizeof(rqt));
        if (len == -1)
        {
            printf("Failed to read from client, ip = %s, port = %hu\n", client_ip_addr, client_port);
            exit(1);
        }
        else if (len == 0)
        {
            break;
        }

        handle_PDU(rqt, ans);

        if (write(connfd, ans, sizeof(ans)) == -1)
        {
            printf("Failed to write to client, ip = %s, port = %hu\n", client_ip_addr, client_port);
            exit(1);
        }
    }
    

    close(connfd);
    printf("[srv] client[%s:%hu] is closed!\n", client_ip_addr, client_port);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, 
        "<usage>:<exec file> <ip address> <port>\n");
        return -1;
    }

    set_signal_handler();
    
    char* listen_ip = argv[1];
    uint16_t port = atoi(argv[2]);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        perror("Failed to create socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, listen_ip, &serv_addr.sin_addr.s_addr);

    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("Failed to bind socket");
        return -1;
    }

    if (listen(listenfd, 10) == -1)
    {
        perror("Failed to listen");
        return -1;
    }

    printf("[srv] server[%s:%hu] is initializing!\n", listen_ip, port);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while (sigint_flag == 0)
    {
        int connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (connfd < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("Failed to accept");
                return -1;
            }
        }
        srv_task(connfd, client_addr);
    }

    close(listenfd);

    printf("[srv] listenfd is closed!\n");
    printf("[srv] server is going to exit!\n");
    return 0;    
}