#include "SendBuffer.h"
#include "CoreLib_Singleton.h"
#include "TLS.h"

using namespace std;

SendBuffer::SendBuffer(shared_ptr<SendBufferChunk> owner, unsigned char* buffer, unsigned int allocSize)
	: owner(owner), buffer(buffer), allocSize(allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

SendBufferChunk::~SendBufferChunk()
{
	cout << "test" << endl;
}

void SendBuffer::Close(unsigned int writeSize)
{
	this->writeSize = writeSize;
	owner->Close(writeSize);
}

void SendBufferChunk::Reset()
{
	open = false;
	usedSize = 0;
}

shared_ptr<SendBuffer> SendBufferChunk::Open(unsigned int allocSize)
{
	if (allocSize > FreeSize())
		abort();

	// 이미 open 된 sendbuffer 를 다시 open 하려고 하면 문제 발생
	if (open)
		abort();

	open = true;
	
	return make_shared<SendBuffer>(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(unsigned int writeSize)
{
	// 이미 close 된 sendbuffer 를 다시 close 하려고 하면 문제 발생
	if (!open)
		abort();

	open = false;
	usedSize += writeSize;
}

shared_ptr<SendBuffer> SendBufferManager::Open(unsigned int size)
{
	if (LSendBufferChunk == nullptr || LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	return LSendBufferChunk->Open(size);
}

shared_ptr<SendBufferChunk> SendBufferManager::Pop()
{
	{
		lock_guard<mutex> lock(mtx);
		if (sendBufferChunks.empty() == false)
		{
			shared_ptr<SendBufferChunk> sendBufferChunk = sendBufferChunks.back();
			sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}

	return shared_ptr<SendBufferChunk>(new SendBufferChunk, [this](SendBufferChunk* buffer) {
		this->PushGlobal(buffer);
		});
}

SendBufferManager::~SendBufferManager()
{
	isDestroy = true;
}

void SendBufferManager::Push(shared_ptr<SendBufferChunk> buffer)
{
	lock_guard<mutex> lock(mtx);
	sendBufferChunks.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	if (!isDestroy)
		GSendBufferManager->Push(shared_ptr<SendBufferChunk>(buffer, [this](SendBufferChunk* buffer) {
		this->PushGlobal(buffer);
			}));
}