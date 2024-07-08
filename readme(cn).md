# 简单的客户端-服务器算术应用程序

该仓库包含一个用C语言实现的简单客户端-服务器算术应用程序。服务器根据客户端的请求执行基本的算术运算（加法、减法、乘法、除法和取模运算）。

## 文件

- `server.c`：包含服务器代码。
- `client.c`：包含客户端代码。

## 构建指南

要编译服务器和客户端程序，请使用以下命令：

\```sh
gcc -o server server.c
gcc -o client client.c
\```

## 使用方法

### 服务器

启动服务器，运行以下命令：

\```sh
./server <ip地址> <端口>
\```

- `<ip地址>`：服务器监听的IP地址。
- `<端口>`：服务器监听的端口。

### 客户端

启动客户端，运行以下命令：

\```sh
./client <ip地址> <端口>
\```

- `<ip地址>`：服务器的IP地址。
- `<端口>`：服务器监听的端口。

## 操作

客户端可以向服务器发送以下算术操作：

- `ADD <number1> <number2>`：将 `number1` 和 `number2` 相加。
- `SUB <number1> <number2>`：将 `number2` 从 `number1` 中减去。
- `MUL <number1> <number2>`：将 `number1` 和 `number2` 相乘。
- `DIV <number1> <number2>`：将 `number1` 除以 `number2`。
- `MOD <number1> <number2>`：计算 `number1` 和 `number2` 的模。
- `EXIT`：关闭客户端连接。

### 示例

1. 启动服务器：

    \```sh
    ./server 127.0.0.1 8080
    \```

2. 启动客户端：

    \```sh
    ./client 127.0.0.1 8080
    \```

3. 在客户端执行操作：

    \```sh
    ADD 10 5
    SUB 10 5
    MUL 10 5
    DIV 10 5
    MOD 10 5
    EXIT
    \```

## PDU（协议数据单元）

在这个应用程序中，PDU（协议数据单元）用于结构化在客户端和服务器之间传输的数据。

- **RQT_PDU（请求PDU）**：一个20字节的数组，包含：
  - 4字节用于操作码（`int32_t`）
  - 8字节用于第一个操作数（`int64_t`）
  - 8字节用于第二个操作数（`int64_t`）

- **REP_PDU（应答PDU）**：一个8字节的数组，包含：
  - 8字节用于结果（`int64_t`）

### 网络字节序

在网络编程中，数据通常在具有不同架构（如小端序与大端序）的系统之间传输。
为了确保数据在任何架构下都能正确解释，数据会转换为一种通用的字节序，称为网络字节序（大端序）。

函数如 `htonl()`、`htonll()`、`ntohl()` 和 `ntohll()` 用于在主机和网络字节序之间转换值：

- `htonl()`：将 `int32_t` 从主机字节序转换为网络字节序。
- `htonll()`：将 `int64_t` 从主机字节序转换为网络字节序。
- `ntohl()`：将 `int32_t` 从网络字节序转换为主机字节序。
- `ntohll()`：将 `int64_t` 从网络字节序转换为主机字节序。

## 信号处理

服务器处理以下信号：

- `SIGPIPE`：忽略以处理断管错误。
- `SIGINT`：捕获信号以允许服务器优雅地关闭。

## 错误处理

服务器和客户端都对套接字操作和系统调用进行了错误处理，并将适当的错误消息打印到 `stderr`。

