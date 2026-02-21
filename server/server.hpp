#include <boost/asio.hpp>
#include <boost/beast.hpp>

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

  //accepts incoming TCP connections, forwards to WS
  tcp::acceptor acceptor_;
};
