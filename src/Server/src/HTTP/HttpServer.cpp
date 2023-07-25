#include "HttpServer.h"	

void HttpServer::Start()
{
	svr.Get("/test", [](const httplib::Request& req, httplib::Response& res) {
		res.set_content("Hello World!", "text/plain");
		});

	svr.listen(ip.c_str(), port);
}

void HttpServer::Stop()
{
	svr.stop();
}
