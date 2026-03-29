#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "../lib/session.hpp"
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;

using tcp = boost::asio::ip::tcp;
using std::move, std::cerr, std::endl, std::string, std::function;

class BrokerSession : public Session
{
  public:
  //wrap tcp socket ownership into ws_
  BrokerSession(tcp::socket socket) : Session(std::move(socket)) {}
};
