#include <map>
#include <set>
#include <string>
using std::map, std::string, std::set;

class Broker
{
  public:
  void subscribe(string& User, const string& Room);
  void unsubscribe(string& User, const string& Room);
  void unsubscribe_all(string& User);
  set<string> fetch_subscribers(const string& Room);
  set<string> fetch_rooms(string& User);


  private:
  map<string, set<string>> rooms;
};
