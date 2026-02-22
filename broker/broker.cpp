#include "broker.hpp"
#include <iostream>
#include <map>
#include <set>
#include <string>

using std::map, std::string, std::set;
using std::cout, std::endl;


Broker::Broker()
{
  cout << "hello, broker!" << endl;
};

void Broker::subscribe(const string& User, const string& Room)
{
  rooms[Room].insert(User);
}

void Broker::unsubscribe(const string &User, const string& Room)
{
  rooms[Room].erase(User);
}

void Broker::unsubscribe_all(const string& User)
{
  //auto& cuts through verbosity -> map<string, set<string>::value pair& : rooms (pair.first, pair.second). yuck!
  for(auto& [room, users]: rooms)
  {
    users.erase(User);
  }
}

set<string> Broker::fetch_subscribers(const string& Room)
{
  return rooms[Room];
}

set<string> Broker::fetch_rooms(const string& User)
{
  set<string> subscribedRooms;

  for(auto& [room, users] : rooms)
  {
    if(users.contains(User))
     subscribedRooms.insert(room);
  }

  return subscribedRooms;
}
