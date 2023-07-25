#pragma once

#include <httplib.h>

class HttpServer
{
public:
	HttpServer(std::string ip, int port) : ip(ip), port(port)
	{};
	
	void Start();
	void Stop();

private:
	httplib::Server svr;
	std::string ip;
	int port;
};