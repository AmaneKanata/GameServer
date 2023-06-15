#pragma once

#include <memory>
#include <map>
#include <mutex>

extern std::shared_ptr<class LogManager> GLogManager;

extern std::map<std::string, std::shared_ptr<class ClientBase>> clients;
extern std::recursive_mutex clients_mtx;