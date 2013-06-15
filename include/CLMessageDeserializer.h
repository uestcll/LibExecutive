#ifndef CLMessageDeserializer_H
#define CLMessageDeserializer_H

class CLMessage;

class CLMessageDeserializer
{
public:
	CLMessageDeserializer();
	virtual ~CLMessageDeserializer();

	virtual CLMessage *Deserialize(char *pBuffer) = 0;

private:
	CLMessageDeserializer(const CLMessageDeserializer&);
	CLMessageDeserializer& operator=(const CLMessageDeserializer&);
};

#endif