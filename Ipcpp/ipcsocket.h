#pragma once

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef linux
#include <string.h>
#include <sys/unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

namespace ipcpp
{
    //Const out of range of known ports
    constexpr unsigned int unassignedPort = 70000;
    constexpr int unixSysCallReturnFailed = -1;
    constexpr unsigned int backlog = 10;

    auto getInAddrType(struct sockaddr* sa)
    {
#ifndef _WIN32
        if (AF_INET == sa->sa_family)
        {
            return reinterpret_cast<sockaddr_in*>(sa)->sin_addr;
        }
        else
        {
            return reinterpret_cast<sockaddr_in*>(sa)->sin_addr;
        }
#endif // !_WIN32
    }

    class CommunicationSocket
    {
    public:
        CommunicationSocket() = delete;

        CommunicationSocket(const int socketFd) :
            socketFd(socketFd)
        {

        }

        virtual ~CommunicationSocket()
        {
#ifndef _WIN32
            close(socketFd);
#endif // !_WIN32

        }

        int send(std::string& message)
        {
#ifndef _WIN32
            if (unixSysCallReturnFailed == ::send(socketFd, message.c_str(), message.size(), 0))
            {
                throw std::runtime_error("Failed to send");
            }
#endif // !_WIN32
        }


        int receive() {}

    private:
        const int socketFd;
    };

    class ServerSocket
    {
    public:
        ServerSocket() = delete;

        /// <summary>
        /// Constructs the ServerSocket and binds it into a port
        /// </summary>
        /// <param name="port">Port to which ServerSocket is bound</param>
        /// <returns></returns>
        ServerSocket(std::string&& port) :
            port(port),
            socketFd(0)
        {
#ifndef _WIN32
            struct addrinfo hints, * servinfo;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = AI_PASSIVE;

            if (0 != ::getaddrinfo(nullptr, port.c_str(), &hints, &servinfo))
            {
                throw std::runtime_error("Failed to get address info");
            }

            socketFd = ::socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
            if (unixSysCallReturnFailed == socketFd)
            {
                throw std::runtime_error("Failed to create socket");
            }

            int yes = 1;
            if (unixSysCallReturnFailed == ::setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
            {
                throw std::runtime_error("Failed to setsockopt");
            }

            if (unixSysCallReturnFailed == ::bind(socketFd, servinfo->ai_addr, servinfo->ai_addrlen))
            {
                throw std::runtime_error("Failed to bind the socket");
            }
            freeaddrinfo(servinfo);

            if (unixSysCallReturnFailed == ::listen(socketFd, ipcpp::backlog))
            {
                throw std::runtime_error("Failed to listen");
            }

#else // !_WIN32
#endif
        }

        /// <summary>
        /// Move socket
        /// </summary>
        /// <param name="move">Object to move</param>
        /// <returns>Takes ownership of an object</returns>
        ServerSocket(ServerSocket&& move) noexcept :
            socketFd(move.socketFd),
            port(move.port)
        {
        }

        ServerSocket&& operator=(ServerSocket&&) noexcept {}

        ServerSocket(const ServerSocket&) = delete;
        ServerSocket& operator=(ServerSocket) = delete;

        //Return communication socket
        CommunicationSocket accept()
        {
#ifndef _WIN32
            struct sockaddr_storage connectedAddress;
            socklen_t size;
            int communicationFd = ::accept(socketFd, reinterpret_cast<sockaddr*>(&connectedAddress), &size);
            if (unixSysCallReturnFailed == communicationFd)
            {
                throw std::runtime_error("Failed to accept");
            }

            return CommunicationSocket(communicationFd);


#else // !_WIN32
#endif

        }
    private:

        std::string port;
        int socketFd;

    };

    class ClientSocket
    {

    public:
        ClientSocket() = default;
        ClientSocket(const std::string& ip, const int port) {}
        ClientSocket(ClientSocket&&) noexcept {}
        ClientSocket&& operator=(ClientSocket&&) noexcept {}


        ClientSocket(const ClientSocket&) = delete;
        ClientSocket& operator=(ClientSocket&) = delete;
        ~ClientSocket() {}

        int connect() {}
        int send() {}
        int receive() {}

    private:



    };

}
