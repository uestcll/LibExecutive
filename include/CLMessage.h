#ifndef CLMessage_H
#define CLMessage_H

/*
用户可定义自己的消息，即从该类派生
CLMessage及派生类应从堆中分配，当消息处理函数返回后，该消息会被自动删除，即不必调用delete
*/
class CLMessage
{
public:
	CLMessage(unsigned long lMsgID);
	virtual ~CLMessage();

public:
	const unsigned long& m_clMsgID;

private:
	CLMessage(const CLMessage&);
	CLMessage& operator=(const CLMessage&);

protected:
	unsigned long m_lMsgID;
};

#endif