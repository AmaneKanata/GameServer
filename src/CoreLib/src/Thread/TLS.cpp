#include "TLS.h"

thread_local std::shared_ptr<SendBufferChunk> LSendBufferChunk;

thread_local std::shared_ptr<boost::asio::ip::udp::socket> UDPSocket;