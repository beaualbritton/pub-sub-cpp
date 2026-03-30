#include "broker_server.hpp"
#include "../lib/actions.hpp"
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>

using std::map, std::string, std::set;
using std::cout, std::endl;
using std::cerr, std::endl, std::string, std::to_string, std::exception, std::make_shared, std::weak_ptr, std::shared_ptr;

namespace json = boost::json;

std::shared_ptr<Session> Broker::create_session(tcp::socket socket)
{
  auto session = make_shared<BrokerSession>(std::move(socket));

  string server_id = to_string(connections.size());
  connections[server_id] = session;

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
  string action = "", session_id = "", msg = "", room ="";

  if(jobj.contains("action"))
    action = string(jobj["action"].as_string());

  if(jobj.contains("id"))
    session_id = string(jobj["id"].as_string());

  if(jobj.contains("message"))
    msg = string(jobj["message"].as_string());

  if(jobj.contains("room"))
    room = string(jobj["room"].as_string());

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
    case Action::FORWARD:
    {
      forward(jobj, room);
      break;
    }
    case Action::NOOP:
      return;
  }
}

void Broker::forward(json::object& jobj, const string& room)
{
  set<string> subscribers = fetch_subscribers(room);
  json::array jsubs;

  for(const string& sub : subscribers)
  {
    jsubs.push_back(json::value(sub));
  }

  jobj["subscribers"] = jsubs;
  //iterator for map,   
  //map<std::string,std::weak_ptr<WebSocketSession>>::iterator!!!!
  for (const auto& [id, connection] : connections)
  {
    shared_ptr<BrokerSession> session = connection.lock();
    if(session)
    {
      session->send(json::serialize(jobj));
    }
    else
      connections.erase(id);
  }
}

void Broker::subscribe(const string& id, const string& room)
{
  rooms[room].insert(id);
}

void Broker::unsubscribe(const string &id, const string& room)
{
  rooms[room].erase(id);
}

void Broker::unsubscribe_all(const string& id)
{
  //auto& cuts through verbosity -> map<string, set<string>::value pair& : rooms (pair.first, pair.second). yuck!
  for(auto& [room, users]: rooms)
  {
    users.erase(id);
  }
}

set<string> Broker::fetch_subscribers(const string& room)
{
  return rooms[room];
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
