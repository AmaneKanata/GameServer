#pragma once

#include <vector>

using namespace std;

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int bufferSize);
	~RecvBuffer();

	void			Clean();
	bool			OnRead(int numOfBytes);
	bool			OnWrite(int numOfBytes);

	unsigned char* ReadPos() { return &_buffer[_readPos]; }
	unsigned char* WritePos() { return &_buffer[_writePos]; }
	int			DataSize() { return _writePos - _readPos; }
	int			FreeSize() { return _capacity - _writePos; }

private:
	int			_capacity = 0;
	int			_bufferSize = 0;
	int			_readPos = 0;
	int			_writePos = 0;
	vector<unsigned char>	_buffer;
};