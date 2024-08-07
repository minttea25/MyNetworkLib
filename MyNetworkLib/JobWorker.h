#pragma once

NAMESPACE_OPEN(NetCore);



ABSTRACT class JobWorker
{
public:
	JobWorker() {}
	virtual ~JobWorker() {}

	PURE_VIRTUAL virtual void AddJobSerializer(AJobSerializerSPtr jobSerializer) = 0;
	PURE_VIRTUAL virtual void DoJobs() = 0;
	PURE_VIRTUAL virtual void ClearJobs() = 0;

	PURE_VIRTUAL virtual ReservableJob& AddReservableJob(const uint64 tickAfter, const JobSPtr job, const AJobSerializerWPtr owner) = 0;
	PURE_VIRTUAL virtual void CheckReservedJob(const uint64 nowTick) = 0;
	PURE_VIRTUAL virtual void ClearReservedJobs() = 0;
private:
	PURE_VIRTUAL virtual AJobSerializerSPtr PopJobSerializer() = 0;
};

NAMESPACE_CLOSE;

