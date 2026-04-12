#include <memory>
#include <pqxx/pqxx>
#include <boost/json.hpp>

using connection = pqxx::connection;
namespace json = boost::json;

class Logger
{
  public:
  Logger(){init_db();};
  void on_log(json::object& jobj);

  private:
  void init_db();

  std::unique_ptr<connection> db_;
};
