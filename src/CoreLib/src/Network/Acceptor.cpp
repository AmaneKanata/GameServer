#include <boost/asio.hpp>
#include <iostream>

#include "Acceptor.h"
#include "Service.h"

using namespace std;
using namespace boost::asio;

bool Acceptor::StartAccept(shared_ptr<Service> owner)
{
	_owner = owner;

	acceptor = make_shared<ip::tcp::acceptor>(_owner->GetIOC(), _owner->GetEndPoint().protocol());
	acceptor->bind(_owner->GetEndPoint());
	acceptor->listen();

	RegisterAccept();

	return true;
}

void Acceptor::RegisterAccept()
{
	auto session = _owner->CreateSession();
	acceptor->async_accept(*session->GetSocket(), [this, session](const boost::system::error_code& error) {
		if (error)
			return;

		session->ProcessConnect();

		this->RegisterAccept();
		});
}