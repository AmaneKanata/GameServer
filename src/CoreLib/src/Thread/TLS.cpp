#include "TLS.h"
#include "../pch.h"

thread_local unsigned int LThreadId = 0;
thread_local unsigned long long LEndTickCount = 0;

thread_local std::shared_ptr<SendBufferChunk> LSendBufferChunk;
thread_local JobQueue* LCurrentJobQueue = nullptr;