#include <boost/asio.hpp>
#include <iostream>

#include "Acceptor.h"
#include "Service.h"

using namespace std;
using namespace boost::asio;

bool Acceptor::StartAccept(shared_ptr<Service> owner)
{
	this->owner = owner;

	acceptor = make_shared<ip::tcp::acceptor>(owner->GetIOC(), owner->GetEndPoint().protocol());
	acceptor->bind(owner->GetEndPoint());
	acceptor->listen();

	RegisterAccept();

	return true;
}

void Acceptor::RegisterAccept()
{
	auto session = owner->CreateSession();
	acceptor->async_accept(*session->GetSocket(), [this, session](const boost::system::error_code& error) {
		if (error)
			return;

		session->ProcessConnect();

		this->RegisterAccept();
		});
}