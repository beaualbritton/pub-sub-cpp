#include "broker_server.hpp"
#include "../lib/actions.hpp"
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>

using std::map, std::string, std::set;
using std::cout, std::endl;
using std::cerr, std::endl, std::string, std::exception, std::make_shared, std::weak_ptr, std::shared_ptr;

namespace json = boost::json;

std::shared_ptr<Session> Broker::create_session(tcp::socket socket)
{
  auto session = make_shared<BrokerSession>(std::move(socket));

  //auto -> std::function<void(const string& message)>
  auto handler_lambda = [this, session](const string& message)
  {
    handle_message(message,session);
  };

  session->set_handler(std::move(handler_lambda));

  return session;
}

void Broker::handle_message(const string& message, std::shared_ptr<BrokerSession> session)
{
 //ugly
  json::value jv = json::parse(message);
  json::object& jobj = jv.as_object();

  string action = string(jobj["action"].as_string());
  string session_id = string(jobj["session_id"].as_string());
  string server_id = string(jobj["server_id"].as_string());

  string room = "general";

  switch(parse_action(action))
  {
    case Action::SUBSCRIBE:
    {
      subscribe(session_id, room);
      break;
    }
    case Action::UNSUBSCRIBE:
    {
      unsubscribe(session_id, room);
      break;
    }
    case Action::UNSUBSCRIBE_ALL:
    {
      unsubscribe_all(session_id);
      break;
    }
    case Action::FETCH_ROOMS:
    {
      set<string> rooms = fetch_rooms(session_id);
      break;
    }
    case Action::FETCH_SUBSCRIBERS:
    {
      set<string> subscribers = fetch_subscribers(room);
      break;
    }
    case Action::NOOP:
      return;
  }
}

void Broker::subscribe(const string& id, const string& Room)
{
  rooms[Room].insert(id);
}

void Broker::unsubscribe(const string &id, const string& Room)
{
  rooms[Room].erase(id);
}

void Broker::unsubscribe_all(const string& id)
{
  //auto& cuts through verbosity -> map<string, set<string>::value pair& : rooms (pair.first, pair.second). yuck!
  for(auto& [room, users]: rooms)
  {
    users.erase(id);
  }
}

set<string> Broker::fetch_subscribers(const string& Room)
{
  return rooms[Room];
}

set<string> Broker::fetch_rooms(const string& id)
{
  set<string> subscribedRooms;

  for(auto& [room, users] : rooms)
  {
    if(users.contains(id))
     subscribedRooms.insert(room);
  }

  return subscribedRooms;
}
