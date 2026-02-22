#include <string>

enum class Action  {JOIN, PUBLISH, NOOP};

Action parse_action(const std::string& action)
{
  if (action == "join")
    return Action::JOIN;
  if (action == "publish")
    return Action::PUBLISH;

  return Action::NOOP;
}
