#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <chrono>
#include <thread>
#include <string>

#define prot IPPROTO_TCP; 

//IPPROTO_UDP

int shutdown_services(ADDRINFO* addrResult,SOCKET* ConnectSocket,std::string message,int result) {
    std::cout << message << " " << result << std::endl;
    if (ConnectSocket != NULL) {
        closesocket(*ConnectSocket);
        *ConnectSocket = INVALID_SOCKET;
    }
    freeaddrinfo(addrResult);
    WSACleanup();
    return 1;
}


int main()
{
    WSADATA wsaData;
    int result;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    ADDRINFO hints;
    ADDRINFO* addrResult = NULL;

    if (result != 0) return shutdown_services(addrResult, NULL, "WSAStartup failed, result = ", result);

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = prot;

    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) return shutdown_services(addrResult,NULL, "getaddrinfo failed, result = ",result);

    SOCKET ConnectSocket = INVALID_SOCKET;

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) return shutdown_services(addrResult, &ConnectSocket, "Socket creation failed, result = ",result);
    
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) return shutdown_services(addrResult, &ConnectSocket, "Unable to connect to server, result = ", result);
    
    int cnt_long, cnt_float;
    std::cout << "How many long numbers do I want? " << std::endl;
    std::cin >> cnt_long;
    std::cout << "How many float numbers do I want? " << std::endl;
    std::cin >> cnt_float;


    std::string sendBuffer = "Hello, from client. I want " + std::to_string(cnt_long) + " long numbers and " + std::to_string(cnt_float) + " float numbers";
    result = send(ConnectSocket, sendBuffer.c_str(), (int)strlen(sendBuffer.c_str()), 0);
    if (result == SOCKET_ERROR) return shutdown_services(addrResult, &ConnectSocket, "Send failed, result = ", result);
  
    std::cout << "Bytes sent: " << result << " bytes" << std::endl;

    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) return shutdown_services(addrResult, &ConnectSocket, "Shutdown failed, result = ", result);

    char recvBuffer[512];


    do {
        ZeroMemory(recvBuffer, 512);

        result = recv(ConnectSocket, recvBuffer, 512, 0);

        if (result > 0) {
            std::cout << "Receieved data: " << recvBuffer << std::endl;
        }
        else if (result == 0) std::cout << "Connection closed" << std::endl;
        else std::cout << "Recv failed with error: " << result << std::endl;
    } while (result > 0);

    shutdown_services(addrResult, &ConnectSocket, "Returned ", result);
    return 0;
}

