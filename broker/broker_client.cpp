//SEE: https://www.boost.org/doc/libs/develop/libs/beast/example/websocket/client/async/websocket_client_async.cpp
#include "broker_client.hpp"
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <exception>
#include <deque>
#include <iostream>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace json = boost::json;

using tcp = boost::asio::ip::tcp;
using std::move, std::cerr, std::endl, std::string, std::function, std::exception;

//run client based on provided host info
void BrokerClient::run(const string& host, const string& port, string id)
{
  host_ = host;
  server_id_ = id;
  resolver_.async_resolve(host,port,
                          beast::bind_front_handler(&BrokerClient::on_resolve, this));
}

void BrokerClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
  if(ec)
  {
    cerr << "Failure: " << ec.message() << " resolve" << endl;
    return;
  }

  asio::async_connect(ws_.next_layer(), results,
                      beast::bind_front_handler(&BrokerClient::on_connect, this));
}

void BrokerClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
  if(ec)
  {
    cerr << "Failure: " << ec.message() << " connect" << endl;
    return;
  }

  ws_.set_option(websocket::stream_base::timeout::suggested(
                 beast::role_type::client));

  ws_.set_option(websocket::stream_base::decorator(
      [](websocket::request_type& req)
      {
          req.set(beast::http::field::user_agent,
                  std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async");
      }));

  host_ += ':' + std::to_string(ep.port());

  ws_.async_handshake(host_, "/",
                      beast::bind_front_handler(&BrokerClient::on_handshake, this));
}


void BrokerClient::on_handshake(beast::error_code ec)
{
  if(ec)
  {
    cerr << "Failure: " << ec.message() << " handshake" << endl;
    return;
  }

  json::object msg;
  msg["action"] = "register";
  msg["server"] = server_id_;
  send(json::serialize(msg));

  ws_.async_read(buffer_,beast::bind_front_handler(&BrokerClient::on_read, this));
}
// same read/write loop as session, write queue for send(), read loop for received message

void BrokerClient::do_read_loop()
{
  //schedules next async on_read, keeps session alive until read, so not reading/using after free, binds fuction to shared_from_this
  ws_.async_read(buffer_, beast::bind_front_handler(&BrokerClient::on_read, this));
}

void BrokerClient::on_read(beast::error_code ec, size_t n)
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
    //read raw bytes into buffer, clear it & send to message handler in server
    response_ = beast::buffers_to_string(buffer_.data());
    buffer_.consume(n);

    handler_(response_);
    do_read_loop();

  }
  catch (exception const& e)
  {
    cerr << "Processing error: " << e.what() << endl;
  }
}

//misleading, happens after send finishes
//cycling read -> write -> read -> write etc etc
void BrokerClient::on_write(beast::error_code ec, size_t n)
{
  cerr << "Write: " << n << " bytes (" << ec.message() << ")" << endl;
  if (!ec)
  {
    send_queue.pop_front();
    writing_ = false;

    if(!send_queue.empty())
      do_write();
  }
}

void BrokerClient::set_handler(function<void(const string&)>handler)
{
  handler_ = std::move(handler);
}

void BrokerClient::send(const string& content)
{
  send_queue.emplace_back(content);
  if(!writing_)
    do_write();
}

void BrokerClient::do_write()
{
  writing_ = true;
  string front = send_queue.front();
  ws_.text(true);
  ws_.async_write(asio::buffer(front),
                  beast::bind_front_handler(&BrokerClient::on_write, this));

}
