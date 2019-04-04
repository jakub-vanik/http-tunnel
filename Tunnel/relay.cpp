#include "relay.h"

Relay::Relay(int port, int timeout, const std::string &password, int httpPort, const std::string &baseUrl) :
    logger(Poco::Logger::get("relay")), runnable(*this, &Relay::loop), link(password, httpPort, baseUrl)
{
  this->port = port;
  this->timeout = timeout;
}

Relay::~Relay()
{
}

void Relay::start()
{
  logger.information("starting");
  running = true;
  thread.start(runnable);
  link.start();
}

void Relay::stop()
{
  logger.information("stopping");
  running = false;
  thread.join();
  link.stop();
}

void Relay::sendData(int id, Poco::FIFOBuffer &data)
{
  logger.information("server sent " + std::to_string(data.used()) + " bytes connection " + std::to_string(id));
  Link::Message message;
  message.connectionId = id;
  message.messageType = Link::Message::DATA;
  link.send(message, data);
}

void Relay::sendClose(int id)
{
  logger.information("server closed connection " + std::to_string(id));
  Link::Message message;
  message.connectionId = id;
  message.messageType = Link::Message::CLOSE;
  Poco::BasicFIFOBuffer<char> data(0);
  link.send(message, data);
}

void Relay::loop()
{
  try
  {
    while (running)
    {
      Link::Message message;
      Poco::BasicFIFOBuffer<char> data(1024);
      if (link.receive(message, data))
      {
        if (message.messageType == Link::Message::RESET)
        {
          logger.information("client reseted");
          for (RelayConnection *connection : connections)
          {
            connection->close();
            delete connection;
          }
          connections.clear();
          continue;
        }
        if (message.messageType == Link::Message::OPEN)
        {
          bool found = false;
          for (RelayConnection *connection : connections)
          {
            if (connection->getId() == message.connectionId)
            {
              found = true;
              break;
            }
          }
          if (found)
          {
            logger.error("duplicate connection " + std::to_string(message.connectionId));
            continue;
          }
          logger.information("client opened connection " + std::to_string(message.connectionId));
          RelayConnection *connection = new RelayConnection(*this, message.connectionId);
          connections.push_back(connection);
          connection->open(port);
          connection->data(data);
          continue;
        }
        bool found = false;
        for (RelayConnection *connection : connections)
        {
          if (connection->getId() == message.connectionId)
          {
            found = true;
            if (message.messageType == Link::Message::CLOSE)
            {
              logger.information("client closed connection " + std::to_string(message.connectionId));
              connection->close();
              delete connection;
              connections.erase(std::remove(connections.begin(), connections.end(), connection), connections.end());
              break;
            }
            if (message.messageType == Link::Message::DATA)
            {
              logger.information("client sent " + std::to_string(data.used()) + " bytes connection " + std::to_string(message.connectionId));
              connection->data(data);
              break;
            }
            logger.error("invalid message type");
            break;
          }
        }
        if (!found)
        {
          if (message.messageType != Link::Message::CLOSE)
          {
            sendClose(message.connectionId);
            logger.warning("connection " + std::to_string(message.connectionId) + " is not registered");
          }
        }
      }
      else
      {
        cleanupConnections();
      }
    }
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unexpected error " + error.displayText());
  }
}

void Relay::cleanupConnections()
{
  Poco::Timestamp threshold = Poco::Timestamp() - Poco::Timespan(timeout, 0);
  std::vector<RelayConnection *>::iterator it = connections.begin();
  while (it != connections.end())
  {
    RelayConnection *connection = *it;
    if (connection->isDead() || connection->getLastActivity() < threshold)
    {
      logger.warning("connection " + std::to_string(connection->getId()) + " timed out");
      connection->close();
      delete connection;
      it = connections.erase(it);
    }
    else
    {
      ++it;
    }
  }
}
