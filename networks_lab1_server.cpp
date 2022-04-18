#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <sstream>


#define prot IPPROTO_TCP;

//IPPROTO_UDP

int shutdown_services(ADDRINFO* addrResult, SOCKET* ConnectSocket, std::string message, int result) {
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
    ADDRINFO hints;
    ADDRINFO* addrResult = NULL;

    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) return shutdown_services(addrResult, NULL, "WSAStartup failed, result = ", result);

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = prot;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, "666", &hints, &addrResult);

    if (result != 0) return shutdown_services(addrResult, NULL, "getaddrinfo failed, result = ", result);

    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) return shutdown_services(addrResult, &ListenSocket, "Socket creation failed, result = ", result);
    

    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) return shutdown_services(addrResult, &ListenSocket, "Binding socket failed, result = ", result);

    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) return shutdown_services(addrResult, &ListenSocket, "Listening socket failed, result = ", result);

    std::cout << "Server is waiting for connection" << std::endl;

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) return shutdown_services(addrResult, &ListenSocket, "Accepting socket failed, result = ", result);

    std::cout << "Client connected" << std::endl;
    closesocket(ListenSocket);

    char recvBuffer[512];
    const char* sendBuffer = "Hello, from server!";

    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ClientSocket, recvBuffer, 512, 0);
        if (result > 0) {
            std::cout << "Receieved data: " << recvBuffer << std::endl;
            std::string arr[12];
            std::string request(recvBuffer);
            std::stringstream ssin(request);
            int i = 0;
            while (ssin.good() && i < 12) {
                ssin >> arr[i];
                ++i;
            }

            int cnt_long, cnt_float;
            cnt_long = std::stoi(arr[5]);
            cnt_float = std::stoi(arr[9]);
            std::cout << "Preparing " << cnt_long << " long values and " << cnt_float << " float values" << std::endl;
            std::string response = "Your long values: ";
            for (i = 0; i < cnt_long; i++) {
                response += std::to_string((long)(rand() % 20000000)) + " ";
            }
            response += "Your float values: ";
            for (i = 0; i < cnt_float; i++) {
                response += std::to_string(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) + " ";
            }

            result = send(ClientSocket, response.c_str(), (int)strlen(response.c_str()), 0);
            std::cout << "Send data back: " << response << std::endl;
            if (result == SOCKET_ERROR) return shutdown_services(addrResult, &ClientSocket, "Sending data back failed, result = ", result);

        }
        else if (result == 0) std::cout << "Connection closed" << std::endl;
        else return shutdown_services(addrResult, &ClientSocket, "Receiving data failed, result = ", result);
    } while (result>0);


    result = shutdown(ClientSocket, SD_SEND);
    if (result == SOCKET_ERROR) return shutdown_services(addrResult, &ClientSocket, "Shutdown failed, result = ", result);
    shutdown_services(addrResult, &ListenSocket, "Returned ", result);
    return 0;
}

