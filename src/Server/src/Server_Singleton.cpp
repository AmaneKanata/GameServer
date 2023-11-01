#include "Server_Singleton.h"
#include "RoomBase.h"
#include "LogManager.h"

std::shared_ptr<RoomBase> GRoom = nullptr;
std::shared_ptr<LogManager> GLogManager = nullptr;

std::atomic<int> session_num = 0;
std::atomic<int> client_num = 0;

std::string MODE = std::getenv("MODE") != nullptr ? std::string(std::getenv("MODE")) : "BASE";

bool CLOSE_ON_EMPTY = std::getenv("CLOSE_ON_EMPTY") != nullptr ? bool(std::stoi(std::getenv("CLOSE_ON_EMPTY"))) : false;
bool CLOSE_ON_IDLE = std::getenv("CLOSE_ON_IDLE") != nullptr ? bool(std::stoi(std::getenv("CLOSE_ON_IDLE"))) : false;
int CHECK_IDEL_INTERVAL = std::getenv("CHECK_IDEL_INTERVAL") != nullptr ? std::stoi(std::getenv("CHECK_IDEL_INTERVAL")) : 10000;

int DISCONNECTED_INTERVAL = std::getenv("DISCONNECTED_INTERVAL") != nullptr ? std::stoi(std::getenv("DISCONNECTED_INTERVAL")) : 10000;
int CHECK_ALIVE_INTERVAL = std::getenv("CHECK_ALIVE_INTERVAL") != nullptr ? std::stoi(std::getenv("CHECK_ALIVE_INTERVAL")) : 10000;

bool DELAY_SEND = std::getenv("DELAY_SEND") != nullptr ? bool(std::stoi(std::getenv("DELAY_SEND"))) : false;
int DELAY_SEND_INTERVAL = std::getenv("DELAY_SEND_INTERVAL") != nullptr ? std::stoi(std::getenv("DELAY_SEND_INTERVAL")) : 0;

std::shared_ptr<agones::SDK> agones_sdk = nullptr;
std::string agones_state = "";

#if _WIN32
bool FPS_DRAW = false;
#endif