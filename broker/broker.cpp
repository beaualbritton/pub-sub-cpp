#include <iostream>
#include <map>
#include <set>
#include <string>

using std::map, std::string, std::set;
using std::cout, std::endl;

class Broker
{
  public:

  Broker()
  {
    cout << "hello, broker!" << endl;
  };

  void subscribe(const string& User, const string& Room)
  {
    if(!rooms.contains(Room))
      rooms[Room] = set<string>();

    rooms[Room].insert(User);

  }

  void unsubscribe(const string &User, const string& Room)
  {
    rooms[Room].erase(User);
  }

  void unsubscribe_all(const string& User)
  {
    //auto& cuts through verbosity -> map<string, set<string>::value pair& : rooms (pair.first, pair.second). yuck!
    for(auto& [room, users]: rooms)
    {
      users.erase(User);
    }
  }

  set<string> fetch_subscribers(const string& Room)
  {
    return rooms[Room];
  }

  set<string> fetch_rooms(const string& User)
  {
    set<string> subscribedRooms;

    for(auto& [room, users] : rooms)
    {
      if(users.contains(User))
       subscribedRooms.insert(room);
    }

    return subscribedRooms;
  }


  private:
  /*
    rooms is a map of sets i.e.,
    rooms = {
        "general": {'user1', 'user2'},
        "gaming": {'user1', 'user3'}
    }
 
  */
  map<string, set<string>> rooms;
};

int main()
{
  Broker broker;

  broker.subscribe("user1", "general");
  broker.subscribe("user2", "general");

  broker.subscribe("bob", "gaming");
  broker.subscribe("alice", "gaming");

  set<string> generalSubscribers = broker.fetch_subscribers("general");
  set<string> gamingSubscribers = broker.fetch_subscribers("gaming");

  for (string users: generalSubscribers)
  {
    cout << users << endl;
  }

  for (string users: gamingSubscribers)
  {
    cout << users << endl;
  }
}
