// Source - https://stackoverflow.com/a/78513859
// Posted by sehe
// Retrieved 2026-02-21, License - CC BY-SA 4.0
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>

namespace asio      = boost::asio;
namespace beast     = boost::beast;
namespace websocket = beast::websocket;
using tcp           = asio::ip::tcp;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> 
{

  public:
  //wrap tcp socket ownership into ws_
  WebSocketSession(tcp::socket socket) : ws_(std::move(socket)) {}

  ~WebSocketSession() { std::cerr << "Session " << peer_ << " closed" << std::endl; }

  //websocket upgrade http -> websocket
  void run()
  {
    ws_.async_accept(beast::bind_front_handler(&WebSocketSession::on_accept,
    /*shared_from_this prevents dangling ptr*/ shared_from_this()));
  }

  private:
  void on_accept(beast::error_code ec)
  {
    std::cerr << "Accept: " << ec.message() << " for " << peer_ << std::endl;
    if (!ec)
      do_read_loop();
  }
  
  void do_read_loop()
  {
    //schedules next async on_read, keeps session alive until read, so not reading/using after free, binds fuction to shared_from_this
    ws_.async_read(buffer_, beast::bind_front_handler(&WebSocketSession::on_read, shared_from_this()));
  }

  void on_read(beast::error_code ec, size_t n)
  {
    if (ec)
    {
      if (ec == websocket::error::closed)
        return;

      std::cerr << "Read error: " << ec.message() << std::endl;
      return;
    }

    try
    {
      //read raw bytes into buffer, clear it & echo back to client
      response_ = beast::buffers_to_string(buffer_.data());
      buffer_.consume(n);

      ws_.text(true);
      ws_.async_write(asio::buffer(response_),
                      beast::bind_front_handler(&WebSocketSession::on_write, shared_from_this())); //binds function to shared_from_this
    }
    catch (std::exception const& e)
    {
      std::cerr << "Processing error: " << e.what() << std::endl;
    }
  }
  
  //misleading, happens after send finishes
  //cycling read -> write -> read -> write etc etc
  void on_write(beast::error_code ec, size_t n)
  {
     std::cerr << "Write: " << n << " bytes (" << ec.message() << ")" << std::endl;
     if (!ec)
         do_read_loop();
  }

  // tcp socket handles bytes, websocket on top.
  websocket::stream<tcp::socket> ws_;   
  beast::flat_buffer buffer_; //scary!

  std::string response_;
  //digs into transport layer to get client ip info
  tcp::endpoint peer_ = ws_.next_layer().remote_endpoint();
};

class WebSocketServer 
{
  public:
  WebSocketServer(asio::io_context& ioc, tcp::endpoint endpoint) : acceptor_(ioc, endpoint) { do_accept(); }

  private:
  void do_accept()
  {
    acceptor_.async_accept(beast::bind_front_handler(&WebSocketServer::on_accept, this));
  }
  
  //self-explanatory - sits on socket and accepts incoming clients kicks off handshake
  //moves to ws session
  void on_accept(beast::error_code ec, tcp::socket socket)
  {
    if (ec)
      std::cerr << "Accept error: " << ec.message() << std::endl;

    else
      std::make_shared<WebSocketSession>(std::move(socket))->run();

    do_accept();
  }

  tcp::acceptor acceptor_;
};

int main()
{
  std::cout << "Web server is running:\n";

  try
  {
    asio::io_context ioc;
    tcp::endpoint endpoint(tcp::v4(), 6969);
    WebSocketServer server(ioc, endpoint);

    //io_context is event loop engine, ioc does the 'work' of the server
    //will need to add multiple threads using ioc.run later
    ioc.run();
  }
  catch (std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
