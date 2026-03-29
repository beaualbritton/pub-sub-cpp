#pragma once
#include <string>

enum class Action  {JOIN, PUBLISH, SUBSCRIBE, UNSUBSCRIBE, UNSUBSCRIBE_ALL, FETCH_ROOMS, FETCH_SUBSCRIBERS, NOOP};

inline Action parse_action(const std::string& action)
{
  if (action == "join")
    return Action::JOIN;
  if (action == "publish")
    return Action::PUBLISH;
  if (action == "subscribe")
    return Action::SUBSCRIBE;
  if (action == "unsubscribe")
    return Action::UNSUBSCRIBE;
  if (action == "unsubscribe_all")
    return Action::UNSUBSCRIBE_ALL;
  if (action == "fetch_rooms")
    return Action::FETCH_ROOMS;
  if (action == "fetch_subscribers")
    return Action::FETCH_SUBSCRIBERS;

  return Action::NOOP;
}
