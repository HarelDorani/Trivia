#pragma once
#include <mutex>
#include <memory>
#include <vector>
#include <vector>
#include <unordered_map>
#include <optional>
#include <functional>
#include "Room.h"
#include "LoggedUser.h"
#include "RoomData.h"


// class responsible for managing all active rooms
class RoomManager
{
public:
    //constractor
    RoomManager();
    ~RoomManager();
    /*
    this function creates a new room
    input: user, room data
    output: none
    */
    void createRoom(const LoggedUser& user, const RoomData& data);

    //this function deletes room by the id
    void deleteRoom(int id);

    /*
    this function is a getter for the room state
    input: the room id
    output: bool - has game begun or not
    */
    bool getRoomState(int id) const;

    /*
    this function is a getter for all the metadata of all the rooms
    input: none
    output: vector of metadata
    */
    std::vector<RoomData> getRooms() const;

    /*
    this function is a getter to a room by the id
    input: the room's id to get
    output: pointer to the room
    */
    std::shared_ptr<Room> getRoom(int id) const;
    //this function is a getter to the roomID field
    unsigned int getRoomID() const;
    //this function is a setter to the room id field
    void setRoomID(const unsigned int roomID);
    /*
    this function is a getter to a room by a user
    input: the user to find in a room
    output: pointer to a room
    */
    std::shared_ptr<Room> getRoomByUser(const LoggedUser& user) const;
private:
    unsigned int _roomID;
    std::vector<std::shared_ptr<Room>> _rooms;  //vector of all rooms
    mutable std::mutex m_mutex;
};
