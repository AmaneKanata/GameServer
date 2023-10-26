#pragma once

#include <memory>
#include <agones/sdk.h>

extern std::shared_ptr<class RoomBase> GRoom;
extern std::shared_ptr<class LogManager> GLogManager;

extern std::atomic<int> session_num;
extern std::atomic<int> client_num;

extern std::string MODE;

extern bool CLOSE_ON_EMPTY;
extern bool CLOSE_ON_IDLE;
extern int CHECK_IDEL_INTERVAL;

extern int DISCONNECTED_INTERVAL;
extern int CHECK_ALIVE_INTERVAL;

extern bool DELAY_SEND;
extern int DELAY_SEND_INTERVAL;

extern std::shared_ptr<agones::SDK> agones_sdk;
extern std::string agones_state;

#if _WIN32
extern bool FPS_DRAW;
#endif