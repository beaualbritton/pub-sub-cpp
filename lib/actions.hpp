#pragma once
#include <string>

enum class Action  {REGISTER, JOIN, PUBLISH, FORWARD, SUBSCRIBE, UNSUBSCRIBE, UNSUBSCRIBE_ALL, FETCH_ROOMS,FETCH_ALL_ROOMS,FETCH_SUBSCRIBERS, NOOP};

inline Action parse_action(const std::string& action)
{
  if (action == "register")
    return Action::REGISTER;
  if (action == "join")
    return Action::JOIN;
  if (action == "publish")
    return Action::PUBLISH;
  if (action == "forward")
    return Action::FORWARD;
  if (action == "subscribe")
    return Action::SUBSCRIBE;
  if (action == "unsubscribe")
    return Action::UNSUBSCRIBE;
  if (action == "unsubscribe_all")
    return Action::UNSUBSCRIBE_ALL;
  if (action == "fetch_rooms")
    return Action::FETCH_ROOMS;
  if (action == "fetch_all_rooms")
    return Action::FETCH_ALL_ROOMS;
  if (action == "fetch_subscribers")
    return Action::FETCH_SUBSCRIBERS;

  return Action::NOOP;
}
