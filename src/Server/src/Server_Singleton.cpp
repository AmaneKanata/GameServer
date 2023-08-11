#include "Server_Singleton.h"
#include "RoomBase.h"
#include "LogManager.h"

std::shared_ptr<RoomBase> GRoom = nullptr;
std::shared_ptr<LogManager> GLogManager = nullptr;

std::atomic<int> session_num = 0;
std::atomic<int> client_num = 0;

bool CLOSE_ON_EMPTY = true;
bool CLOSE_ON_IDLE = false;
int CHECK_IDEL_INTERVAL = 10000;

int DISCONNECTED_INTERVAL = 10000;
int CHECK_ALIVE_INTERVAL = 10000;

std::shared_ptr<agones::SDK> agones_sdk = nullptr;
std::string agones_state = "";