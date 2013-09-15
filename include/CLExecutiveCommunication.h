#ifndef CLExecutiveCommunication_H
#define CLExecutiveCommunication_H

#include <map>
#include <uuid/uuid.h>
#include "CLStatus.h"

class CLMessageSerializer;
class CLProtocolEncapsulator;
class CLDataPoster;
class CLEvent;
class CLPostResultNotifier;
class CLMessage;

class CLExecutiveCommunication
{
public:
	CLExecutiveCommunication(CLDataPoster *pDataPoster, CLProtocolEncapsulator *pProtocolEncapsulator = 0, CLEvent *pEvent = 0, CLMessageSerializer *pCommonMsgSerializer = 0, bool bMsgDelete = true);
	virtual ~CLExecutiveCommunication();

	CLStatus RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer);
	CLStatus UnregisterSerializer(unsigned long lMsgID);

	CLStatus Initialize(CLPostResultNotifier *pResultNotifier = 0);
	CLStatus Uninitialize();

	CLStatus PostExecutiveMessage(CLMessage *pMsg, CLPostResultNotifier *pResultNotifier = 0);

private:
	CLExecutiveCommunication(const CLExecutiveCommunication&);
	CLExecutiveCommunication& operator=(const CLExecutiveCommunication&);

private:
	CLMessageSerializer *m_pCommonSerializer;
	std::map<unsigned long, CLMessageSerializer*> m_SerializerTable;
	CLProtocolEncapsulator *m_pProtocolEncapsulator;
	CLDataPoster *m_pDataPoster;
	uuid_t m_UuidOfPoster;
	CLEvent *m_pEvent;
	bool m_bMsgDelete;
};

#endif