#pragma once

#include <memory>
#include <agones/sdk.h>

extern std::shared_ptr<class RoomBase> GRoom;
extern std::shared_ptr<class LogManager> GLogManager;

extern std::atomic<int> session_num;
extern std::atomic<int> client_num;

extern bool CLOSE_ON_EMPTY;

extern int DISCONNECTED_INTERVAL;
extern int CHECK_ALIVE_INTERVAL;

extern std::shared_ptr<agones::SDK> agones_sdk;
extern std::string agones_state;