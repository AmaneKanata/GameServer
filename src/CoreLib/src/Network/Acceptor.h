#pragma once

#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;

class Service;

class Acceptor
{
public:
	bool StartAccept(shared_ptr<Service> owner);

private:
	void RegisterAccept();

private:
	shared_ptr<Service> owner;
	shared_ptr<ip::tcp::acceptor> acceptor;
};