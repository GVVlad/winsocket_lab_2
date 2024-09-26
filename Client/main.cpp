#include <iostream>
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 27015
#define BUFFER_SIZE 1024

void startClient() {
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    SetConsoleOutputCP(CP_UTF8);

    // Ініціалізація Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Не вдалося ініціалізувати Winsock. Код помилки: " << WSAGetLastError() << std::endl;
        return;
    }

    // Створення сокету
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Не вдалося створити сокет. Код помилки: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    serverAddr.sin_family = AF_INET; // Вказуємо, що використовуємо IPv4
    InetPton(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr); // Конвертуємо рядок IP в адресний формат
    serverAddr.sin_port = htons(PORT); // Встановлюємо порт сервера у мережевому порядку байтів

    // Підключення до сервера
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Не вдалося підключитися до сервера. Код помилки: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    std::cout << "Підключено до сервера." << std::endl;

    // Відкриваємо файл для запису
    std::ofstream outputFile("received.txt", std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Не вдалося відкрити файл для запису." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytesReceived;

    // Отримуємо дані з сервера
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        outputFile.write(buffer, bytesReceived);
    }

    std::cout << "Файл отримано і збережено як received.txt." << std::endl;

    // Закриття з'єднання
    closesocket(clientSocket);
    WSACleanup();

    // Читання вмісту збереженого файлу
    outputFile.close();
    std::ifstream inputFile("received.txt");
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            std::cout << line << std::endl;
        }
        inputFile.close();
    } else {
        std::cerr << "Не вдалося відкрити отриманий файл." << std::endl;
    }
}

int main() {
    startClient();
    return 0;
}
