#pragma once

#include <map>
#include <string>

enum class CLIENT_LEAVE
{
	LEAVED,
	DUPLICATED,
	DISCONNECTED,
	HEARTBEAT_FAIL,
	ROOM_CLOSED,
};

static std::map <CLIENT_LEAVE, std::string> CLIENT_LEAVE_STRINGS =
{
	{CLIENT_LEAVE::LEAVED, "LEAVED"},
	{CLIENT_LEAVE::DUPLICATED, "DUPLICATED"},
	{CLIENT_LEAVE::DISCONNECTED, "DISCONNECTED"},
	{CLIENT_LEAVE::HEARTBEAT_FAIL, "HEARTBEAT_FAIL"},
	{CLIENT_LEAVE::ROOM_CLOSED, "ROOM_CLOSED"},
};

std::string CLIENT_LEAVE_ToString(CLIENT_LEAVE value)
{
	auto it = CLIENT_LEAVE_STRINGS.find(value);
	if (it != CLIENT_LEAVE_STRINGS.end()) {
		return it->second;
	}

	return "Unknown";
}