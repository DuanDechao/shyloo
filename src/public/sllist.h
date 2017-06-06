#ifndef __SL_PUBLIC_LIST_H__
#define __SL_PUBLIC_LIST_H__
namespace sl{
class SLList;
class ISLListNode{
public:
	ISLListNode() :_prev(nullptr), _next(nullptr), _list(nullptr){}
	virtual ~ISLListNode(){}
	inline void setPrev(ISLListNode* node) { _prev = node; }
	inline ISLListNode* getPrev() const { return _prev; }
	inline void setNext(ISLListNode* node) { _next = node; }
	inline ISLListNode* getNext() const { return _next; }
	inline void setList(SLList* list) { _list = list; }
	inline SLList* getList() const { return _list;}
private:
	ISLListNode* _prev;
	ISLListNode* _next;
	SLList* _list;
};

class SLList{
public:
	SLList() :_head(nullptr), _tail(nullptr){}
	~SLList(){}

	void pushBack(ISLListNode* node){
		SLASSERT(node && node->getList() == nullptr, "wtf");
		node->setList(this);
		node->setPrev(_tail);
		node->setNext(nullptr);
		if (_tail != nullptr)
			_tail->setNext(node);
		else
			_head = node;
		
		_tail = node;
	}

	ISLListNode* popFront(){
		ISLListNode* node = _head;
		if (_head != nullptr){
			_head = _head->getNext();
			if (_head == nullptr)
				_tail = nullptr;

			node->setPrev(nullptr);
			node->setNext(nullptr);
			node->setList(nullptr);
		}

		return node;
	}

	void remove(ISLListNode* node){
		SLASSERT(node && node->getList() == this, "wtf");

		if (node->getNext()){
			node->getNext()->setPrev(node->getPrev());
		}

		if (node->getPrev()){
			node->getPrev()->setNext(node->getNext());
		}

		if (node == _head){
			_head = node->getNext();
		}
		if (node == _tail){
			_tail = node->getPrev();
		}

		node->setPrev(nullptr);
		node->setNext(nullptr);
		node->setList(nullptr);
	}

	void swap(SLList& list){
		std::swap(_head, list._head);
		std::swap(_tail, list._tail);
	}

	inline bool isEmpty() { return _head == nullptr; }

private:
	SLList(const SLList&);
	SLList& operator = (const SLList&);

private:
	ISLListNode* _head;
	ISLListNode* _tail;
};
}
#endif