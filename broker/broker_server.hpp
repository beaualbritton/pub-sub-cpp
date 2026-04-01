#include "../lib/server.hpp"
#include "broker_session.hpp"
#include <map>
#include <set>
#include <string>
#include <boost/json.hpp>
using std::map, std::string, std::set;
namespace json = boost::json;

class Broker : public Server
{

  public:
  Broker(asio::io_context& ioc, tcp::endpoint endpoint) : Server(ioc, endpoint){};

  //overrides base class
  std::shared_ptr<Session> create_session(tcp::socket socket) override;

  //parses message actions received from servers
  void handle_message(const string& message, std::shared_ptr<BrokerSession> session);

  //forwards messages from Server A -> all servers, universal subscriber list
  void forward(json::object& jobj, const string& room);
  //same class methods before broker became seprate service
  void subscribe(const string& id, const string& room);
  void unsubscribe(const string& id, const string& room);
  void unsubscribe_all(const string& id);
  set<string> fetch_subscribers(const string& room);
  set<string> fetch_rooms(const string& id);


  private:
  /*
    rooms is a map of sets i.e.,
    rooms = {
        "general": {'user1', 'user2'},
        "gaming": {'user1', 'user3'}
    }
 
  */
  map<string, set<string>> rooms;
  std::map<std::string,std::weak_ptr<BrokerSession>> connections;
};
