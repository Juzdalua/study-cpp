#include "linkedList.h"
#include <iostream>

void InitList(LinkedList *_pList)
{
    _pList->count = 0;
    _pList->headNode = nullptr;
};

void RPushList(LinkedList *_pList, int data)
{
    LNode *pNode = (LNode *)malloc(sizeof(lNode));
    pNode->data = data;
    pNode->nextNode = nullptr;

    if (_pList->count == 0)
    {
        _pList->headNode = pNode;
    }
    else
    {
        LNode *currentNode = _pList->headNode;

        while (currentNode->nextNode != nullptr)
            currentNode = currentNode->nextNode;
        currentNode->nextNode = pNode;
    }

    _pList->count++;
};

void ReleaseList(LinkedList *_pList)
{
    LNode *currentNode = _pList->headNode;
    while (currentNode != nullptr)
    {
        LNode *nextNode = currentNode->nextNode;
        free(_pList->headNode);
        currentNode = nextNode;
    }
};

void LPushList(LinkedList *_pList, int data)
{
    LNode *newNode = (LNode *)malloc(sizeof(LNode));
    newNode->data = data;
    newNode->nextNode = _pList->headNode ? _pList->headNode : nullptr;
    _pList->headNode = newNode;
    _pList->count++;
};