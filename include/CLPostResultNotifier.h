#ifndef CLPostResultNotifier_H
#define CLPostResultNotifier_H

class CLPostResultNotifier
{
public:
	CLPostResultNotifier();
	virtual ~CLPostResultNotifier();

private:
	CLPostResultNotifier(const CLPostResultNotifier&);
	CLPostResultNotifier& operator=(const CLPostResultNotifier&);
};

#endif