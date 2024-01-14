#pragma once
#include "listMessages.h"

ListMessages* list_init_msg(int limit)
{
    ListMessages* lista = (ListMessages*)malloc(sizeof(ListMessages));
    lista->head = NULL;
    lista->limit = limit;
    lista->len = 0;

    return lista;
}

bool list_addAtEnd_msg(ListMessages* list, char* data) {
    if (list == NULL || list->len >= list->limit) {
        return false; // Invalid list or list is full
    }

    ListItemMessages* item = (ListItemMessages*)malloc(sizeof(ListItemMessages));
    if (item == NULL) {
        return false; // Memory allocation error
    }

    item->data = _strdup(data); // Use _strdup instead of strdup
    if (item->data == NULL) {
        free(item);
        return false; // Memory allocation error
    }

    item->next = NULL;

    if (list->head == NULL) {
        list->head = item;
    }
    else {
        ListItemMessages* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = item;
    }

    list->len++;
    return true;
}


char* list_getAt_msg(ListMessages* list, int index)
{
    if (list == NULL)
    {
        printf("List uninitialized.\n");
        return NULL;
    }
    if (list->len <= index)
    {
        printf("Index out of bounds.\n");
        return NULL;
    }
    if (index == 0)
    {
        return list->head->data;
    }

    ListItemMessages* current = list->head;

    for (int i = 1; i <= index; i++)
    {
        current = current->next;
    }

    return current->data;
}

bool list_deleteAt_msg(ListMessages* list, int index)
{
    ListMessages* lista = list;
    ListItemMessages* pomocni = lista->head;
    ListItemMessages* temp = NULL;
    temp = (ListItemMessages*)malloc(sizeof(ListItemMessages));

    if (lista->len < index) {
        printf("Item not found!\n");
        return false;
    }

    for (int i = 1; i < index - 1; i++)
    {
        if (pomocni->next == NULL)
        {
            return false;
        }
        pomocni = pomocni->next;
    }

    temp = pomocni->next;
    pomocni->next = temp->next;

    lista->len--;
    free(temp);

    return true;
}

bool list_dump_msg(ListMessages* list)
{
    if (list == NULL)
    {
        printf("list is nullptr\n");
        return false;
    }

    if (list->len == 0)
    {
        free(list);
        return true;
    }

    ListItemMessages* current = list->head;
    ListItemMessages* temp;

    while (current != NULL)
    {
        temp = current->next;
        // Assuming data is not a socket, so no need to close it
        free(current);
        current = temp;
    }

    list->head = NULL;

    return true;
}

void printListMessages(const ListMessages* list) {
    if (list == NULL) {
        printf("ListMessages is NULL\n");
        return;
    }

    ListItemMessages* current = list->head;

    while (current != NULL) {
        printf("%s ", current->data);
        current = current->next;
    }

    printf("\n");
}