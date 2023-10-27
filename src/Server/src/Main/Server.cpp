#include <boost/asio.hpp>
#include <Acceptor.h>
#include <CoreLib_Singleton.h>
#include <ThreadManager.h>

#include <agones/sdk.h>
#include <grpc++/grpc++.h>

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
#include "FPSRoom.h"

#include "backward.hpp"
backward::SignalHandling sh;

int main()
{
#ifdef linux
	const std::string log_path = "/mnt/coredump/";
	auto epoch_time = std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
	std::string pod_name = std::getenv("HOSTNAME");
	std::string log_filename = log_path + std::to_string(epoch_time) + "_" + pod_name + ".log";
	freopen(log_filename.c_str(), "a", stderr);
#endif
	
	std::string localHostIp;
	int socketPort = 7777;
	int httpPort = 7778;

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

	agones_sdk = std::make_shared<agones::SDK>();
	if (!agones_sdk->Connect())
		return 0;

	GThreadManager->Launch([]()
		{
			while (agones_state != "Shutdown") {
				bool ok = agones_sdk->Health();
				std::this_thread::sleep_for(std::chrono::seconds(2));
			}
		});

	GThreadManager->Launch([]()
		{
			agones_sdk->WatchGameServer([](const agones::dev::sdk::GameServer& gameserver) {
				agones_state = gameserver.status().state();
				});
		});

	agones_sdk->Ready();

	while (agones_state != "Allocated")
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
	}

	agones::dev::sdk::GameServer gameServer;
	if (agones_sdk->GameServer(&gameServer).ok())
	{
		auto labels = gameServer.object_meta().labels();
		for (const auto& label : labels)
			std::cout << "Label: " << label.first << " = " << label.second << std::endl;
	}

	boost::asio::io_context ioc;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(localHostIp), socketPort);

	GLogManager = std::make_shared<LogManager>(ioc);

	if (MODE == "BASE")
		GRoom = std::make_shared<RoomBase>(ioc);
	else if(MODE == "FPS")
		GRoom = std::make_shared<FPSRoom>(ioc);
	else
	{
		GLogManager->Log("Invalid Mode");
		return 0;
	}

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
				while (agones_state != "Shutdown")
				{
					ioc.run_for(std::chrono::milliseconds{1000});
				}
			});
	}

	HttpServer httpServer(localHostIp, httpPort);

	GThreadManager->Launch([&httpServer]()
		{
			httpServer.Start();
		});

	GThreadManager->Launch([&httpServer]()
		{
			while (agones_state != "Shutdown")
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

	std::cout << "Game Server Has Exited" << std::endl;
}