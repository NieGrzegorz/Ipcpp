// Ipcpp.lib.cpp : Defines the entry point for the application.
//
#include "ipcsocket.h"

using namespace std;

void HandleConnection(ipcpp::CommunicationSocket socket)
{
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
