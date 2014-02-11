#ifndef CLTCPConnectedSocket_H
#define CLTCPConnectedSocket_H

class CLTCPConnectedSocket
{
public:
	CLTCPConnectedSocket();
	virtual ~CLTCPConnectedSocket();

private:
	CLTCPConnectedSocket(const CLTCPConnectedSocket&);
	CLTCPConnectedSocket& operator=(const CLTCPConnectedSocket&);
};

#endif