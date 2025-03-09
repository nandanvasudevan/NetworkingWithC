#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "logger/logger.h"

#include "server.h"

#define PORT 8081

#define SOCK_RECV_CLIENT_LEFT 0

typedef struct
{
    int fd;
    struct sockaddr_in addr;
    unsigned int connectionIndex;
} SConnectionInfo;

void *handleClient(void *arg);

int main(void)
{
    struct sockaddr_in server_addr;
    unsigned int activeConnections = 0;

    SConnectionInfo connections[MAX_CONNECTIONS] = {};

    LOG(LOG_LEVEL_INFO, "Web Server %d.%d.%d", C_WEB_SERVER_VERSION_MAJOR,
                                    C_WEB_SERVER_VERSION_MINOR,
                                    C_WEB_SERVER_VERSION_BUILD);

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    const int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    if(INVALID_FD == serverFd)
    {
        LOG(LOG_LEVEL_FATAL, "Cannot open socket!");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    const int bindSuccess = bind(serverFd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if(bindSuccess < 0)
    {
        LOG(LOG_LEVEL_FATAL, "Bind failed!");
    }

    const int listenSuccess = listen(serverFd, 10);

    if(listenSuccess < 0)
    {
        LOG(LOG_LEVEL_FATAL, "Listen failed!");
    }

    LOG(LOG_LEVEL_INFO, "Listening on port: %d", PORT);

    while(true)
    {
        pthread_t threadId;
        struct sockaddr_in client_addr;
        int len = sizeof(client_addr);

        LOG(LOG_LEVEL_INFO, "Waiting for client..."); 

        const int clientFd = accept(serverFd, (struct sockaddr *)&client_addr, &len);

        if(clientFd < 0)
        {
            LOG(LOG_LEVEL_FATAL, "Could not accept client connections!");
        }

        LOG(LOG_LEVEL_INFO, "Accepted connection.");

        const char* welcomeMessage = "I see you!\n";
        send(clientFd, welcomeMessage, strlen(welcomeMessage), 0);

        const char * clientIp = inet_ntoa(client_addr.sin_addr);

        LOG(LOG_LEVEL_INFO, "New connetion from %s:%d", clientIp, client_addr.sin_port);

        SConnectionInfo * activeConnection = &connections[activeConnections];
        activeConnection->fd = clientFd;
        activeConnection->addr = client_addr;
        activeConnection->connectionIndex = activeConnections;

        activeConnections++;

        pthread_create(&threadId, NULL, handleClient, (void*)activeConnection);
        pthread_detach(threadId);
    }

    LOG(LOG_LEVEL_INFO, "Closing server...");

    const int closeSuccess = close(serverFd);

    if(closeSuccess >= 0)
    { 
        LOG(LOG_LEVEL_INFO, "Server closed.");
    }
    
    return 0;
}

void *handleClient(void *arg)
{
    int readBytes = -1;
    char clientIp[20] = {};
    SConnectionInfo *connection = (SConnectionInfo *)arg;
    struct sockaddr_in client = connection->addr;
    bool connectionClosed = false;

    const char* ip = inet_ntoa(client.sin_addr);
    strncpy(clientIp, ip, strlen(ip));

    do
    {
        char buffer[1024] = {};
        readBytes = recv(connection->fd, buffer, sizeof(buffer), 0);
        if(readBytes < 0)
        {
            LOG(LOG_LEVEL_FATAL, "Read from socket failed! Error: %s (%d)", strerror(errno), errno);
        }

        if(readBytes > 0)
        {
            LOG(LOG_LEVEL_INFO, "%s:%d - %s", clientIp, client.sin_port, buffer);
        }

        const int sendSuccess = send(connection->fd, "", 1, MSG_NOSIGNAL);

        if(sendSuccess < 0)
        {
            connectionClosed = true;
            LOG(LOG_LEVEL_WARN, "Client %s:%d left. (%d) %s (%d)", clientIp, client.sin_port, 
                                                                   sendSuccess, strerror(errno), errno);
            break;
        }
    } while(true);

    if(!connectionClosed)
    {
        const int shutdownSuccess = shutdown(connection->fd, SHUT_RDWR);

        if(shutdownSuccess < 0)
        {
            LOG(LOG_LEVEL_FATAL, "Shutdown failed! Error: %s (%d)", strerror(errno), errno);
        }
    }
    else
    {
        LOG(LOG_LEVEL_INFO, "Connection closed by client.");
        const int closeSuccess = close(connection->fd);

        if(closeSuccess < 0)
        {
            LOG(LOG_LEVEL_FATAL, "Close failed! Error: %s (%d)", strerror(errno), errno);
        }
    }
    
    const int currentFd = fcntl(connection->fd, F_GETFD);

    if(currentFd > 0)
    {
        LOG(LOG_LEVEL_FATAL, "FD not closed properly! FD = %d", currentFd);
    }

    LOG(LOG_LEVEL_INFO, "Connection with %s:%d closed.", clientIp, client.sin_port);

    return NULL;
}