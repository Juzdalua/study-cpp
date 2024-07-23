#pragma once

typedef struct lNode
{
    int data;
    lNode *nextNode;
} LNode;

typedef struct linkedList
{
    int count;
    LNode *headNode; // 첫 번째 노드의 주소
} LinkedList;

void InitList(LinkedList *_pList);
void RPushList(LinkedList *_pList, int data);
void LPushList(LinkedList *_pList, int data);
void ReleaseList(LinkedList *_pList);