#include "UDPSession.h"
#include "SendBuffer.h"

#include "../pch.h"

#define MAX_PACKET_SEQ 10000
#define MAX_PACKET_SIZE 1000 //MTU 보다 작을 것

UDPSession::UDPSession(io_context& ioc, ip::udp::endpoint ep)
	: ep(ep)
	, timer(ioc)
	, interval(std::chrono::milliseconds(100))
    , packetSequence(0)
{}

void UDPSession::Write(shared_ptr<SendBuffer> sendBuffer)
{
    lock_guard<recursive_mutex> lock(mtx);
    sendQueue.push(sendBuffer);
}

void UDPSession::StartSend()
{
	timer.expires_after(interval);
	timer.async_wait([this](const boost::system::error_code&) { Send(); });
}

void UDPSession::Send()
{
    queue<std::shared_ptr<SendBuffer>> tempSendQueue;

    {
        lock_guard<recursive_mutex> lock(mtx);
        std::swap(tempSendQueue, sendQueue);
    }

    while (!tempSendQueue.empty())
    {
        int currentPacketSize = sizeof(packetSequence);
        
        vector<boost::asio::const_buffer> sendBuffers;
        sendBuffers.emplace_back(&packetSequence, sizeof(packetSequence));

        packetSequence = (packetSequence + 1) % MAX_PACKET_SEQ;

        std::shared_ptr<vector<std::shared_ptr<SendBuffer>>> tempSendBufferRefs = std::make_shared<vector<std::shared_ptr<SendBuffer>>>();

        //단일 SendBuffer 로 MAX_PACKET_SIZE 를 초과하는 경우를 고려해야 함. 만약 그런 패킷이 존재한다면 이후로 통신이 수행되지 않음
        while (!tempSendQueue.empty() && currentPacketSize + tempSendQueue.front()->WriteSize() < MAX_PACKET_SIZE)
        {
            currentPacketSize += tempSendQueue.front()->WriteSize();
            sendBuffers.emplace_back(tempSendQueue.front()->Buffer(), tempSendQueue.front()->WriteSize());
            tempSendBufferRefs->emplace_back(tempSendQueue.front());
            tempSendQueue.pop();
        }

        UDPSocket->async_send_to(sendBuffers, ep, [this, tempSendBufferRefs](const boost::system::error_code& error, std::size_t bytes_transferred) {
            tempSendBufferRefs->clear();
            });
    }

    StartSend();
}
