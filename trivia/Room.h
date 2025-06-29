#pragma once
#include <mutex>
#include <vector>
#include <string>
#include "RoomData.h"
#include "LoggedUser.h"

class Room
{
public:
    // constructor that initializes room metadata
    Room(const RoomData& metadata);

    Room();// default constructor

    // adds a user to the room
    void addUser(const LoggedUser& user);

    // removes a user from the room
    void removeUser(const LoggedUser& user);

    // returns the list of usernames in the room
    std::vector<std::string> getAllUsers() const;

    // returns the room's metadata
    const RoomData& getRoomData() const;

private:
    RoomData m_metadata;              // metadata of the room
    std::vector<LoggedUser> m_users;   // list of users in the room
    mutable std::mutex m_mutex;
};
