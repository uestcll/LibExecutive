#ifndef CLDATARECEIVER_H
#define CLDATARECEIVER_H

class CLBuffer;

class CLDataReceiver
{
public:
	CLDataReceiver();
	virtual ~CLDataReceiver();

	virtual CLStataus GetData(CLBuffer *pBuffer);

private:
	
};

#endif
