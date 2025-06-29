#include "RoomManager.h"
#include <stdexcept>
RoomManager::RoomManager()
{
}
RoomManager::~RoomManager()
{
}
void RoomManager::createRoom(const LoggedUser& user, const RoomData& data)
{
    unsigned int id = data.id;
    //creats a room with the given metadata
    std::shared_ptr<Room> room = std::make_shared<Room>(data);
    room->addUser(user);  //adding the given user to the room
    {
        try
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            _rooms.push_back(room);  //adding the room to the rooms vector
            setRoomID(data.id);   //setting the rooms id to be the lates
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Insert threw exception: " << ex.what() << std::endl;
        }
    }
}

void RoomManager::deleteRoom(int id)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<std::shared_ptr<Room>> temp;
    //loop through all the rooms and adds all the rooms excpet the room with the given id
    for (const auto& room : _rooms) 
    {
        if (room->getRoomData().id != id)
        {
            temp.push_back(room);
        }
    }

    _rooms = std::move(temp);  //updating the field to be the temp
}



bool RoomManager::getRoomState(int id) const
{
    std::shared_ptr<Room> room = getRoom(id);  //getting the room by id
    if (room == nullptr)
    {
        throw std::runtime_error("Room not found");
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return room->getRoomData().status;  //gettong the room status
    }
}

std::vector<RoomData> RoomManager::getRooms() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<RoomData> roomList;  //vector of all metadata of rooms
    for (const auto& room : _rooms)  //loop through all rooms and add each metadata
    {
        roomList.push_back(room->getRoomData());
    }
    return roomList;
}

std::shared_ptr<Room> RoomManager::getRoom(int id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& room : _rooms)  //loop through all rooms and returning the room where IDs match
    {
        if (room->getRoomData().id == id)
        {
            return room;
        }
    }
    throw std::runtime_error("Room not found");
}

unsigned int RoomManager::getRoomID() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return this->_roomID;  //returning the roomID field
}
void RoomManager::setRoomID(const unsigned int roomID)
{
    this->_roomID = roomID;  //setting the roomID
}

std::shared_ptr<Room> RoomManager::getRoomByUser(const LoggedUser& user) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& room : _rooms)  //loop through all rooms
    {
        //gettingg all users in the room
        const std::vector<std::string>& users = room->getAllUsers();
        for (const auto& u : users)  //loop through all users in the room
        {
            if (u == user.getUsername())  //is username match
            {
                return room;  //room of user found
            }
        }
    }
    throw std::runtime_error("User not in any room");
}
