#include "relay_connection.h"

RelayConnection::RelayConnection(Relay &relay, int id) :
    logger(Poco::Logger::get("connection-" + std::to_string(id))), relay(relay), runnable(*this, &RelayConnection::loop)
{
  this->id = id;
  closed = false;
  dead = false;
}

RelayConnection::~RelayConnection()
{
}

int RelayConnection::getId()
{
  return id;
}

bool RelayConnection::isDead()
{
  return dead;
}

const Poco::Timestamp &RelayConnection::getLastActivity()
{
  return lastActivity;
}

void RelayConnection::open(int port)
{
  try
  {
    socket.connect(Poco::Net::SocketAddress("127.0.0.1", port), Poco::Timespan(1, 0));
    socket.setKeepAlive(true);
    socket.setNoDelay(true);
    thread.start(runnable);
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unable to connect " + error.displayText());
    dead = true;
  }
}

void RelayConnection::data(Poco::BasicFIFOBuffer<char> &data)
{
  try
  {
    socket.sendBytes(data);
    lastActivity.update();
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unable to send " + error.displayText());
    relay.sendClose(id);
    dead = true;
  }
}

void RelayConnection::close()
{
  closed = true;
  try
  {
    socket.shutdownReceive();
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unable to shutdown " + error.displayText());
  }
  try
  {
    thread.join();
    socket.close();
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unable to close " + error.displayText());
    relay.sendClose(id);
    dead = true;
  }
}

void RelayConnection::loop()
{
  try
  {
    Poco::BasicFIFOBuffer<char> data(1024);
    while (!closed)
    {
      int bytes = socket.receiveBytes(data);
      if (bytes <= 0)
      {
        break;
      }
      relay.sendData(id, data);
      lastActivity.update();
    }
    if (!closed)
    {
      relay.sendClose(id);
    }
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unable to receive " + error.displayText());
    relay.sendClose(id);
    dead = true;
  }
}
