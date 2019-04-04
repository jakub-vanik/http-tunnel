#ifndef RELAY_CONNECTION_H_
#define RELAY_CONNECTION_H_

#include <Poco/Exception.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/Logger.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>
#include <Poco/Timestamp.h>
#include <Poco/Timespan.h>
#include "relay.h"

class Relay;

class RelayConnection
{
public:
  RelayConnection(Relay &relay, int id);
  ~RelayConnection();
  int getId();
  bool isDead();
  const Poco::Timestamp &getLastActivity();
  void open(int port);
  void data(Poco::BasicFIFOBuffer<char> &data);
  void close();
private:
  Poco::Logger &logger;
  Relay &relay;
  int id;
  volatile bool closed;
  bool dead;
  Poco::Timestamp lastActivity;
  Poco::Net::StreamSocket socket;
  Poco::RunnableAdapter<RelayConnection> runnable;
  Poco::Thread thread;
  void loop();
};

#endif
