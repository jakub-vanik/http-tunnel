#ifndef RELAY_H_
#define RELAY_H_

#include <string>
#include <vector>
#include <Poco/Error.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/FIFOBufferStream.h>
#include <Poco/Logger.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>
#include "link/server_link.h"
#include "link/message.h"
#include "relay_connection.h"

class RelayConnection;

class Relay
{
public:
  Relay(int port, int timeout, const std::string &password, int httpPort, const std::string &baseUrl);
  ~Relay();
  void start();
  void stop();
  void sendData(int id, Poco::FIFOBuffer &data);
  void sendClose(int id);
private:
  Poco::Logger &logger;
  int port;
  int timeout;
  Poco::RunnableAdapter<Relay> runnable;
  Poco::Thread thread;
  volatile bool running;
  Link::ServerLink link;
  std::vector<RelayConnection *> connections;
  void loop();
  void cleanupConnections();
};

#endif
