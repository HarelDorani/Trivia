#include "Room.h"

// constructor that initializes the room metadata
Room::Room(const RoomData& metadata)
    : m_metadata(metadata)
{
}

Room::Room()
{
}

// adds a user to the room
void Room::addUser(const LoggedUser& user)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    //checking if the users vector is the size of the max players num
    if (this->m_users.size() == this->m_metadata.maxPlayers)
    {
        throw std::runtime_error("Room is full! cant join room");
    }
    else
    {
        m_users.push_back(user);
    }
}

// removes a user from the room
void Room::removeUser(const LoggedUser& user)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    //moving the user with the same username to the end of the vector
    auto it = std::remove_if(m_users.begin(), m_users.end(),
        [&user](const LoggedUser& u) { return u.getUsername() == user.getUsername(); });

    if (it != m_users.end())
    {
        //removing the user that in the end of the vector
        m_users.erase(it, m_users.end());
    }
}

// returns a list of all usernames in the room
std::vector<std::string> Room::getAllUsers() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> usernames;
    for (const auto& user : m_users) //looping through all users
    {
        usernames.push_back(user.getUsername());  //adding the username
    }
    return usernames;
}

// returns the room metadata
const RoomData& Room::getRoomData() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_metadata;
}

