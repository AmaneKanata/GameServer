#pragma once

extern class ThreadManager* GThreadManager;
extern class SendBufferManager* GSendBufferManager;
extern class JobTimer* GJobTimer;

#include <memory>

template<typename T>
class LockQueue;
class JobQueue;
extern LockQueue<std::shared_ptr<JobQueue>>* GPendingJobQueues;

extern std::shared_ptr<class LogManager> GLogManager;