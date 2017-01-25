#include "MessageCache.h"
#include <time.h>
#define RECYCLETIME 300
#include "MemBlock.h"
namespace ZXParallelFrame
{
	Mutex MessageCache::m_mutexMessage;
	MemPool<MessageParam>* MessageCache::m_MessagePool;
	int MessageCache::m_newNum = 0;
	int MessageCache::m_FreeNum = 0;
	
	MessageCache::MessageCache()
	{

	}


	MessageCache::~MessageCache()
	{

	}

	void MessageCache::SetPoolNum(int memSize, int blockSize)
	{
		m_MessagePool = new MemPool<MessageParam>(memSize, blockSize, sizeof(MessageParam));
	}
	MessageParam* MessageCache::NewInnerMessage()
	{
		m_mutexMessage.acquire();
		MessageCache::m_newNum++;
		MessageParam* message = m_MessagePool->NewMemUnit();
		//MessageParam* message = new MessageParam;
		m_mutexMessage.release();
		return message;
	}

	MessageParam* MessageCache::NewMessage()
	{
		MessageParam* message = NewInnerMessage();
		return message;
	}

	MessageParam* MessageCache::NewMessage(int messageCode)
	{
		MessageParam* message = NewInnerMessage();
		message->MessageCode = messageCode;
		return message;
	}
	MessageParam* MessageCache::NewMessage(int dstID, int messageCode)
	{
		MessageParam* message = NewInnerMessage();
		message->Dst = dstID;
		message->MessageCode = messageCode;
		return message;
	}

	MessageParam* MessageCache::NewMessage(int dstID, int messageCode, EnumMessageLevel messageLevel)
	{
		MessageParam* message = NewInnerMessage();
		message->Dst = dstID;
		message->MessageCode = messageCode;
		message->MessageLevel = messageLevel;
		return message;
	}

	void MessageCache::DeleteMessage(MessageParam* message)
	{
		//对内存进行回收
		m_mutexMessage.acquire();
		MessageCache::m_FreeNum++;
		message->ResetMessage();
		//delete message;
		m_MessagePool->DelMemUnit(message);
		m_mutexMessage.release();
	}
}
