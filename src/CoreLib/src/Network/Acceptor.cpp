#include <boost/asio.hpp>
#include <iostream>

#include "Acceptor.h"
#include "Session.h"

using namespace std;
using namespace boost::asio;

bool Acceptor::StartAccept()
{
	acceptor = make_shared<ip::tcp::acceptor>(ioc, ep.protocol());
	acceptor->bind(ep);
	acceptor->listen();

	RegisterAccept();

	return true;
}

void Acceptor::RegisterAccept()
{
	auto session = sessionFactory(ioc);
	acceptor->async_accept(*session->GetSocket(), [this, session](const boost::system::error_code& error) {
		if (error)
			return;

		session->ProcessConnect();

		this->RegisterAccept();
		});
}