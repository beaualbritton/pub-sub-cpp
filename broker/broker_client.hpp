//SEE: https://www.boost.org/doc/libs/develop/libs/beast/example/websocket/client/async/websocket_client_async.cpp
#pragma once
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/uuid.hpp>
#include <deque>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;

using tcp = boost::asio::ip::tcp;
using uuid = boost::uuids::uuid;
using std::move, std::cerr, std::endl, std::string, std::function;

// connects server to broker service
// client (WS Server)<--> broker session <--> broker server
class BrokerClient
{
  public:
  //wrap tcp socket ownership into ws_
  explicit BrokerClient(boost::asio::io_context& ioc) : resolver_(ioc), ws_(boost::asio::make_strand(ioc)) { writing_ = false; }

  ~BrokerClient() { cerr << "Server" << host_ << " closed" << endl; }

  void run(const string& host, const string& port, string id);

  void set_handler(function<void(const string&)>handler);
  void send(const string& content);

  private:

  void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
  void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
  void on_handshake(beast::error_code ec);
  void on_close(beast::error_code ec);

  void do_read_loop();
  void do_write();

  void on_read(beast::error_code ec, size_t n);
  void on_write(beast::error_code ec, size_t n);

  
  tcp::resolver resolver_;
  // tcp socket handles bytes, websocket on top.
  websocket::stream<tcp::socket> ws_; // this was tcp stream in docs, gave me some issues
  beast::flat_buffer buffer_; //scary!
  std::string host_;
  std::string response_;



  function<void(const string&)> handler_;
  std::deque<string> send_queue;
  bool writing_;
  string server_id_;
};
