#include "queue.h"

namespace Link {

Queue::Queue(int capacity) :
    logger(Poco::Logger::get("queue"))
{
  this->capacity = capacity;
  waiting = 0;
}

Queue::~Queue()
{
}

void Queue::push(const std::string &text)
{
  mutex.lock();
  if ((int) queue.size() >= capacity)
  {
    waiting++;
    logger.warning("queue is full, waiting");
    full.wait(mutex);
    logger.warning("queue freed, continuing");
  }
  if (queue.empty())
  {
    empty.signal();
  }
  queue.push(text);
  mutex.unlock();
}

bool Queue::pop(std::string &text, int timeout)
{
  mutex.lock();
  if (queue.empty())
  {
    if (timeout == 0)
    {
      mutex.unlock();
      return false;
    }
    if (!empty.tryWait(mutex, timeout))
    {
      mutex.unlock();
      return false;
    }
  }
  if (waiting > 0)
  {
    waiting--;
    full.signal();
  }
  text = queue.front();
  queue.pop();
  mutex.unlock();
  return true;
}

}
