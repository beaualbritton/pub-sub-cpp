#include "session.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <exception>
#include <iostream>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;

using tcp = asio::ip::tcp;
using std::cerr, std::endl, std::exception;


//websocket upgrade http -> websocket
void WebSocketSession::run()
{
  ws_.async_accept(beast::bind_front_handler(&WebSocketSession::on_accept,
  /*shared_from_this prevents dangling ptr*/ shared_from_this()));
}

void WebSocketSession::on_accept(beast::error_code ec)
{
  cerr << "Accept: " << ec.message() << " for " << peer_ << endl;
  if (!ec)
    do_read_loop();
}

void WebSocketSession::do_read_loop()
{
  //schedules next async on_read, keeps session alive until read, so not reading/using after free, binds fuction to shared_from_this
  ws_.async_read(buffer_, beast::bind_front_handler(&WebSocketSession::on_read, shared_from_this()));
}

void WebSocketSession::on_read(beast::error_code ec, size_t n)
{
  if (ec)
  {
    if (ec == websocket::error::closed)
      return;

    cerr << "Read error: " << ec.message() << endl;
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
  catch (exception const& e)
  {
    cerr << "Processing error: " << e.what() << endl;
  }
}

//misleading, happens after send finishes
//cycling read -> write -> read -> write etc etc
void WebSocketSession::on_write(beast::error_code ec, size_t n)
{
   cerr << "Write: " << n << " bytes (" << ec.message() << ")" << endl;
   if (!ec)
       do_read_loop();
}
