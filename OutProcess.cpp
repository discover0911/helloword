#include "OutProcess.h"

namespace ZXParallelFrame
{
	OutProcess::OutProcess(int threadNum, int processID)
		:BaseProcess(threadNum, processID)
	{
		
	}

	OutProcess::~OutProcess()
	{
	}

	void  OutProcess::ReceiveOutMessage(MessageParam* message)
	{
		switch (message->MessageType)
		{
			case Response:
			{
				bool isFindReq = false;
				m_mutexInToOut.acquire();
				HandleTimeOutMessage();//处理过期消息
				std::map<ulong, TransmitMessage*>::iterator itor = m_IntoToOutDic.begin();
				itor = m_IntoToOutDic.find(message->Sequnce);
				if (itor != m_IntoToOutDic.end())
				{
					delete itor->second;
					m_IntoToOutDic.erase(itor);
					isFindReq = true;
				}
				m_mutexInToOut.release();
				if (isFindReq)
				{
					PushMessage(message);
				}	
			}
			break;
			case Request:
			{
				message->Sequnce = GetSequnce();
				PushMessage(message);
			}
			break;
			case Notify:
			{
				 PushMessage(message);
			}
			break;
		}
	}

	void OutProcess::SendOutMessage(MessageParam* message)
	{
		switch (message->MessageType)
		{
			case Response:
			{
				//PushMessage(message);
			}
				break;
			case Request:
			{
				message->Sequnce = GetSequnce();
				m_mutexInToOut.acquire();
				TransmitMessage* tranMessage = new TransmitMessage();
				tranMessage->m_reqMessage = message;
				m_messageList.push_back(tranMessage);
				tranMessage->m_listItor = --m_messageList.end();
				m_IntoToOutDic[message->Sequnce] = tranMessage;
				m_mutexInToOut.release();
				//PushMessage(message);
			}
			break;
		}
		//发送数据到网络
	}

	void OutProcess::HandleTimeOutMessage()
	{
		while (true)
		{
			if (m_messageList.size() == 0) break;
			TransmitMessage* transMessage = *(m_messageList.begin());
			if (isTimeOutMessage(transMessage->m_reqMessage))
			{
				//该消息已经过期,并发送过期通知消息
				m_messageList.erase(transMessage->m_listItor);
				std::map<ulong, TransmitMessage*>::iterator  mapItor = m_IntoToOutDic.find(transMessage->m_reqMessage->Sequnce);
				if (mapItor != m_IntoToOutDic.end())
				{
					MessageParam* res = CreateMessage(transMessage->m_reqMessage);
					PushMessage(res);
					m_IntoToOutDic.erase(mapItor);
					
				}
				delete transMessage;
			}
			else
			{
				break;
			}
		}
	}

	bool  OutProcess::isTimeOutMessage(MessageParam* message)
	{
		return false;
	}
}



