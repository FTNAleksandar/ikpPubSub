#pragma once
#include <WS2tcpip.h>
#include "../Common/connect.h"
#include "../Common/hashtable.h"
#define MAX_BUFFER_SIZE 1024

//Handles publishers in treads
void handlePublisher(SOCKET acceptSocket, HashTable* table);

// accepts publisher connections 
DWORD WINAPI pubAccept(LPVOID lpParameter);

// accepts subscriber connections 
DWORD WINAPI subAccept(LPVOID lpParameter);
