#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;

using tcp = boost::asio::ip::tcp;
using std::move, std::cerr, std::endl;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
  public:
  //wrap tcp socket ownership into ws_
  WebSocketSession(tcp::socket socket) : ws_(std::move(socket)) {}
  ~WebSocketSession() { cerr << "Session " << peer_ << " closed" << endl; }


  void run();


  private:
  void on_accept(beast::error_code ec);
  void do_read_loop();
  void on_read(beast::error_code ec, size_t n);
  void on_write(beast::error_code ec, size_t n);

  // tcp socket handles bytes, websocket on top.
  websocket::stream<tcp::socket> ws_;
  beast::flat_buffer buffer_; //scary!

  std::string response_;
  //digs into transport layer to get client ip info
  tcp::endpoint peer_ = ws_.next_layer().remote_endpoint();

};
