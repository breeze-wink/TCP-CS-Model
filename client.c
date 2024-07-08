#include <stdlib.h>
#include <netinet/in.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef char RQT_PDU[20];
typedef char REP_PDU[8] ;


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

void cli_task(int sockfd)
{
    int32_t OP;
    int64_t OP1, OP2;
    char OP_str[8] = {0};
    char OP_;

    while (1)
    {
        scanf("%s", OP_str);

        if (strcmp(OP_str, "EXIT") == 0)
        {
            printf("[cli] command EXIT received\n");
            break;
        }

        scanf("%ld %ld", &OP1, &OP2);

        if (strcmp(OP_str, "ADD") == 0)
        {
            OP = 0x0001;
            OP_ = '+';
        }
        else if (strcmp(OP_str, "SUB") == 0)
        {
            OP = 0x0002;
            OP_ = '-';
        }
        else if (strcmp(OP_str, "MUL") == 0)
        {
            OP = 0x0004;
            OP_ = '*';
        }
        else if (strcmp(OP_str, "DIV") == 0)
        {
            OP = 0x0008;
            OP_ = '/';
        }
        else if (strcmp(OP_str, "MOD") == 0)
        {
            OP = 0x0010;
            OP_ = '%';
        }
        else
        {
            continue;
        }
        RQT_PDU rqt = {0};
        
        printf("[rep_rcv] %ld %c %ld = ", OP1, OP_, OP2);

        OP = htonl(OP);
        OP1 = htonll(OP1);
        OP2 = htonll(OP2);

        memcpy(rqt, &OP, sizeof(OP));
        memcpy(rqt + 4, &OP1, sizeof(OP1));
        memcpy(rqt + 12, &OP2, sizeof(OP2));

        if (write(sockfd, rqt, sizeof(rqt)) == -1)
        {
            perror("Failed to write to srv");
            exit(1);
        }

        REP_PDU ans = {0};
        if (read(sockfd, ans, sizeof(ans)) == -1)
        {
            perror("Failed to read from srv");
            exit(1);
        }

        int64_t res = *((int64_t*)ans);
        res = ntohll(res);
        printf("%ld\n",res);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, 
        "<usage>:<exec file> <ip address> <port>\n");
        return -1;
    }

    char* connect_ip_addr = argv[1];
    uint16_t connect_port = atoi(argv[2]);

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
    {
        perror("Failed to create socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(connect_port);
    inet_pton(AF_INET, connect_ip_addr, &serv_addr.sin_addr.s_addr);

    if (connect(clientfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("Failed to connect to srv");
        return -1;
    }

    printf("[cli] server[%s:%hu] is connected!\n", connect_ip_addr, connect_port);

    cli_task(clientfd);

    close(clientfd);
    printf("[cli] connfd is closed!\n");
    printf("[cli] client is going to exit!\n");
    return 0;
}
