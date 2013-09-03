#ifndef CLMessagePoster_H
#define CLMessagePoster_H

class CLMessagePoster
{
public:
	CLMessagePoster();
	virtual ~CLMessagePoster();

private:
	CLMessagePoster(const CLMessagePoster&);
	CLMessagePoster& operator=(const CLMessagePoster&);
};

#endif