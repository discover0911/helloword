#include "JobObjectCache.h"
#include <time.h>
#define RECYCLETIME 300
#include "MemBlock.h"
namespace ZXParallelFrame
{
	Mutex JobObjectCache::m_mutexJobObject;
	MemPool<JobObject>* JobObjectCache::m_JobObjectPool;
	int JobObjectCache::m_newNum = 0;
	int JobObjectCache::m_FreeNum = 0;
	
	JobObjectCache::JobObjectCache()
	{

	}

	JobObjectCache::~JobObjectCache()
	{

	}

	void JobObjectCache::SetPoolNum(int memSize, int blockSize)
	{
		m_JobObjectPool = new MemPool<JobObject>(memSize, blockSize, sizeof(JobObject));
	}

	JobObject* JobObjectCache::NewJobObject()
	{
		m_mutexJobObject.acquire();
		JobObjectCache::m_newNum++;
		//JobObject* jobObject = new JobObject();
		JobObject* jobObject = m_JobObjectPool->NewMemUnit();
		m_mutexJobObject.release();
		return jobObject;
	}

	void JobObjectCache::DeleteJobObject(JobObject* jobObject)
	{
		//对内存进行回收
		m_mutexJobObject.acquire();
		JobObjectCache::m_FreeNum++;

		//delete jobObject;
		jobObject->ResetJobObject();
		m_JobObjectPool->DelMemUnit(jobObject);
		m_mutexJobObject.release();
	}
}
