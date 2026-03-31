#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "../broker/broker_client.hpp"
#include "../lib/server.hpp"
#include "ws_session.hpp"
#include <map>
#include <memory>
namespace asio  = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;


class WebSocketServer : public Server
{
  public:
  WebSocketServer(asio::io_context& ioc, tcp::endpoint endpoint) : Server(ioc, endpoint), broker_client(ioc) { set_forward_handler(); broker_client.run("broker", "6767");};
  

  std::shared_ptr<Session> create_session(tcp::socket socket) override;

  private:
  //self-explanatory - sits on socket and accepts incoming clients kicks off handshake
  //moves to ws session

  void handle_message(const string& message, std::shared_ptr<WebSocketSession> session);
  void handle_forward(const string& message);
  void set_forward_handler();

  void broadcast(const string& username, const string& room, const string& message);

  void join(const string& username, const string& room, std::shared_ptr<WebSocketSession> session);

  void publish(const string& username, const string& room, string& content);

  //accepts incoming TCP connections, forwards to WS
  BrokerClient broker_client;
  std::map<std::string,std::weak_ptr<WebSocketSession>> connections;
};
