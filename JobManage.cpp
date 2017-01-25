#include "JobManage.h"
#include "ZXParallelFrameDefine.h"
#include "JobObjectCache.h "
namespace ZXParallelFrame
{
	JobManage* JobManage::m_jobManage = NULL;
	Mutex JobManage::m_synObject;
	JobManage::JobManage()
	{
		m_jobId = 0;
	}

	JobManage::~JobManage()
	{
	}

	JobManage* JobManage::CreateSingleInstance()
	{
		if (m_jobManage == NULL)
		{
			m_jobManage = new JobManage();
		}
		return JobManage::m_jobManage;
	}

	JobObject* JobManage::CreateJob()
	{
	    m_synObject.acquire();
		m_jobId++;
		JobObject* job = JobObjectCache::NewJobObject();
		job->JobId = m_jobId;
		m_jobDic[job->JobId] = job;
		m_synObject.release();
		return job;
	}

	JobObject* JobManage::GetJobObject(int jobId)
	{
		JobObject* job = NULL;
		m_synObject.acquire();
		std::map<int, JobObject*>::iterator itor = m_jobDic.begin();
		itor = m_jobDic.find(jobId);
		if (itor != m_jobDic.end())
		{
			job = itor->second;
		}
		m_synObject.release();
		return job;
	}

	bool JobManage::DeleteJob(int jobId)
	{
		bool isDelete = false;
		m_synObject.acquire();
		std::map<int, JobObject*>::iterator itor = m_jobDic.begin();
		itor = m_jobDic.find(jobId);
		if (itor != m_jobDic.end())
		{
			SAFE_DELETE(itor->second);
			m_jobDic.erase(itor);
			isDelete = true;
		}
		m_synObject.release();
		return isDelete;
	}
}


