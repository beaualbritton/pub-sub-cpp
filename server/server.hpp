#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "../broker/broker.hpp"
#include "session.hpp"
#include <map>
#include <memory>
namespace asio  = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;


class WebSocketServer 
{
  public:
  WebSocketServer(asio::io_context& ioc, tcp::endpoint endpoint) : acceptor_(ioc, endpoint) { do_accept(); }

  private:
  void do_accept();

  //self-explanatory - sits on socket and accepts incoming clients kicks off handshake
  //moves to ws session
  void on_accept(beast::error_code ec, tcp::socket socket);

  void handle_message(const string& message, std::shared_ptr<WebSocketSession> session);

  void broadcast(const string& username, const string& room, const string& message);

  void join(const string& username, const string& room, std::shared_ptr<WebSocketSession> session);

  void publish(const string& username, const string& room, string& content);

  //accepts incoming TCP connections, forwards to WS
  tcp::acceptor acceptor_;
  Broker broker;
  std::map<std::string,std::weak_ptr<WebSocketSession>> connections;
};
