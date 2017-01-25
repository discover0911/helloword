
#include "MessageParam.h"
namespace ZXParallelFrame
{
	static int index = 0;
	MessageParam::MessageParam()
	{
		MessageType = InValid;
		MessageLevel = Normal;
		ParantMessage = NULL;
		ReqMessage = NULL;
		Coroutine = NULL;
		JobId = 0;
	}
	MessageParam::MessageParam(int messageCode)
	{
		MessageType = InValid;
		ParantMessage = NULL;
		ReqMessage = NULL;
		Coroutine = NULL;
		MessageLevel = Normal;
		MessageCode = messageCode;
		JobId = 0;
	}
	MessageParam::MessageParam(int dstID, int messageCode)
	{
		MessageType = InValid;
		ParantMessage = NULL;
		ReqMessage = NULL;
		Coroutine = NULL;
		Dst = dstID;
		MessageCode = messageCode;
		MessageLevel = Normal;
		JobId = 0;
	}
	MessageParam::MessageParam(int dstID, int messageCode, EnumMessageLevel messageLevel)
	{
		MessageType = InValid;
		ParantMessage = NULL;
		ReqMessage = NULL;
		JobId = 0;
		Coroutine = NULL;
		Dst = dstID;
		MessageCode = messageCode;
		MessageLevel = messageLevel;
	}

	void MessageParam::ResetMessage()
	{
		Sequnce = 0;
		Src = 0;
		Dst = 0;
		MessageCode = 0;
		MessageType = InValid;
		MessageLevel = Normal;
		ParantMessage = NULL;
		ReqMessage = NULL;
		JobId = 0;
		Coroutine = NULL;
		MapString.clear();
		MapInt.clear();;
		MapFloat.clear();
		MapDouble.clear();
	}
	
	MessageParam::~MessageParam()
	{
	}
}

