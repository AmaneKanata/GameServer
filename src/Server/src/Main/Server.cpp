#include <boost/asio.hpp>
#include <Acceptor.h>
#include <CoreLib_Singleton.h>
#include <ThreadManager.h>

#ifdef linux
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#endif

#include "Server_Singleton.h"

#include "LogManager.h"

#include "HttpServer.h"

#include "GameSession.h"
#include "RoomBase.h"
#include "GameObjectRoom.h"

int main()
{
	std::string localHostIp;
	int port = 7777;

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

	if (localHostIp.empty())
	{
		GLogManager->Log("No Valid IP Address");
		return 0;
	}

	boost::asio::io_context ioc;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(localHostIp), port);

	GLogManager = std::make_shared<LogManager>(ioc);

	GRoom = std::make_shared<GameObjectRoom>(ioc);
	GRoom->Init();

	auto acceptor = std::make_shared<Acceptor>(ioc, ep,
		[](boost::asio::io_context& ioc) {
			return std::make_shared<GameSession>(ioc);
		}
	);
	acceptor->StartAccept();

	GLogManager->Log("Game Server Started with IP ", localHostIp);

	for (int i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&ioc]()
			{
				while (GRoom->GetState() != HandlerState::Closed)
				{
					ioc.run_for(std::chrono::milliseconds{1000});
				}
			});
	}

	HttpServer httpServer(localHostIp, 8080);

	GThreadManager->Launch([&httpServer]()
		{
			httpServer.Start();
		});

	GThreadManager->Launch([&httpServer]()
		{
			while (GRoom->GetState() != HandlerState::Closed)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
			}
			httpServer.Stop();
		});

	GThreadManager->Join();
	
	//Close Server

	acceptor->Stop();

	ioc.run(); //handle remain jobs

	GLogManager = nullptr;
	GRoom = nullptr;
}