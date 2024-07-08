# Simple Client-Server Arithmetic Application

This repository contains a simple client-server arithmetic application implemented in C. The server performs basic arithmetic operations (addition, subtraction, multiplication, division, and modulus) based on the client's request.

## Files

- `server.c`: Contains the server code.
- `client.c`: Contains the client code.

## Build Instructions

To compile the server and client programs, use the following commands:

\```sh
gcc -o server server.c
gcc -o client client.c
\```

## Usage

### Server

To start the server, run the following command:

\```sh
./server <ip address> <port>
\```

- `<ip address>`: The IP address on which the server will listen.
- `<port>`: The port on which the server will listen.

### Client

To start the client, run the following command:

\```sh
./client <ip address> <port>
\```

- `<ip address>`: The IP address of the server.
- `<port>`: The port on which the server is listening.

## Operations

The client can send the following arithmetic operations to the server:

- `ADD <number1> <number2>`: Adds `number1` and `number2`.
- `SUB <number1> <number2>`: Subtracts `number2` from `number1`.
- `MUL <number1> <number2>`: Multiplies `number1` and `number2`.
- `DIV <number1> <number2>`: Divides `number1` by `number2`.
- `MOD <number1> <number2>`: Computes the modulus of `number1` and `number2`.
- `EXIT`: Closes the client connection.

### Example

1. Start the server:

    \```sh
    ./server 127.0.0.1 8080
    \```

2. Start the client:

    \```sh
    ./client 127.0.0.1 8080
    \```

3. Perform operations in the client:

    \```sh
    ADD 10 5
    SUB 10 5
    MUL 10 5
    DIV 10 5
    MOD 10 5
    EXIT
    \```

## PDU (Protocol Data Unit)

In this application, PDUs (Protocol Data Units) are used to structure the data being sent between the client and server.

- **RQT_PDU (Request PDU)**: A 20-byte array that contains:
  - 4 bytes for the operation code (`int32_t`)
  - 8 bytes for the first operand (`int64_t`)
  - 8 bytes for the second operand (`int64_t`)

- **REP_PDU (Reply PDU)**: An 8-byte array that contains:
  - 8 bytes for the result (`int64_t`)

### Network Byte Order

In network programming, data is often transmitted between systems with different architectures (e.g., little-endian vs big-endian). To ensure data is interpreted correctly regardless of the architecture, the data is converted to a common byte order known as network byte order (big-endian).

Functions such as `htonl()`, `htonll()`, `ntohl()`, and `ntohll()` are used to convert values between host and network byte order:

- `htonl()`: Converts `int32_t` from host to network byte order.
- `htonll()`: Converts `int64_t` from host to network byte order.
- `ntohl()`: Converts `int32_t` from network to host byte order.
- `ntohll()`: Converts `int64_t` from network to host byte order.

## Signal Handling

The server handles the following signals:

- `SIGPIPE`: Ignored to handle broken pipe errors.
- `SIGINT`: Catches the signal to allow graceful shutdown of the server.

## Error Handling

Both server and client have error handling for socket operations and system calls, with appropriate error messages printed to `stderr`.

