#include <map>
#include <set>
#include <string>
using std::map, std::string, std::set;

class Broker
{
  public:
  Broker();
  void subscribe(const string& User, const string& Room);
  void unsubscribe(const string& User, const string& Room);
  void unsubscribe_all(const string& User);
  set<string> fetch_subscribers(const string& Room);
  set<string> fetch_rooms(const string& User);


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
