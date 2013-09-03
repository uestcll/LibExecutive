#ifndef CLProtocolDataPoster_H
#define CLProtocolDataPoster_H

class CLProtocolDataPoster
{
public:
	CLProtocolDataPoster();
	virtual ~CLProtocolDataPoster();

private:
	CLProtocolDataPoster(const CLProtocolDataPoster&);
	CLProtocolDataPoster& operator=(const CLProtocolDataPoster&);
};

#endif