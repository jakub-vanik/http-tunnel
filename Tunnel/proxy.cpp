#include "proxy.h"

Proxy::Proxy(int port, const std::string &password, const std::string &hostName, int httpPort, const std::string &baseUrl, const std::string &fakeHost) :
    logger(Poco::Logger::get("proxy")), threadPool(64, 64), socket(port), server(new ProxyConnectionFactory(*this), threadPool, socket), runnable(*this, &Proxy::loop), link(password, hostName, httpPort, baseUrl, fakeHost)
{
  nextId = 0;
}

Proxy::~Proxy()
{
}

void Proxy::start()
{
  logger.information("starting");
  link.start();
  sendReset();
  server.start();
  running = true;
  thread.start(runnable);
}

void Proxy::stop()
{
  logger.information("stopping");
  server.stop();
  link.stop();
  running = false;
  thread.join();
}

int Proxy::registerConnection(ProxyConnection *connection)
{
  connectionsLock.lock();
  if (connections.empty())
  {
    link.setIdle(false);
  }
  connections.push_back(connection);
  int id = generateId();
  connectionsLock.unlock();
  return id;
}

void Proxy::unregisterConnection(ProxyConnection *connection)
{
  connectionsLock.lock();
  connections.erase(std::remove(connections.begin(), connections.end(), connection), connections.end());
  if (connections.empty())
  {
    link.setIdle(true);
  }
  connectionsLock.unlock();
}

void Proxy::sendReset()
{
  Link::Message message;
  message.connectionId = 0;
  message.messageType = Link::Message::RESET;
  Poco::BasicFIFOBuffer<char> data(0);
  link.send(message, data);
}

void Proxy::sendOpen(int id, Poco::FIFOBuffer &data)
{
  logger.information("client opened connection " + std::to_string(id));
  Link::Message message;
  message.connectionId = id;
  message.messageType = Link::Message::OPEN;
  link.send(message, data);
}

void Proxy::sendData(int id, Poco::FIFOBuffer &data)
{
  logger.information("client sent " + std::to_string(data.used()) + " bytes connection " + std::to_string(id));
  Link::Message message;
  message.connectionId = id;
  message.messageType = Link::Message::DATA;
  link.send(message, data);
}

void Proxy::sendClose(int id)
{
  logger.information("client closed connection " + std::to_string(id));
  Link::Message message;
  message.connectionId = id;
  message.messageType = Link::Message::CLOSE;
  Poco::BasicFIFOBuffer<char> data(0);
  link.send(message, data);
}

void Proxy::loop()
{
  while (running)
  {
    Link::Message message;
    Poco::BasicFIFOBuffer<char> data(1024);
    if (link.receive(message, data))
    {
      bool found = false;
      connectionsLock.lock();
      for (ProxyConnection *connection : connections)
      {
        if (connection->getId() == message.connectionId)
        {
          found = true;
          if (message.messageType == Link::Message::CLOSE)
          {
            logger.information("server closed connection " + std::to_string(message.connectionId));
            connection->close();
            break;
          }
          if (message.messageType == Link::Message::DATA)
          {
            logger.information("server sent " + std::to_string(data.used()) + " bytes connection " + std::to_string(message.connectionId));
            connection->data(data);
            break;
          }
          logger.error("invalid message type");
          break;
        }
      }
      connectionsLock.unlock();
      if (!found)
      {
        if (message.messageType != Link::Message::CLOSE)
        {
          logger.warning("connection " + std::to_string(message.connectionId) + " is not registered");
        }
      }
    }
  }
}

int Proxy::generateId()
{
  while (true)
  {
    int id = nextId;
    nextId = (nextId + 1) & 0xffff;
    bool found = false;
    for (ProxyConnection *connection : connections)
    {
      if (connection->getId() == id)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      return id;
    }
  }
}
