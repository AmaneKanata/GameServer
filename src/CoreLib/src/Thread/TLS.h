#pragma once

#include <memory>
#include <boost/asio.hpp>

extern thread_local std::shared_ptr<class SendBufferChunk> LSendBufferChunk;
//extern thread_local std::shared_ptr<class boost::asio::ip::udp::socket> UDPSocket;
