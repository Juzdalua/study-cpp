#pragma once

template <typename T>
struct cListNode
{
    T data;
    cListNode<T> *pPrevNode;
    cListNode *pNextNode; // 템플릿 내부에서 본인을 지칭할 때, T를 생략 가능.

    cListNode() : data(), pPrevNode(nullptr), pNextNode(nullptr)
    {
    }

    cListNode(const T &data, cListNode<T> *_pPrev, cListNode<T> *_pNext)
        : data(data), pPrevNode(_pPrev), pNextNode(_pNext)
    {
    }

    ~cListNode() {}
};

template <typename T>
class CList
{
private:
    cListNode<T> *pHeadNode;
    cListNode<T> *pTailNode;
    int count;

public:
    void RPush(const T &data);
    void LPush(const T &data);

public:
    CList();
    ~CList();
};

template <typename T>
CList<T>::CList() : pHeadNode(nullptr), pTailNode(nullptr), count(0){};

template <typename T>
CList<T>::~CList()
{
    cListNode<T> *pDeleteNode = this->pHeadNode;

    while (pDeleteNode)
    {
        cListNode<T> *_pNext = pDeleteNode->pNextNode;
        delete (pDeleteNode);
        pDeleteNode = _pNext;
    }
};

template <typename T>
void CList<T>::RPush(const T &_data)
{
    cListNode<T> *pNewNode = new cListNode<T>(_data, nullptr, nullptr);

    if (this->pHeadNode == nullptr)
    {
        this->pHeadNode = pNewNode;
    }
    else
    {
        this->pTailNode->pNextNode = pNewNode;
        pNewNode->pPrevNode = this->pTailNode;
    }

    this->pTailNode = pNewNode;
    this->count++;
};

template <typename T>
void CList<T>::LPush(const T &_data)
{
    cListNode<T> *pNewNode = new cListNode<T>(_data, nullptr, this->pHeadNode);

    if (this->pHeadNode != nullptr)
    {
        this->pHeadNode->pPrevNode = pNewNode;
    }

    this->pHeadNode = pNewNode;
    this->count++;
};