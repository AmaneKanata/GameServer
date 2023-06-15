#ifdef linux
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#endif

#include <map>

#include <boost/asio.hpp>

#include <ThreadManager.h>

#include "DummyClient_Singleton.h"
#include "LogManager.h"

#include "ClientBase.h"
#include "GameObjectClient.h"

#include "GameSession.h"

template<typename T>
std::shared_ptr<ClientBase> MakeClient(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint& ep, std::string clientId)
{
	

	return client;
}

int main()
{
	std::string localHostIp;
	int tcpPort = 7777;

#ifdef linux
	struct ifaddrs* ifAddrStruct = NULL;
	struct ifaddrs* ifa = NULL;
	void* tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) {
			tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			if (strcmp(ifa->ifa_name, "eth0") == 0)
			{
				localHostIp = addressBuffer;
			}
		}
	}
	if (ifAddrStruct != NULL) freeifaddrs(ifAddrStruct);
#elif _WIN32
	{
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
		boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;
		while (iter != end) {
			boost::asio::ip::tcp::endpoint ep = *iter++;
			if (ep.protocol() == boost::asio::ip::tcp::v4() && ep.address().to_string() != "127.0.0.1") {
				localHostIp = ep.address().to_string();
				break;
			}
		}
	}
#endif

	boost::asio::io_context ioc;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(localHostIp), tcpPort);

	GLogManager = std::make_shared<LogManager>(ioc);

	bool state = true;

	for (int i = 0; i < 10; i++)
	{
		GThreadManager->Launch([&ioc, &state]()
			{
				while (state)
				{
					ioc.run_for(std::chrono::milliseconds{ 50 });
				}
			});
	}

	GLogManager->Log("enter \"help\" for information or enter \"exit\' to exit program");

	while (state)
	{
		string command;
		cin >> command;

		if (command == "exit")
		{
			state = false;
			continue;
		}

		if (command == "help")
		{
			GLogManager->Log("connect <client id> - Connect to server. And automatically send C_ENTER message");
			GLogManager->Log("leave <client id> - Send C_LEAVE message");
			GLogManager->Log("disconnect <client id> - Disconnect session");
			GLogManager->Log("move <client id> - Move game object by 1 in X direction. And send C_SET_TRANSFORM message");
		}

		if (command == "connect")
		{
			string clientId;
			cin >> clientId;

			if (clients.count(clientId))
			{
				GLogManager->Log("Client Id \"", clientId, "\" is duplicated. Try Duplicated Login");
			}

			auto session = make_shared<GameSession>(ioc);

			auto client = make_shared<GameObjectClient>(ioc, clientId, session);

			session->client = client;
			session->Connect(ep);

			client->Init();
			client->Post(&ClientBase::Enter);
			client->DelayPost(5000, &ClientBase::CheckAlive);

			continue;
		}

		if (command == "leave")
		{
			string clientId;
			cin >> clientId;

			auto client = clients.find(clientId);
			if (client == clients.end())
			{
				GLogManager->Log("Wrong Client Id : ", clientId);
				continue;
			}

			client->second->Post(&ClientBase::Leave);

			continue;
		}

		if (command == "disconnect")
		{
			string clientId;
			cin >> clientId;

			auto client = clients.find(clientId);
			if (client == clients.end())
			{
				GLogManager->Log("Wrong Client Id : ", clientId);
				continue;
			}

			client->second->Post(&ClientBase::Disconnect);

			continue;
		}

		else if (command == "move")
		{
			string clientId;
			cin >> clientId;

			auto client = clients.find(clientId);
			if (client == clients.end())
			{
				GLogManager->Log("Wrong Client Id : ", clientId);
				continue;
			}

			shared_ptr<GameObjectClient> gClient = dynamic_pointer_cast<GameObjectClient>(client->second);
			if(gClient != nullptr)
				gClient->Post(&GameObjectClient::Move);

			continue;
		}
	}

	GThreadManager->Join();
}