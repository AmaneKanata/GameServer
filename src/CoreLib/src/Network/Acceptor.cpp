#include <boost/asio.hpp>

#include "Acceptor.h"
#include "Session.h"

bool Acceptor::StartAccept()
{
	acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(ioc, ep.protocol());
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

void Acceptor::Stop()
{
	acceptor->close();
}