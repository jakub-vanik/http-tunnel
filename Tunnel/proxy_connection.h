#ifndef PROXY_CONNECTION_H_
#define PROXY_CONNECTION_H_

#include <Poco/Exception.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/Logger.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerConnectionFactory.h>
#include "proxy.h"

class Proxy;

class ProxyConnection: public Poco::Net::TCPServerConnection
{
public:
  ProxyConnection(Proxy &proxy, const Poco::Net::StreamSocket &socket);
  ~ProxyConnection();
  int getId();
  void run();
  void data(Poco::BasicFIFOBuffer<char> &data);
  void close();
private:
  Poco::Logger &logger;
  Proxy &proxy;
  int id;
  volatile bool closed;
};

class ProxyConnectionFactory: public Poco::Net::TCPServerConnectionFactory
{
public:
  ProxyConnectionFactory(Proxy &proxy);
  Poco::Net::TCPServerConnection *createConnection(const Poco::Net::StreamSocket &socket);
private:
  Proxy &proxy;
};

#endif
