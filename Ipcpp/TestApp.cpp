// Ipcpp.lib.cpp : Defines the entry point for the application.
//
#include "ipcsocket.hpp"
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

    auto fd = server.accept();
    HandleConnection(fd);

    return 0;
}
