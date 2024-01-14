#pragma once
#include <string.h>
#include "list.h"
#include "listMessages.h"

#define TABLE_SIZE 2000
#define MAX_KEYLEN 100

typedef struct
{
    char* key;
    List* list;
    ListMessages* messages;
} TableItem;

typedef struct
{
    CRITICAL_SECTION cs;
    TableItem* items;
} HashTable;

// calculate hash based on input string
unsigned int hash(char* key);

// initialize new lists
// returns pointer to new list or NULL if error
HashTable* table_init();

// add new list to table at specified key
// returns true if successful
// returns false if error
bool table_addList(HashTable* table, char* key);

// add new message to topic at specified key
// returns true if successful
// returns false if error
bool table_addDataToTopic(HashTable* table, char* key, char* message);

// add socket to end of list at specified key (if it exists) 
// returns true if successful
// returns false if error
bool table_addSocket(HashTable* table, char* key, SOCKET value);

// get list at specified key
// returns NULL if error
List* table_get(HashTable* table, char* key);

// get list of messages for specified key
// returns NULL if error
ListMessages* table_get_msg(HashTable* table, char* key);

// look for specified key in table
// returns true if found
// returns false if not found
bool table_hasKey(HashTable* table, char* key);

// delete table and it's contents from memory
// returns true if successful
// returns false if error
bool table_dump(HashTable* table);

// prints table and it's contents
void table_print(HashTable* table);

// returns number of subscribers per topic
int get_subscribers(HashTable* table,char* key);