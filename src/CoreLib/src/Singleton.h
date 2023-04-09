#pragma once

extern class ThreadManager* GThreadManager;
extern class SendBufferManager* GSendBufferManager;
extern class GlobalQueue* GGlobalQueue;
extern class JobTimer* GJobTimer;

#include <memory>
extern std::shared_ptr<class LogManager> GLogManager;