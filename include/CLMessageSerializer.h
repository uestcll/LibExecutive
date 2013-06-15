#ifndef CLMessageSerializer_H
#define CLMessageSerializer_H

class CLMessage;

class CLMessageSerializer
{
public:
	CLMessageSerializer();
	virtual ~CLMessageSerializer();

	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength) = 0;

private:
	CLMessageSerializer(const CLMessageSerializer&);
	CLMessageSerializer& operator=(const CLMessageSerializer&);
};

#endif