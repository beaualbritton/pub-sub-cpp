#include "broker_server.hpp"
#include "../lib/actions.hpp"
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


int main()
{
  std::cout << "Broker server is running:\n";

  try
  {
    asio::io_context ioc;
    tcp::endpoint endpoint(tcp::v4(), 6767);
    Broker broker(ioc, endpoint);

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
