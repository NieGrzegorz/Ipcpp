// Ipcpp.lib.cpp : Defines the entry point for the application.
//
#include "ipcsocket.h"
#include <thread>

using namespace std;

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
		std::thread acceptThread(HandleConnection, server.accept());
		acceptThread.join();
	}
	return 0;
}
