#ifndef PROXY_H_
#define PROXY_H_

#include <string>
#include <vector>
#include <Poco/FIFOBuffer.h>
#include <Poco/FIFOBufferStream.h>
#include <Poco/Logger.h>
#include <Poco/Mutex.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerConnectionFactory.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>
#include <Poco/ThreadPool.h>
#include "link/client_link.h"
#include "link/message.h"
#include "proxy_connection.h"

class ProxyConnection;

class Proxy
{
public:
  Proxy(int port, const std::string &password, const std::string &hostName, int httpPort, const std::string &baseUrl, const std::string &fakeHost);
  ~Proxy();
  void start();
  void stop();
  int registerConnection(ProxyConnection *connection);
  void unregisterConnection(ProxyConnection *connection);
  void sendReset();
  void sendOpen(int id, Poco::FIFOBuffer &data);
  void sendData(int id, Poco::FIFOBuffer &data);
  void sendClose(int id);
private:
  Poco::Logger &logger;
  Poco::ThreadPool threadPool;
  Poco::Net::ServerSocket socket;
  Poco::Net::TCPServer server;
  Poco::RunnableAdapter<Proxy> runnable;
  Poco::Thread thread;
  volatile bool running;
  Link::ClientLink link;
  Poco::Mutex connectionsLock;
  std::vector<ProxyConnection *> connections;
  int nextId;
  void loop();
  int generateId();
};

#endif
