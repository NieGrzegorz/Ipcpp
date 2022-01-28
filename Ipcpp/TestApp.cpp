// Ipcpp.lib.cpp : Defines the entry point for the application.
//
#include "ipcsocket.h"
#include <thread>

void HandleConnection(const int communicationFd)
{
    ipcpp::CommunicationSocket socket(communicationFd);
    auto rc = socket.receive();
    std::cout<<rc<<std::endl;

}

int main()
{
    ipcpp::ServerSocket server("1921");
    while (1)
    {
        auto fd = server.accept();
        std::thread connectionHandler(HandleConnection, fd);
        connectionHandler.join();
    }
    return 0;
}
