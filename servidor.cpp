#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

#define SERVER_PORT 7777
#define BUFFER_SIZE 1024

void handleClient(SOCKET clientSocket, sockaddr_in clientAddr) {
    char buffer[BUFFER_SIZE];
    int receivedBytes;
    char* clientIP = inet_ntoa(clientAddr.sin_addr);

    std::cout << "Conectado ao cliente: " << clientIP << "\n";

    while (true) {
        receivedBytes = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (receivedBytes > 0) {
            buffer[receivedBytes] = '\0';

            int num = atoi(buffer);
            if (num == 0 && strcmp(buffer, "0") != 0) {
                std::cerr << "Número invalido recebido.\n";
                send(clientSocket, "Erro: Valor invalido.", 22, 0);
            } else {
                std::string resposta = (num % 2 == 0) ? "Par" : "Impar";
                std::cout << "Recebido do cliente " << clientIP << ": " << num << " (" << resposta << ")\n";
                send(clientSocket, resposta.c_str(), resposta.length(), 0);
            }
        } else {
            std::cerr << "Conexao encerrada pelo cliente.\n";
            break;
        }
    }

    closesocket(clientSocket);
    std::cout << "Cliente desconectado: " << clientIP << "\n";
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Erro na inicialização do Winsock.\n";
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Erro ao criar o socket.\n";
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Erro no bind.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Erro ao ouvir o socket.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Aguardando conexões...\n";

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Erro ao aceitar conexão.\n";
            continue;
        }

        std::thread clientThread(handleClient, clientSocket, clientAddr);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
