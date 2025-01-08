#include "LineIndex.h"

#include <list>
#include <algorithm>

void DCM::LineIndex::updateIndex()
{
	if (prev != nullptr)
		index = prev->getEndIndex() + 1;
	else
		index = 0;
}
void DCM::LineIndex::updateOrder()
{
	if (prev != nullptr)
		order = prev->getOrder() + 1;
	else
		order = 0;
}
void DCM::LineIndex::update()
{
	updateIndex();
	updateOrder();

	if (index < 0 || length < 0) // 아직 정보가 없는 상태
		return;

	//int curIndex = index + length;
	//int curOrder = order + 1;
	auto itr = next;
	while (itr != nullptr)
	{
		//itr->index = curIndex;
		//itr->order = curOrder;
		//curIndex += itr->length;
		//curOrder += 1;
		itr->updateIndex();
		itr->updateOrder();

		itr = itr->next;
	}
}

DCM::LineIndex::LineIndex(int index, int length)
	: index(index), length(length), order(0), prev(nullptr), next(nullptr)
{

};
DCM::LineIndex::LineIndex(LineIndex* _prev, LineIndex* _next)

{
	updateIndex();
	updateOrder();

	if (_prev == nullptr)
		return;

	if (next != nullptr)
#ifndef max
		length = std::max(1, next->getIndex() - index);
#else
		length = max(1, next->getIndex() - index);
#endif
	else
		length = 1;

	attachPrev(_prev);
	attachNext(_next);
}
int DCM::LineIndex::getIndex()
{
	updateIndex();
	return index;
}
int DCM::LineIndex::getEndIndex()
{
	return getIndex() + length - 1;
}
int DCM::LineIndex::getOrder()
{
	updateOrder();
	return order;
}
void DCM::LineIndex::putLength(int len)
{
	length = len;
	update();
}
void DCM::LineIndex::putEndIndex(int endIndex)
{
	length = endIndex - getIndex() + 1;
	update();
}
/* attch와 detach는 하나의 연결리스트만 존재한다고 가정하고 작동한다 */
/* 현재의 prev를 until까지 떼어내고 없앤다 즉 until 이전까지는 없애지 않는다 왜냐하면 새로운 prev가 될 수 있으므로 */
void DCM::LineIndex::detachPrev(DCM::LineIndex* until)
{
	if (prev != nullptr)
	{
		std::list<LineIndex*> trashBin;
		auto itr = prev;
		while (itr != until && itr != nullptr && itr != this)
		{
			trashBin.push_front(itr);
			itr = itr->prev;
		}
		for (auto trash : trashBin)
			delete trash;
		prev = nullptr;

		updateIndex();
		updateOrder();

		//update();
	}
}
/* 현재의 prev를 detach하고 새로운 _prev를 붙인다 */
void DCM::LineIndex::attachPrev(DCM::LineIndex* _prev)
{
	if (_prev == prev)
		return;

	detachPrev(_prev);
	prev = _prev;
	if (prev != nullptr)
	{
		prev->attachNext(this);
		update();
	}
}
void DCM::LineIndex::detachNext(DCM::LineIndex* until)
{
	std::list<LineIndex*> trashBin;
	auto itr = next;
	while (itr != until && itr != nullptr && itr != this)
	{
		trashBin.push_front(itr);
		itr = itr->next;
	}
	for (auto trash : trashBin)
		delete trash;
	next = nullptr;
	//update();	
}
void DCM::LineIndex::attachNext(DCM::LineIndex* _next)
{
	if (_next == next)
		return;

	detachNext(_next);
	next = _next;
	if (next != nullptr)
	{
		next->attachPrev(this);
		update();
	}
}
/*  */
void DCM::LineIndex::attach(DCM::LineIndex* _prev, DCM::LineIndex* _next)
{
	//prev = _prev;
	//next = _next;
	attachPrev(_prev);
	attachNext(_next);
}
void DCM::LineIndex::detach(DCM::LineIndex* _prev, DCM::LineIndex* _next)
{
	detachPrev(_prev);
	detachNext(_next);
}

void DCM::LineIndex::independent()
{
	if(prev != nullptr)
		prev->detachNext(this); // 삭제 없이 연결만 끊기 prev->next = nullptr
	if(next != nullptr)
		next->detachPrev(this); // 자신과 연결 안끊고 attach하면 자신이 삭제될 수 있음

	if(prev != nullptr)
		prev->attachNext(next); //
	if(next != nullptr)
		next->attachPrev(prev); // prev->attachNext(next)에 next->attachPrev 있어서 사실상 중복

	prev = nullptr;
	next = nullptr;

	updateIndex();
	updateOrder();
}
void DCM::LineIndex::resetIndex()
{
	if (prev == nullptr)
		index = 0;
	update();
}
DCM::LineIndex* DCM::LineIndex::findIndex(int line)
{
	int curEndIndex = getEndIndex();
	int curIndex = getIndex();

	if (line < curIndex)
		return prev->findIndex(line);
	else if (line >= curIndex && line <= curEndIndex)
		return this;
	else
		return next->findIndex(line);
}
DCM::LineIndex::LineIndex(const DCM::LineIndex& other)
	: index(other.index), length(other.length), order(other.order), prev(other.prev), next(other.next) {};
DCM::LineIndex& DCM::LineIndex::operator=(const DCM::LineIndex& other)
{
	if (this != &other)
	{
		index = other.index;
		length = other.length;
		order = other.order;
		prev = other.prev; // 기존 연결관계를 어떻게 할지 안 정함
		next = other.next;
	}
	return *this;
}
DCM::LineIndex::~LineIndex()
{
	independent();
}