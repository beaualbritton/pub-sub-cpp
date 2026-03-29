#include "../lib/server.hpp"
#include "broker_session.hpp"
#include <map>
#include <set>
#include <string>
using std::map, std::string, std::set;

class Broker : public Server
{

  public:
  Broker(asio::io_context& ioc, tcp::endpoint endpoint) : Server(ioc, endpoint){};

  std::shared_ptr<Session> create_session(tcp::socket socket) override;

  void handle_message(const string& message, std::shared_ptr<BrokerSession> session);

  void subscribe(const string& id, const string& Room);
  void unsubscribe(const string& id, const string& Room);
  void unsubscribe_all(const string& id);
  set<string> fetch_subscribers(const string& Room);
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
};
