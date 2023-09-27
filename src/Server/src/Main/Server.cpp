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
#elif _WIN32
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

#if _WIN32
double lastX = 320, lastY = 240;
double yaw = -90.0f, pitch = 0.0f;
float cameraSpeed = 0.2f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
#endif

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

	boost::asio::io_context ioc;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(localHostIp), socketPort);

	GLogManager = std::make_shared<LogManager>(ioc);

	GRoom = std::make_shared<FPSRoom>(ioc);
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

#if _WIN32
	GThreadManager->Launch([]()
		{
			if (!glfwInit()) {
				return -1;
			}

			GLFWwindow* window = glfwCreateWindow(1280, 720, "Bullet Debug Draw", NULL, NULL);
			if (!window) {
				glfwTerminate();
				return -1;
			}

			glfwMakeContextCurrent(window);
			
			glEnable(GL_DEPTH_TEST);

			glViewport(0, 0, 1280, 720);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(40.0, 1280.0 / 720.0, 0.1, 1000.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(0, 2, -2, 0, 0, 10, 0, 1, 0);

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
				double xoffset = xpos - lastX;
				double yoffset = lastY - ypos;
				lastX = xpos;
				lastY = ypos;

				float sensitivity = 0.1f;
				xoffset *= sensitivity;
				yoffset *= sensitivity;

				yaw += xoffset;
				pitch += yoffset;

				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;

				glm::vec3 front;
				front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				front.y = sin(glm::radians(pitch));
				front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
				cameraFront = glm::normalize(front);
				});

			auto& room = std::static_pointer_cast<FPSRoom>(GRoom);

			while (agones_state != "Shutdown")
			{
				if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
					cameraPos += cameraSpeed * cameraFront;
				if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
					cameraPos -= cameraSpeed * cameraFront;
				if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
					cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
					cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

				if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
					cameraPos.y += cameraSpeed;
				if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
					cameraPos.y -= cameraSpeed;

				glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glLoadIdentity();
				glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
				glLoadMatrixf(glm::value_ptr(view));

				room->dynamicsWorld->debugDrawWorld();

				glfwSwapBuffers(window);
				glfwPollEvents();

				std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 / 60 });
			}

			glfwDestroyWindow(window);
			glfwTerminate();
		});
#endif

	GThreadManager->Join();

	//Close Server

	acceptor->Stop();

	ioc.run(); //handle remain jobs

	GLogManager = nullptr;
	GRoom = nullptr;

	std::cout << "Game Server Has Exited" << std::endl;
}