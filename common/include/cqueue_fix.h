#ifndef circle_queue_fixbuff_h__
#define circle_queue_fixbuff_h__

namespace comm
{

#define CQF_HEADER_SIZE   64

/**
* 已分配好的内存上建立起环形队列，比如共享内存
* circle_queue_fixbuff<Elem> *pcq = (circle_queue_fixbuff<Elem> *)sharem;
* pcq->initialize(sharem_size)
* 使用场景：IPC通信
*/
template<class value_type>
class circle_queue_fixbuff
{
private: //can't new circle_queue_fixbuff directly
	circle_queue_fixbuff()
	{
	}

public:
	void initialize(int fix_buff_size)
	{
		if (fix_buff_size <= (CQF_HEADER_SIZE + sizeof(value_type) * 2))
		{
			return;  //error
		}

		m_head = m_tail = m_tag = 0;
		m_size = 0;
		m_maxsize = (fix_buff_size - CQF_HEADER_SIZE) / sizeof(value_type);
		m_reader_prepared = 0;
		m_writer_prepared = 0;
	}
	
	void push(const value_type& _Val)
	{	
		//need call if(!full()) first	
		m_elems[m_tail] = _Val;
		m_tail = (m_tail + 1) % m_maxsize;
		if (m_head==m_tail)
		{
			m_tag = 1;
		}

		m_size++;
	}

	void pop(value_type& _Val)
	{
		//need call if(!empty()) first
		_Val = m_elems[m_head];
		m_head = (m_head + 1) % m_maxsize;

		if (m_head==m_tail)
		{
			m_tag = 0;
		}

		m_size--;
	}

	bool empty() const
	{	
		return m_head==m_tail && m_tag==0;
	}

	bool full() const 
	{
		return m_head==m_tail && m_tag==1;
	}

	int size() const
	{
		return m_size;
	}

	int maxsize() const
	{
		return m_maxsize;
	}

	void setReaderPrepared(int value)
	{
		m_reader_prepared = value;
	}

	int getReaderPrepared(){
		return m_reader_prepared;
	}

	void setWriterPrepared(int value)
	{
		m_writer_prepared = value;
	}

	int getWriterPrepared(){
		return m_writer_prepared;
	}

protected:
	int m_head;
	int m_tail;
	int m_tag;
	int m_size;
	int m_maxsize;
	int m_reader_prepared;
	int m_writer_prepared;

	value_type m_elems[1]; //value_type array more than 1 in realy
};

}
#endif
