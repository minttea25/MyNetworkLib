#pragma once
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
		// For releasing shared pointer of job when returning to pool
		//_job = nullptr;
		DESTRUCTOR(JobData);
	}

private:
	JobSPtr _job;
	AJobSerializerWPtr _owner;

	friend class ReservableJob;
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

	/*~ReservableJob()
	{
		if(_jobData) ObjectPool<JobData>::Release(_jobData);
		DESTRUCTOR(ReservableJob);
	}*/

	/// <summary>
	/// Call it when the job is executable. 
	/// </summary>
	void PushJobToSerializer()
	{
		ASSERT_CRASH(_jobData != nullptr);

		if (_canceled == false)
		{
			if (auto owner = _jobData->_owner.lock())
			{
				owner->Push(_jobData->_job);
			}
		}

		ObjectPool<JobData>::Release(_jobData);
	}

	/// <summary>
	/// Get executable tick value
	/// </summary>
	/// <returns>_executionTick</returns>
	uint64 GetExecutionTick() const { return _executionTick; }

	/// <summary>
	/// Cancel this job. It will be not added in job serializer.
	/// </summary>
	void Cancel() { _canceled = true; }

	
	bool operator<(const ReservableJob& other) const
	{
		return _executionTick > other._executionTick;
	}

private:
	JobData* _jobData;
	uint64 _executionTick;
	bool _canceled;

};

NAMESPACE_CLOSE;

