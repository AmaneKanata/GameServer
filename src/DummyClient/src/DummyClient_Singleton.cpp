#include "DummyClient_Singleton.h"

#include <map>
#include <ClientBase.h>

std::shared_ptr<class LogManager> GLogManager;

std::map<std::string, std::shared_ptr<ClientBase>> clients;
std::recursive_mutex clients_mtx;