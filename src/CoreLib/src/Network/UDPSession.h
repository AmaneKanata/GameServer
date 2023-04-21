#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace std;

class SendBuffer;

class UDPSession : enable_shared_from_this<UDPSession>
{
public:
	UDPSession(io_context& ioc, ip::udp::endpoint ep);

	virtual void Init() {};
	void StartSend();
	void Write(shared_ptr<SendBuffer> sendBuffer);

private:
	void Send();

	recursive_mutex mtx;
	
	ip::udp::endpoint ep;
	queue<shared_ptr<SendBuffer>> sendQueue;
	boost::asio::steady_timer timer;
	std::chrono::milliseconds interval;
	unsigned short packetSequence;
};