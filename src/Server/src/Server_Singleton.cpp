#include "Server_Singleton.h"
#include "RoomBase.h"
#include "LogManager.h"

std::shared_ptr<RoomBase> GRoom = nullptr;
std::shared_ptr<LogManager> GLogManager = nullptr;

std::atomic<int> session_num = 0;
std::atomic<int> client_num = 0;