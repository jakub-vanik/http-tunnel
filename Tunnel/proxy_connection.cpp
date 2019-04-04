#include "proxy_connection.h"

ProxyConnection::ProxyConnection(Proxy &proxy, const Poco::Net::StreamSocket &socket) :
    TCPServerConnection(socket), logger(Poco::Logger::get("connection-" + std::to_string(id))), proxy(proxy)
{
  id = proxy.registerConnection(this);
  closed = false;
}

ProxyConnection::~ProxyConnection()
{
  proxy.unregisterConnection(this);
}

int ProxyConnection::getId()
{
  return id;
}

void ProxyConnection::run()
{
  bool opened = false;
  try
  {
    socket().setKeepAlive(true);
    socket().setNoDelay(true);
    Poco::BasicFIFOBuffer<char> data(1024);
    while (!closed)
    {
      int bytes = socket().receiveBytes(data);
      if (bytes <= 0)
      {
        break;
      }
      if (opened)
      {
        proxy.sendData(id, data);
      }
      else
      {
        proxy.sendOpen(id, data);
        opened = true;
      }
    }
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unable to receive " + error.displayText());
  }
  if (opened)
  {
    proxy.sendClose(id);
  }
}

void ProxyConnection::data(Poco::BasicFIFOBuffer<char> &data)
{
  try
  {
    socket().sendBytes(data);
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unable to send " + error.displayText());
  }
}

void ProxyConnection::close()
{
  closed = true;
  try
  {
    socket().shutdownReceive();
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unable to shutdown " + error.displayText());
  }
}

ProxyConnectionFactory::ProxyConnectionFactory(Proxy &proxy) :
    proxy(proxy)
{
}

Poco::Net::TCPServerConnection *ProxyConnectionFactory::createConnection(const Poco::Net::StreamSocket &socket)
{
  return new ProxyConnection(proxy, socket);
}
