#ifndef CLMESSAGERECEIVER_H
#define CLMESSAGERECEIVER_H

class CLMessage;

class CLMessageReceiver
{
public:
	CLMessageReceiver();
	virtual ~CLMessageReceiver();

	virtual CLMessage *GetMessage() = 0;

	const int& m_ciFd;
private:
	CLMessageReceiver(const CLMessageReceiver&);
	CLMessageReceiver& operator=(const CLMessageReceiver&);

protected:
	int m_Fd;
};

#endif

