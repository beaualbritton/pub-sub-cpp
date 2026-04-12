#include "logger.hpp"
#include <boost/json/object.hpp>
#include <iostream>
#include <memory>
#include <string>

using std::string, std::cout, std::endl;

void Logger::init_db()
{
  std::string url = std::getenv("DATABASE_URL");
  cout << "DATABASE_URL: " << url << endl;

  db_ = std::make_unique<connection>(url);

  cout << "connected to supabase" << endl;
}

void Logger::on_log(json::object& jobj)
{

  string action = "", server_id = "", sender_id = "", room= "", msg="";

  if(jobj.contains("action"))
    action = string(jobj["action"].as_string());

  if(jobj.contains("server"))
    server_id = string(jobj["server"].as_string());

  if(jobj.contains("id"))
    sender_id = string(jobj["id"].as_string());

  if(jobj.contains("room"))
    room = string(jobj["room"].as_string());

  if(jobj.contains("message"))
    msg = string(jobj["message"].as_string());

  pqxx::work transaction(*db_);

  string query =  "INSERT INTO messages (action, server, sender, room, message) "
                  "VALUES ($1, $2, $3, $4, $5)";

  pqxx::params parameters{action, server_id, sender_id, room, msg};

  transaction.exec(query, parameters).no_rows();
  transaction.commit();
}
