#ifndef LINK_QUEUE_H_
#define LINK_QUEUE_H_

#include <queue>
#include <string>
#include <Poco/Condition.h>
#include <Poco/Logger.h>
#include <Poco/Mutex.h>

namespace Link {

class Queue
{
public:
  Queue(int capacity);
  ~Queue();
  void push(const std::string &text);
  bool pop(std::string &text, int timeout);
private:
  Poco::Logger &logger;
  int capacity;
  int waiting;
  std::queue<std::string> queue;
  Poco::Mutex mutex;
  Poco::Condition empty;
  Poco::Condition full;
};

}

#endif
