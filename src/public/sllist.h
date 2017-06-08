#ifndef __SL_PUBLIC_LIST_H__
#define __SL_PUBLIC_LIST_H__
namespace sl{
class SLList;
class ISLListNode{
public:
	ISLListNode() :_prev(nullptr), _next(nullptr), _list(nullptr), _ignoreOwner(false){}
	virtual ~ISLListNode(){}
	inline void setPrev(ISLListNode* node) { _prev = node; }
	inline ISLListNode* getPrev() const { return _prev; }
	inline void setNext(ISLListNode* node) { _next = node; }
	inline ISLListNode* getNext() const { return _next; }
	inline void setList(SLList* list) { _list = list; }
	inline SLList* getList() const { return _list;}

	inline void setIgnoreOwner(bool ignore) { _ignoreOwner = ignore; }
	inline bool ignoreOwner() const { return _ignoreOwner; }

private:
	ISLListNode* _prev;
	ISLListNode* _next;
	SLList* _list;
	bool _ignoreOwner;
};

class SLList{
public:
	SLList() :_head(nullptr), _tail(nullptr){}
	~SLList(){}

	inline ISLListNode* getHead() const { return _head; }
	inline ISLListNode* getTail() const { return _tail; }

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

	ISLListNode* front(){
		return _head;
	}

	void remove(ISLListNode* node){
		SLASSERT(node, "wtf");
		if (!node->ignoreOwner() && node->getList() != this){
			SLASSERT(false, "wtf");
			return;
		}

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

	void merge(SLList& list){
		if (list.isEmpty())
			return;

		if (isEmpty()){
			_head = list._head;
			_tail = list._tail;
		}
		else{
			_tail->setNext(list.getHead());
			_tail = list.getTail();
		}
	}

	inline bool isEmpty() { return _head == nullptr; }

	inline void clear() { _head = nullptr; _tail = nullptr; }

private:
	SLList(const SLList&);
	SLList& operator = (const SLList&);

private:
	ISLListNode* _head;
	ISLListNode* _tail;
};
}
#endif