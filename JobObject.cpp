#include "JobObject.h"
#include "ZXParallelFrameDefine.h"
#include "MessageCache.h"
namespace ZXParallelFrame
{
	using namespace std;
	JobObject::JobObject()
	{
		JobId = 0;
		m_messageCode = NULL;
		m_orgMesage = NULL;
	}

	JobObject::JobObject(int jobId)
	{
		JobId = jobId;
		m_messageCode = NULL;
		m_orgMesage = NULL;
	}

	JobObject::~JobObject(void)
	{
		std::list<MessageParam*>::iterator itor = m_reqList.begin();
		for (; itor != m_reqList.end(); ++itor)
		{
			MessageCache::DeleteMessage(*itor);
			
		}
		itor = m_resList.begin();
		for (; itor != m_resList.end(); ++itor)
		{
			MessageCache::DeleteMessage(*itor);

		}
		m_reqList.clear();
		m_resList.clear();
	}

	void JobObject::ResetJobObject()
	{
		std::list<MessageParam*>::iterator itor = m_reqList.begin();
		for (; itor != m_reqList.end(); ++itor)
		{
			MessageCache::DeleteMessage((*itor));

		}
		itor = m_resList.begin();
		for (; itor != m_resList.end(); ++itor)
		{
			MessageCache::DeleteMessage((*itor));

		}
		JobId = 0;
		m_reqList.clear();
		m_resList.clear();
	}

	bool JobObject::PushResMessage(MessageParam* res)
	{
		bool isOK = false;
		m_synObject.acquire();
		m_resList.push_back(res);
		if (m_resList.size() == m_reqList.size())
		{
			isOK = true;
		}
		m_synObject.release();
		return isOK;
	}
	
}


