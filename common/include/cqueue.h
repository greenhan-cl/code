#ifndef CIRCLE_QUEUE_H__
#define CIRCLE_QUEUE_H__

#include <cassert>

namespace comm
{
/**
* 环形队列
*/
template<class value_type>
class circle_queue
{
public:
	circle_queue(int maxsize)
	{
		assert(maxsize>0);
		m_head = m_tail = 0;
		m_size = 0;		
		m_maxsize = (maxsize + 1);
		m_elems = new value_type[m_maxsize];
	}

	~circle_queue()
	{
		delete[] m_elems;
	}

	void push(const value_type& _Val)
	{	
		//need call if(!full()) first		
		m_tail = (m_tail + 1) % m_maxsize;
		m_elems[m_tail] = _Val;
		m_size++;
	}

	void pop(value_type& _Val)
	{
		//need call if(!empty()) first
		m_head = (m_head + 1) % m_maxsize;
		_Val = m_elems[m_head];

#if !defined(CIRCLEQ_VALUETYPE_ISPTR)
		m_elems[m_head].reset(); //elem must has reset
#endif
		m_size--;
	}

	const value_type& get()
	{
		int pos = (m_head + 1) % m_maxsize;
		return m_elems[pos];
	}

	int canPutPos()
	{
		return (m_tail + 1) % m_maxsize;
	}

	bool empty() const
	{	
		return m_size == 0;
	}

	int size() const
	{
		return m_size;
	}

	int maxsize() const
	{
		return m_maxsize - 1;
	}

	bool full() const 
	{
		return (m_tail + 1) % m_maxsize == m_head;
	}

	void reset()
	{
		m_head = m_tail = 0;
		m_size = 0;
	}

protected:
	int m_head;
	int m_tail;
	int m_size;
	int m_maxsize;
	value_type* m_elems;
};

}
#endif
