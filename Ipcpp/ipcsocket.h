#ifndef IPCSOCKET_HPP
#define IPCSOCKET_HPP

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
#include <cerrno>
#endif

namespace ipcpp
{
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

    enum class eProtocool
    {
      TCP,
      UDP
    };

    class CommunicationSocket
    {
      public:
          CommunicationSocket() = default;

          explicit CommunicationSocket(const int socketFd) :
              socketFd(socketFd)
          {}

          CommunicationSocket(const CommunicationSocket&) = delete;
          CommunicationSocket& operator=(const CommunicationSocket) = delete;

        virtual ~CommunicationSocket() noexcept
        {
#ifndef _WIN32
            close(socketFd);
#endif // !_WIN32

        }

        virtual int send(std::string& message)
        {
#ifndef _WIN32
            if (unixSysCallReturnFailed == ::send(socketFd, message.c_str(), message.size(), 0))
            {
                close(socketFd);
                throw std::runtime_error("Failed to send" + errno);
            }
#endif // !_WIN32
        }


        virtual std::string receive()
        {
#ifndef _WIN32
            constexpr int maxDataRecived = 100;
            char buffer[maxDataRecived];

            int bytes = ::recv(socketFd, buffer, maxDataRecived - 1, 0);
            if (unixSysCallReturnFailed == bytes)
            {
                throw std::runtime_error("Failed to receive data" + errno);
            }

            std::string response(buffer);
            return response;

#endif // !_WIN32
        }

    protected:
        int socketFd;
    };

    //! \brief Server socket class
    class ServerSocket
    {
    public:
        ServerSocket() = delete;

        explicit ServerSocket(std::string&& port):
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

            const int yes = 1;
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

        ~ServerSocket()
        {
#ifndef _WIN32
            close(socketFd);
#endif // !_WIN32

        }

        //! \brief accept the connection
        //! \return file descriptor for communiation socket
        int accept()
        {
#ifndef _WIN32
            struct sockaddr_storage connectedAddress;
            socklen_t size;
            int communicationFd = ::accept(socketFd, reinterpret_cast<sockaddr*>(&connectedAddress), &size);
            if (unixSysCallReturnFailed == communicationFd)
            {
                close(socketFd);
                close(communicationFd);
                std::cout<<"dupsko\n";
                throw std::runtime_error("Failed to accept" + errno);
            }

            return communicationFd;
#else // !_WIN32
#endif
        }
    private:
        std::string port;
        int socketFd;
    };

    class ClientSocket: public CommunicationSocket
    {

    public:
        ClientSocket() = delete;

        ClientSocket(const std::string& ip, std::string& port)
        {
#ifndef _WIN32
            struct addrinfo hints, *servinfo;

            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            if (0 != ::getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo))
            {
                throw std::runtime_error("Failed to get address info");
            }

            socketFd = ::socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
            if (unixSysCallReturnFailed == socketFd)
            {
                throw std::runtime_error("Failed to create socket");
            }

            if (unixSysCallReturnFailed == ::connect(socketFd, servinfo->ai_addr, servinfo->ai_addrlen))
            {
                throw std::runtime_error("Failed to connect");
            }

#endif // !_WIN32

        }

        ClientSocket(ClientSocket&&) noexcept {}
        ClientSocket&& operator=(ClientSocket&&) noexcept {}

        ClientSocket(const ClientSocket&) = delete;
        ClientSocket& operator=(ClientSocket&) = delete;
        ~ClientSocket() noexcept = default;

    private:



    };
} // namespace ipcpp

#endif
