#pragma once

#include "Job.h"
#include "JobSerializer.h"

NAMESPACE_OPEN(NetCore);

struct JobData
{
public:
	JobData(const JobSPtr _job, const AJobSerializerWPtr _owner)
		: _job(_job), _owner(_owner)
	{
	}

	~JobData()
	{
	}

private:
	JobSPtr _job;
	AJobSerializerWPtr _owner;

	friend struct ReservableJob;
};

/// <summary>
/// ReservableJob struct is cancelable and reservable job struct.
/// </summary>
struct ReservableJob
{
public:
	ReservableJob(const uint64 _executionTick, const JobSPtr _job, const AJobSerializerWPtr _owner)
		: _jobData(ObjectPool<JobData>::Acquire(_job, _owner)), _executionTick(_executionTick), _canceled(false)
	{
	}

	~ReservableJob()
	{
	}

	/// <summary>
	/// Call it when the job is executable. 
	/// </summary>
	void PushJobToSerializer();

	/// <summary>
	/// Get executable tick value
	/// </summary>
	/// <returns>_executionTick</returns>
	uint64 GetExecutionTick() const { return _executionTick; }

	/// <summary>
	/// Cancel this job. It will be not added in job serializer.
	/// </summary>
	void Cancel() { _canceled = true; }

	/// <summary>
	/// Return if this job is canceled
	/// </summary>
	/// <returns>true if this job is canceled, false otherwise</returns>
	bool IsCanceled() const { return _canceled; }
	
	bool operator<(const ReservableJob& other) const
	{
		return _executionTick > other._executionTick;
	}

private:
	JobData* _jobData;
	uint64 _executionTick;
	bool _canceled;

	friend struct ReservableJobCompare;
};

/// <summary>
/// Reservable Job Comparator for priority queue.
/// </summary>
struct ReservableJobCompare
{
	bool operator()(const ReservableJobSPtr& lhs, const ReservableJobSPtr& rhs) const
	{
		return lhs->_executionTick > rhs->_executionTick;
	}
};

NAMESPACE_CLOSE;

