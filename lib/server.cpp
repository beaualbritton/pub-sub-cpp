// Source - https://stackoverflow.com/a/78513859
// Retrieved 2026-02-21, License - CC BY-SA 4.0
#include "server.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace json = boost::json;

using tcp = asio::ip::tcp;
using std::cerr, std::endl, std::string, std::exception, std::make_shared, std::weak_ptr, std::shared_ptr;


void Server::do_accept()
{
  acceptor_.async_accept(beast::bind_front_handler(&Server::on_accept, this));
}

//self-explanatory - sits on socket and accepts incoming clients kicks off handshake
//moves to ws session
void Server::on_accept(beast::error_code ec, tcp::socket socket)
{
  if (ec)
    cerr << "Accept error: " << ec.message() << endl;
  else
  {
    auto session = create_session(std::move(socket));
    session->run();
  }
  do_accept();
}



int main()
{
  std::cout << "Web server is running:\n";

  try
  {
    asio::io_context ioc;
    tcp::endpoint endpoint(tcp::v4(), 6969);
    Server server(ioc, endpoint);

    //io_context is event loop engine, ioc does the 'work' of the server
    //will need to add multiple threads using ioc.run later
    ioc.run();
  }
  catch (exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
