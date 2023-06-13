#pragma once

#include <boost/asio.hpp>

class Session;
class Service;

using SessionFactory = std::function<std::shared_ptr<Session>(boost::asio::io_context& ioc)>;

class Acceptor
{
public:
	Acceptor(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint& ep, SessionFactory sessionFactory)
		: ioc(ioc)
		, ep(ep)
		, sessionFactory(sessionFactory)
	{}
	bool StartAccept();

private:
	void RegisterAccept();

private:
	boost::asio::io_context& ioc;
	boost::asio::ip::tcp::endpoint& ep;
	SessionFactory sessionFactory;
	std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
};