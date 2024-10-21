#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define SERVER_PORT 7777
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    int result;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Erro na inicialização do Winsock.\n";
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Erro ao criar o socket.\n";
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Erro ao conectar ao servidor.\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Conectado ao servidor.\n";

    while (true) {
        std::cout << "Digite um numero (ou 'exit' para sair): ";
        std::cin >> buffer;

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        send(clientSocket, buffer, strlen(buffer), 0);

        result = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (result > 0) {
            buffer[result] = '\0';
            std::cout << "Resposta do servidor: " << buffer << "\n";
        } else {
            std::cerr << "Erro ao receber dados.\n";
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    std::cout << "Cliente encerrado.\n";
    
    return 0;
}
