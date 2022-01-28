// Ipcpp.lib.cpp : Defines the entry point for the application.
//
#include "ipcsocket.h"
#include <thread>

void HandleConnection(const int communicationFd)
{
    ipcpp::CommunicationSocket socket(communicationFd);
    std::string msg = "Oh Hai!";
    socket.send(msg);
}

int main()
{
    ipcpp::ServerSocket server("1921");
    while (1)
    {
        HandleConnection(server.accept()); 
    }
    return 0;
}
