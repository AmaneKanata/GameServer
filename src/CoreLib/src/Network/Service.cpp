#include "Service.h"

#include <chrono>

using namespace std;

void Service::Start()
{
	acceptor = make_shared<Acceptor>();
	acceptor->StartAccept(shared_from_this());
}

shared_ptr<Session> Service::CreateSession()
{
	return sessionFactory(ioc);
}

bool Service::Dispatch(unsigned int timeoutMs)
{
	ioc.run_for(std::chrono::milliseconds{timeoutMs});
	return true;
}
