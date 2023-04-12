#pragma once

#include <vector>

using namespace std;

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int bufferSize);
	~RecvBuffer();

	void Clean();
	bool OnRead(int numOfBytes);
	bool OnWrite(int numOfBytes);

	unsigned char* ReadPos() { return &buffer[readPos]; }
	unsigned char* WritePos() { return &buffer[writePos]; }
	int DataSize() { return writePos - readPos; }
	int FreeSize() { return capacity - writePos; }

private:
	int capacity = 0;
	int bufferSize = 0;
	int readPos = 0;
	int writePos = 0;
	vector<unsigned char> buffer;
};