#include "BaseProcess.h"
#include "ZXParallelFrameDefine.h"
#include "JobObjectCache.h"
namespace ZXParallelFrame
{
	MessageParam* MessageStrategy(BaseProcess* baseProcess)
	{
		MessageParam* message = NULL;
		baseProcess->m_queueLock.acquire();
		for (int i = (int)High; i <= (int)Low; i++)
		{
			if (baseProcess->m_QueueArrary[i].empty())
			{
				baseProcess->m_QueueArrary[i].clear();
				message = NULL;
			}
			else
			{
				message = baseProcess->m_QueueArrary[i].front();
				baseProcess->m_QueueArrary[i].pop_front();
				break;
			}
		}
		baseProcess->m_queueLock.release();
		return message;
	}

	int BaseProcess::m_processNum = 0;
	BaseProcess** BaseProcess::m_messageProcess ;
	BaseProcess::BaseProcess(int threadNum, int processID)
		:ThreadBase(threadNum)
	{
		ProcessID = processID;
		m_sequence = 0;
		m_QueueArrary = new std::deque<MessageParam*>[(int)Low + 1];
		this->Start();
		m_MessageStrategFunc = MessageStrategy;
	}

	BaseProcess::~BaseProcess(void)
	{
		SAFE_DELETE_V(m_QueueArrary);
		SAFE_DELETE_V(m_messageProcess);
		
	}

	void BaseProcess::SetProcessNum(int processNum)
	{
		m_processNum = processNum;
		m_messageProcess = new BaseProcess*[m_processNum];
	}

	int BaseProcess::AddProcess( BaseProcess* process)
	{
		if (process->ProcessID > m_processNum - 1) return -1;
		BaseProcess::m_messageProcess[process->ProcessID] = process;
		return 0;
	}

	BaseProcess* BaseProcess::GetProcess(int processID)
	{
		return BaseProcess::m_messageProcess[processID];
	}

	void BaseProcess::SendResMessage(MessageParam* resMessage)
	{
		MessageParam* message = resMessage;
		message->MessageType = Response;
		message->Src = ProcessID;
		BaseProcess::m_messageProcess[message->Dst]->PushMessage(message);
	}

	void BaseProcess::SendReqMessage(MessageParam* reqMessage)
	{
		MessageParam* message = reqMessage;
		message->MessageType = Request;
		message->Src = ProcessID;
		message->Sequnce = GetSequnce();
		BaseProcess::m_messageProcess[message->Dst]->PushMessage(message);

	}

	void BaseProcess::SendReqMessage(MessageParam* reqMessage, MessageParam* orgReqMessage)
	{
		MessageParam* message = reqMessage;
		message->MessageType = Request;
		message->Src = ProcessID;
		message->Sequnce = GetSequnce();

		message->ParantMessage = orgReqMessage;
		BaseProcess::m_messageProcess[message->Dst]->PushMessage(message);
	}

	void BaseProcess::SendNfyMessage(MessageParam* nfyMessage)
	{
		MessageParam* message = nfyMessage;
		message->MessageType = Notify;
		message->Src = ProcessID;
		BaseProcess::m_messageProcess[message->Dst]->PushMessage(message);
	}

	void BaseProcess::SendMultiMessage(int messageCode, std::list<MessageParam*>& reqList, MessageParam* orgMesage)
	{
		//发送多个消息
		JobObject* jobObject = m_jobMange.CreateJob();
		jobObject->m_reqList = reqList;
		jobObject->m_orgMesage = orgMesage;
		jobObject->m_messageCode = messageCode;
		std::list<MessageParam*>::iterator itor = jobObject->m_reqList.begin();
		for (; itor != jobObject->m_reqList.end(); ++itor)
		{
			(*itor)->JobId = jobObject->JobId;
			SendReqMessage(*itor);
		}
	}
	void BaseProcess::PushMessage(MessageParam* message, EnumMessageType messageType)
	{
		message->MessageType = messageType;
		PushMessage(message);
	}

	void BaseProcess::PushMessage(MessageParam* message)
	{
		m_queueLock.acquire();
		m_QueueArrary[(int)message->MessageLevel].push_back(message); 	
		m_queueLock.release();
		SendEvent();
	}

	MessageParam* BaseProcess::CreateMessage(MessageParam* message)
	{
		MessageParam* ack = MessageCache::NewMessage();
		ack->Sequnce = message->Sequnce;
		ack->MessageCode = message->MessageCode;
		ack->Src = message->Dst;
		ack->Dst = message->Src;
		ack->MessageLevel = message->MessageLevel;
		if (message == NULL)
		{
			int a = 0;
		}
		ack->ReqMessage = message;
		return ack;
	}

	unsigned long BaseProcess::GetSequnce()
	{
		m_sequenceLock.acquire();
		m_sequence++;
		m_sequenceLock.release();
		return m_sequence;	
	}

	void BaseProcess::Run()
	{
		MessageParam* message = NULL;
		MessageParam* reqMessage = NULL;
		while (true)
		{
			message = MessageStrategy(this);
			if (message == NULL)
			{
				return;
			}
			if (message->MessageType == Response)
			{
				reqMessage = message->ReqMessage;
				if (reqMessage->JobId != NULL)
				{
					JobObject* job = m_jobMange.GetJobObject(reqMessage->JobId);
					if (job != NULL)
					{
						if (job->PushResMessage(message))
						{
							DealWithMultiResponse(job->m_messageCode, job->m_reqList, job->m_resList, job->m_orgMesage);
							JobObjectCache::DeleteJobObject(job);
						}
					}
					break;
				}
			}
			switch (message->MessageType)
			{
			case Request:
				{
					DealWithRequest( message);
				}
				break;
			case Response:
				{
					DealWithResponse( reqMessage,  message);
					MessageCache::DeleteMessage(reqMessage);
					MessageCache::DeleteMessage(message);
				}
				break;
			case Notify:
				{
					DealWithNotify( message);
					MessageCache::DeleteMessage(message);
				}
				break;
			}
		}
	}
}


