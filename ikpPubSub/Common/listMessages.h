#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <WinSock2.h>

#define DEFAULT_LIST_LIMIT 100

typedef struct ListItemMsg_st
{
	char* data;
	struct ListItemMsg_st* next;
} ListItemMessages;

typedef struct ListMsg_st
{
	ListItemMessages* head;
	int len;
	int limit;
} ListMessages;

// initialize new list
// limit - max ammount of elements in list
// returns pointer to new list or NULL if error
ListMessages* list_init_msg(int limit);

// add item to end of list if under the size limit
// returns true if successful
// returns false if error
bool list_addAtEnd_msg(ListMessages* list, char* data);

// gets socket handle at specified index
// returns null if error
char* list_getAt_msg(ListMessages* list, int index);

// deletes item from list at specified index
// returns true if successful
// returns false if error
bool list_deleteAt_msg(ListMessages* list, int index);

// free list and all elements from memory
// returns true if successful
// returns false if error
bool list_dump_msg(ListMessages* list);