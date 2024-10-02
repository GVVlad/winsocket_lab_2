#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

#define PORT 27015
#define BUFFER_SIZE 1024

void startServer() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SetConsoleOutputCP(CP_UTF8);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Не вдалося ініціалізувати Winsock. Код помилки: " << WSAGetLastError() << std::endl;
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Не вдалося створити сокет. Код помилки: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Не вдалося виконати прив'язку. Код помилки: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Сервер запущений на порту " << PORT << std::endl;

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Не вдалося почати прослуховування. Код помилки: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Очікування підключення клієнта..." << std::endl;

    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Не вдалося прийняти підключення. Код помилки: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Клієнт підключено." << std::endl;

    // std::string fileName = "images.jpg";
    std::string fileName = "file.txt";


    if (send(clientSocket, fileName.c_str(), fileName.size(), 0) == SOCKET_ERROR) {
        std::cerr << "Не вдалося відправити назву файлу. Код помилки: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Не вдалося відкрити файл." << std::endl;
        closesocket(clientSocket);
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    char buffer[BUFFER_SIZE];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        int bytesSent = send(clientSocket, buffer, file.gcount(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Помилка під час відправки даних. Код помилки: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    std::cout << "Файл відправлено." << std::endl;

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    startServer();
    return 0;
}
