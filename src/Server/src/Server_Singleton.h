#pragma once

#include <memory>

extern std::shared_ptr<class RoomBase> GRoom;
extern std::shared_ptr<class LogManager> GLogManager;

extern std::atomic<int> session_num;
extern std::atomic<int> client_num;

const bool CLOSE_ON_EMPTY = true;
const int DISCONNECTED_WAIT_TIME = 10000;