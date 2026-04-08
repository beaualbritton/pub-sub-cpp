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

//whenever a server joins, add it to a map of connected servers
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

//parses received messages from server, corresponding actions
void Broker::handle_message(const string& message, std::shared_ptr<BrokerSession> session)
{
  json::value jv = json::parse(message);
  json::object& jobj = jv.as_object();

  //some of these fields might be null - subscribe won't carry a message for example
  string action = "", session_id = "", server_id = "",msg = "", room ="";

  if(jobj.contains("action"))
    action = string(jobj["action"].as_string());

  if(jobj.contains("server"))
    server_id = string(jobj["server"].as_string());

  if(jobj.contains("id"))
    session_id = string(jobj["id"].as_string());

  if(jobj.contains("message"))
    msg = string(jobj["message"].as_string());

  if(jobj.contains("room"))
    room = string(jobj["room"].as_string());

  switch(parse_action(action))
  {
    case Action::REGISTER:
    {
      register_server(server_id, session);
      break;
    }
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
      append_jobj(jobj, rooms, "rooms");
      send_back(jobj,session);
      break;
    }
    case Action::FETCH_ALL_ROOMS:
    {
      set<string> all_rooms = fetch_all_rooms();
      append_jobj(jobj, all_rooms, "all_rooms");
      send_back(jobj,session);
      break;
    }
    case Action::FETCH_SUBSCRIBERS:
    {
      set<string> subscribers = fetch_subscribers(room);
      append_jobj(jobj, subscribers, "subscribers");
      send_back(jobj,session);
      break;
    }
    case Action::FORWARD:
    {
      //appends message received from server with a subscriber list
      //depending on room's subscribers - send message appended w/ universal list
      forward(jobj, room);
      break;
    }
    case Action::NOOP:
      return;
  }
}

//forwards message from Server A -> all servers, with appended subscriber list
void Broker::forward(json::object& jobj, const string& room)
{
  set<string> subscribers = fetch_subscribers(room);
  append_jobj(jobj, subscribers, "subscribers");

  //iterate thru map (dictionary) w/ pairs
  for (const auto& [id, connection] : connections)
  {
    shared_ptr<BrokerSession> session = connection.lock();
    //send each Server a new message
    if(session)
    {
      session->send(json::serialize(jobj));
    }
    else
      connections.erase(id); //TODO: delete connections another way
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

set<string> Broker::fetch_all_rooms()
{
  set<string> subscribedRooms;

  for(auto& [room, users] : rooms)
    subscribedRooms.insert(room);

  return subscribedRooms;
}

void Broker::register_server(const string& server_id, std::shared_ptr<BrokerSession> session)
{
  connections[server_id] = session;
}

void Broker::send_back(json::object& jobj, std::shared_ptr<BrokerSession> session) 
{
  session->send(json::serialize(jobj));
}

void Broker::append_jobj(json::object& jobj, set<string> to_append, const string entry)
{
  json::array jarray;

  for(const string& sub : to_append)
  {
    jarray.push_back(json::value(sub));
  }

  jobj[entry] = jarray;
}
