/*
Server Socket:
socket -> bind -> listen -> accept -> switch threads -> recv -> send
Client Socket:
socket -> connect -> send -> recv
*/

#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 收发包的端口
#define PORT 8080
// 存放报文的字符数组长度
#define BUFFER_SIZE 2048

using namespace std;

// 处理单个客户端连接的函数
void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE] = {0}; // 存放报文的字符数组
    ssize_t bytes; // 存放 recv 的返回值, -1 表示 recv 出错, 所以不能使用 size_t

    cout << "[Server] Handling new client..." << endl;

    // bytes == -1 表示 recv 出错, bytes == 0 表示连接已关闭
    while ((bytes = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes] = '\0';

        // 打印接受的消息
        cout << "[Server] Received: " << buffer << endl;

        // 发送添加 "Echo: " 前缀的消息
        string response = "Echo: " + string(buffer);
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    // bytes == 0 表示连接已关闭
    if (bytes == 0) {
        cout << "[Server] Client disconnected." << endl;
    } else {
        perror("[Server] recv failed");
    }

    close(clientSocket);
}

// 服务端线程函数
void serverThread() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 创建 socket 用于监听 IP 报文,
    // IPPROTO_TCP 可用 0 替换, 系统会通过其他参数自动推断出使用 TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("socket failed");
        return;
    }

    // 设置地址重用, 1 表示开启, 0 表示关闭
    char opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET; // 设置 IP 类型
    address.sin_addr.s_addr = INADDR_ANY; // 设置 IP 地址
    address.sin_port = htons(PORT); // 设置端口号, 注意必须转为网络序

    // 绑定
    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0) {
        perror("bind failed");
        close(server_fd);
        return;
    }

    // 监听, 返回 -1 表示失败
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        return;
    }

    cout << "[Server] Listening on port " << PORT << endl;

    while (true) {
        // accept 接口默认是阻塞模式, 若有连接建立, 则返回新 socket_fd, 后续可使用该 fd 进行收发包
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept failed");
            break;
        }
        // 为每个客户端创建新线程, 并将新线程与当前线程分离, 本线程无需等待新线程
        thread(handleClient, new_socket).detach();
    }

    close(server_fd);
}

// 客户端线程函数
void clientThread() {
    this_thread::sleep_for(100ms); // 等待服务端启动

    int socket_fd = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // IPPROTO_TCP 可用 0 替换, 系统会通过其他参数自动推断出使用 TCP
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "[Client] Socket creation error" << endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cerr << "[Client] Invalid address\n";
        close(socket_fd);
        return;
    }

    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "[Client] Connection failed\n";
        close(socket_fd);
        return;
    }

    string message = "Hello from client!";
    send(socket_fd, message.c_str(), message.size(), 0);
    cout << "[Client] Sent: " << message << endl;

    ssize_t bytes = recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        cout << "[Client] Received: " << buffer << endl;
    }

    close(socket_fd);
}

int socket_model()
{
    // 启动服务端线程
    thread server(serverThread);
    
    // 启动客户端线程
    thread client(clientThread);

    // 等待客户端完成（服务端持续运行）
    client.join();
    server.detach(); // 或者用信号控制退出

    cout << "Client finished. Server still running...\n";
    sleep(2); // 简单等待后退出（实际应用中应优雅关闭）

    return 0;
}