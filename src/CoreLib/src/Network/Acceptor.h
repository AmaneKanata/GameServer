#pragma once

#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;

class Session;
class Service;

using SessionFactory = function<shared_ptr<Session>(io_context& ioc)>;

class Acceptor
{
public:
	Acceptor(io_context& ioc, ip::tcp::endpoint& ep, SessionFactory sessionFactory) 
		: ioc(ioc)
		, ep(ep)
		, sessionFactory(sessionFactory)
	{}
	bool StartAccept();

private:
	void RegisterAccept();

private:
	io_context& ioc;
	ip::tcp::endpoint& ep;
	SessionFactory sessionFactory;
	shared_ptr<ip::tcp::acceptor> acceptor;
};