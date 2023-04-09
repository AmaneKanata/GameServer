#pragma once

extern thread_local	unsigned int LThreadId;
extern thread_local unsigned long long LEndTickCount;
extern thread_local class JobQueue* LCurrentJobQueue;

#include <memory>
extern thread_local std::shared_ptr<class SendBufferChunk> LSendBufferChunk;
