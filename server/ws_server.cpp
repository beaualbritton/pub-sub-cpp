// SourcBrokerSessione - https://stackoverflow.com/a/78513859
// Retrieved 2026-02-21, License - CC BY-SA 4.0
#include "ws_server.hpp"
#include "../lib/actions.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <string>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace json = boost::json;

using tcp = asio::ip::tcp;
using std::cerr, std::set, std::endl, std::string, std::exception, std::make_shared, std::weak_ptr, std::shared_ptr;


std::shared_ptr<Session> WebSocketServer::create_session(tcp::socket socket)
{
  auto session = make_shared<WebSocketSession>(std::move(socket));

  //auto -> std::function<void(const string& message)>
  auto handler_lambda = [this, session](const string& message)
  {
    handle_message(message,session);
  };

  session->set_handler(std::move(handler_lambda));

  return session;
}

void WebSocketServer::handle_message(const string& message, shared_ptr<WebSocketSession> session)
{
  //ugly
  json::value jv = json::parse(message);
  json::object& jobj = jv.as_object();

  string action = string(jobj["action"].as_string());
  string username = string(jobj["from"].as_string());

  string room = "general";

  switch(parse_action(action))
  {
    case Action::JOIN:
    {
      join(username, room, session);
      break;
    }
    case Action::PUBLISH:
    {
     string content = string(jobj["content"].as_string());

      publish(username, room, content);
      break;


    }
    case Action::NOOP:
      return;
  };

}

void WebSocketServer::handle_forward(const string& msg_recv)
{
  json::value jv = json::parse(msg_recv);
  json::object& jobj = jv.as_object();

  string from = string(jobj["id"].as_string());
  string msg = string(jobj["message"].as_string());

  json::array& all_subscribers = jobj["subscribers"].as_array();
  std::set<string> subscribers;

  for(auto sub : all_subscribers)
    subscribers.insert(string(sub.as_string()));

  for(string sub: subscribers)
  {
    //iterator for map, i don't like using auto but it'd look.. disgusting 
    // map<std::string,std::weak_ptr<WebSocketSession>>::iterator!!!!
    
    auto client_iterator = connections.find(sub);

    if (client_iterator != connections.end())
    {
      shared_ptr<WebSocketSession> client;
      client = client_iterator->second.lock();

      if(client)
      {
        if(sub != from)
          client->send(msg);
      }

      else
        connections.erase(client_iterator);
    }
  }
  std::cout << msg << std::endl;
}

void WebSocketServer::set_forward_handler()
{
  //auto -> std::function<void(const string& message)>
  auto forward_lambda = [this](const string& message)
  {
    handle_forward(message);
  };

  broker_client.set_handler(forward_lambda);

}

void WebSocketServer::broadcast(const string& username, const string& room, const string& message)
{
  json::object msg;
  msg["action"] = "forward";
  msg["id"] = username;
  msg["room"] = room;
  msg["message"] = message;

  broker_client.send(json::serialize(msg));
}

void WebSocketServer::join(const string& username, const string& room, std::shared_ptr<WebSocketSession> session)
{
  connections[username] = session;

  json::object msg;
  msg["action"] = "subscribe";
  msg["id"] = username;
  msg["room"] = room;

  broker_client.send(json::serialize(msg));
  string content = "joined room " + room;

  publish(username, room, content);
}

void WebSocketServer::publish(const string& username, const string& room, string& content)
{
  content = username + ": " + content;
  broadcast(username, room, content);
}

int main()
{
  std::cout << "Web server is running:\n";

  try
  {
    asio::io_context ioc;
    int port = std::stoi(std::getenv("PORT"));
    tcp::endpoint endpoint(tcp::v4(), port);
    WebSocketServer server(ioc, endpoint);

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
