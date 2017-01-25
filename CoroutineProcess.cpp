#include "CoroutineProcess.h"
#include "pcl.h"
namespace ZXParallelFrame
{
	#define CO_STACK_SIZE (64*1024)
	CoroutineProcess* coroutineProcess = NULL;
	coroutine_t corotest;
	void CoroutineFuc(void *data)
	{
		coroutineProcess->DealCoroutineRequest((MessageParam*)data);
	}

	CoroutineProcess::CoroutineProcess(int threadNum, int processID) :BaseProcess(threadNum, processID)
	{
		m_stackSize = CO_STACK_SIZE;
		coroutineProcess = this;
	}


	CoroutineProcess::~CoroutineProcess()
	{
	}
	
	int CoroutineProcess::DealWithRequest(MessageParam* req)
	{
		if (FAILURE == DealWithMessageRequest(req))
		{
			coroutine_t coro;
			coro = co_create(CoroutineFuc, req, NULL,
				m_stackSize);
			co_call(coro);
		}
		
		return 0;
	}

	int CoroutineProcess::DealWithResponse(MessageParam* req, MessageParam* res)
	{
		coroutine_t coro = res->ReqMessage->Coroutine;
		if (coro != NULL)
		{
			co_set_data(coro, res);
			co_call(coro);
		}
		else
		{
			DealWithMessageResponse(req, res);
		}
		return 0;
	}

	int CoroutineProcess::SendMessageAndSwitch(MessageParam* req)
	{
		req->Coroutine = co_current();
		SendReqMessage(req);
		co_resume();
		return 0;
	}

	void CoroutineProcess::SendMultiMessageAndSwitch(int messageCode, std::list<MessageParam*>& reqList, MessageParam* orgMesage)
	{
		(*(reqList.begin()))->Coroutine = co_current();
		SendMultiMessage(messageCode, reqList, orgMesage);
		co_resume();
	}

	int CoroutineProcess::DealWithMultiResponse(int messageCode, std::list<MessageParam*>& reqList, std::list<MessageParam*>& resList, MessageParam* orgMesage)
	{
		coroutine_t coro = (*(reqList.begin()))->Coroutine;
		if (coro == NULL)
		{
			DealWithMultiMessageResponse(messageCode, reqList, resList, orgMesage);
		}
		else
		{
			co_set_data(coro, &resList);
			co_call(coro);
		}
		return 0;
	}

	void* CoroutineProcess::GetResponseData()
	{
		return co_get_data(co_current());
	}

	void CoroutineProcess::SetStackSize(int stackSize)
	{
		m_stackSize = stackSize;
	}

	int CoroutineProcess::GetStackSize()
	{
		return  m_stackSize;
	}

	
}


