#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/uuid.hpp>
#include "../broker/broker_client.hpp"
#include "../lib/server.hpp"
#include "ws_session.hpp"
#include <boost/uuid/uuid_io.hpp>
#include <map>
#include <memory>
namespace asio  = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;


class WebSocketServer : public Server
{
  public:
  WebSocketServer(asio::io_context& ioc, tcp::endpoint endpoint) : Server(ioc, endpoint), broker_client(ioc) { set_forward_handler(); broker_client.run("broker", "6767", boost::uuids::to_string(this->id_));}; // run broker 

  //overrides base class
  std::shared_ptr<Session> create_session(tcp::socket socket) override;

  private:
  //handle message for end-users
  void handle_message(const string& message, std::shared_ptr<WebSocketSession> session);
  //handle received message from broker
  void handle_forward(const string& message);
  void set_forward_handler();

  void broadcast(const string& username, const string& room, const string& message);
  void join(const string& username, const string& room, std::shared_ptr<WebSocketSession> session);
  void publish(const string& username, const string& room, string& content);

  //client to connect to broker service instead of outright owning broker object
  BrokerClient broker_client;
  std::map<std::string,std::weak_ptr<WebSocketSession>> connections;
};
