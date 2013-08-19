#ifndef CLMessage_H
#define CLMessage_H

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