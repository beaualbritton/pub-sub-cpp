#include <map>
#include <set>
#include <string>
using std::map, std::string, std::set;

class Broker
{
  public:
  void subscribe(string User, string Room);
  void unsubscribe(string User, string Room);
  void unsubscribe_all(string User);
  set<string> fetch_subscribers(string Room);
  set<string> fetch_rooms(string User);


  private:
  map<string, set<string>> rooms;
};
