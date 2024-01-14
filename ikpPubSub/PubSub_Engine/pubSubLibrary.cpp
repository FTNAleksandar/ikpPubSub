#include "pubSubLibrary.h"   
#include "../Common/hashtable.cpp"
#include <thread>
#include "../Common/listMessages.cpp"


void handlePublisher(SOCKET acceptSocket, HashTable* table)
{
    while (1)
    {
        PublisherData receivedData;
        List* subscribers;
        ListMessages* messages;

        // Receive the structure from the client
        int recvBytes = recv(acceptSocket, reinterpret_cast<char*>(&receivedData), sizeof(PublisherData), 0);
        if (recvBytes == sizeof(PublisherData))
        {
            printf("Message from Publisher %d:\n Topic: %s\n Message: %s\n", acceptSocket, receivedData.topic, receivedData.message);
            if (!table_hasKey(table, receivedData.topic))
            {
                printf("First message for topic %s, creating topic...\n", receivedData.topic);
                table_addList(table, receivedData.topic);
                table_addDataToTopic(table, receivedData.topic, receivedData.message); 

            }
            else
            {
                table_addDataToTopic(table, receivedData.topic, receivedData.message);
                printf("Adding message to topic \n");

                if ((subscribers = table_get(table, receivedData.topic)) == NULL)
                {
                    printf("Error reading list of subscribers.\n");
                }
                if ((messages = table_get_msg(table, receivedData.topic)) == NULL)
                {
                    printf("Error reading list of messages.\n");
                }

                ListItem* current = subscribers->head;
                while (current != NULL)
                {
                        if (send(current->data, receivedData.message, strlen(receivedData.message) + 1, 0) == SOCKET_ERROR)
                        {
                            printf("send failed with error %d\n", WSAGetLastError());
                            return;
                        }
                    current = current->next;
                }

                printf("Successfully sent message to %d subscribers.\n", get_subscribers(table,receivedData.topic));
            }
        }
               
        else if (recvBytes == 0)
        {
            // Connection closed by the client
            printf("Client disconnected.\n");
            break;
        }
        else
        {
            // Error in recv
            printf("recv failed with error %d\n", WSAGetLastError());
            break;
        }
    }
}

DWORD WINAPI pubAccept(LPVOID lpParameter)
{
    HashTable* table = static_cast<HashTable*>(lpParameter);

    // Prepare address information structures
    addrinfo* resultingAddress = NULL;
    addrinfo hints;

    // store function return value
    int iResult;

    SOCKET acceptSocket = INVALID_SOCKET;
    SOCKET listenSocket = INVALID_SOCKET;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4 address
    hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;     // 

    // Resolve the server address and port for publishers
    iResult = getaddrinfo(NULL, PUB_PORT, &hints, &resultingAddress);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for publishers to connect to server
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }

    // Setup the TCP publisher listening socket - bind port number and local address to socket
    iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Since we don't need resultingAddress any more, free it
    freeaddrinfo(resultingAddress);

    // Set listenSocket in listening mode 
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Accepting new publisher connections...\n");

    while (TRUE)
    {
        SOCKADDR_IN sockAddr;
        int addrLen = sizeof(sockAddr);

        // Accept publisher/subscriber socket connection
        SOCKET acceptSocket = accept(listenSocket, (sockaddr*)&sockAddr, &addrLen);

        if (WSAGetLastError() != WSAEWOULDBLOCK && acceptSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        printf("Publisher connected.\n");

        std::thread publisherThread(handlePublisher, acceptSocket, std::ref(table));
        publisherThread.detach();
    }
}


DWORD WINAPI subAccept(LPVOID lpParameter)
{

    HashTable* table = static_cast<HashTable*>(lpParameter);
    // Prepare address information structures
    addrinfo* resultingAddress = NULL;
    addrinfo hints;

    // store function return value
    int iResult;

    SOCKET acceptSocketSub = INVALID_SOCKET;
    SOCKET listenSocketSub = INVALID_SOCKET;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4 address
    hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;     // 

    // Resolve the server address and port for publishers
    iResult = getaddrinfo(NULL, SUB_PORT, &hints, &resultingAddress);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for publishers to connect to server
    listenSocketSub = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocketSub == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }

    // Setup the TCP publisher listening socket - bind port number and local address to socket
    iResult = bind(listenSocketSub, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocketSub);
        WSACleanup();
        return 1;
    }

    // Since we don't need resultingAddress any more, free it
    freeaddrinfo(resultingAddress);

    // Set listenSocket in listening mode 
    iResult = listen(listenSocketSub, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocketSub);
        WSACleanup();
        return 1;
    }

    printf("Accepting new subscriber connections...\n");

    while (TRUE)
    {
        SOCKADDR_IN sockAddr;
        int addrLen = sizeof(sockAddr);

        // Accept publisher socket connection
        acceptSocketSub = accept(listenSocketSub, (sockaddr*)&sockAddr, &addrLen);
        if (WSAGetLastError() != WSAEWOULDBLOCK && acceptSocketSub == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocketSub);
            closesocket(acceptSocketSub);
            WSACleanup();
            return 1;
        }

        printf("Subscriber %d connected.\n", acceptSocketSub);

        char receivedTopic[MAX_BUFFER_SIZE] = "";

        // Receive the structure from the client
        int recvBytes = recv(acceptSocketSub, receivedTopic, MAX_BUFFER_SIZE, 0);

        if (recvBytes > 0)
        {
            printf("Subscriber %d requesting subscription to topic %s\n", acceptSocketSub, receivedTopic);

            if (table_hasKey(table, receivedTopic))
            {
                if (table_addSocket(table, receivedTopic, acceptSocketSub))
                {
                    printf("Subscrbier %d subscribed to topic %s successfully\n", acceptSocketSub, receivedTopic);

                    //Send all messages on topic
                    ListMessages* messages;
                    messages = table_get_msg(table, receivedTopic);

                    ListItemMessages* currentMsg = messages->head;
                    while (currentMsg != NULL) {
                        
                        if (send(acceptSocketSub, currentMsg->data, strlen(currentMsg->data) + 1, 0) == SOCKET_ERROR)
                        {
                            printf("send failed with error %d\n", WSAGetLastError());
                            return 1;
                        }
                        Sleep(300);
                        currentMsg = currentMsg->next;
                    }
                    
                }
                else
                {
                    printf("Failed to subscribe subscriber %d to topic %s\n", acceptSocketSub, receivedTopic);
                }
            }
            else
            {
                printf("Specified topic does not exist. Sending report to subscriber %d\n", acceptSocketSub);

                char errorMsg[] = "Topic does not exist!\n";
                if (send(acceptSocketSub, errorMsg, strlen(errorMsg) + 1, 0) == SOCKET_ERROR)
                {
                    printf("Send failed with error %d\n", WSAGetLastError());
                }

                return 1;
            }
        }
        else if (recvBytes == 0)
        {
            // Connection closed by the client
            printf("Client disconnected.\n");
            break;
        }
        else
        {
            // Error in recv
            printf("recv failed with error %d\n", WSAGetLastError());
            break;
        }
        
    }
}
