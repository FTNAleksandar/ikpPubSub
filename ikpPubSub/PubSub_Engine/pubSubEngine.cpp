#include "pubSubLibrary.h"

int main()
{

    HashTable* table = table_init();
    if (table == NULL)
    {
        printf("table_init failed\n");
        return 1;
    }

    if (initializeWindowsSockets() == false)
    {
        // not logging anything cause the function itself logs errors 
        return 1;
    }


    HANDLE pubAcceptThread = CreateThread(NULL, 0, pubAccept, table, 0, NULL);
    HANDLE subAcceptThread = CreateThread(NULL, 0, subAccept, table, 0, NULL);

    getchar();

    CloseHandle(pubAcceptThread);
    CloseHandle(subAcceptThread);
    WSACleanup();
    return 0;
}