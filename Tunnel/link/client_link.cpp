#include "client_link.h"

namespace Link {

ClientLink::ClientLink(const std::string &password, const std::string &hostName, int httpPort, const std::string &baseUrl, const std::string &fakeHost) :
    Link(10, 200, password), session(hostName, httpPort), runnable(*this, &ClientLink::loop)
{
  this->baseUrl = baseUrl;
  this->fakeHost = fakeHost;
  idle = true;
  session.setTimeout(Poco::Timespan(5, 0));
  session.setKeepAlive(true);
  session.setKeepAliveTimeout(Poco::Timespan(300, 0));
}

ClientLink::~ClientLink()
{
}

void ClientLink::start()
{
  running = true;
  thread.start(runnable);
}

void ClientLink::stop()
{
  running = false;
  thread.join();
}

void ClientLink::setIdle(bool idle)
{
  this->idle = idle;
}

void ClientLink::loop()
{
  try
  {
    int timeout = 0;
    while (running)
    {
      std::string request;
      if (uploadQueue.pop(request, timeout))
      {
        timeout = 0;
      }
      else
      {
        timeout = idle ? 60000 : 100;
      }
      std::string response = get(request);
      if (!response.empty())
      {
        std::stringstream stream;
        stream << response;
        std::string line;
        while (std::getline(stream, line, '\n'))
        {
          downloadQueue.push(line);
        }
        timeout = 0;
      }
    }
  }
  catch (const Poco::Exception &error)
  {
    logger.error("unexpected error " + error.displayText());
  }
}

std::string ClientLink::get(const std::string &text)
{
  while (true)
  {
    try
    {
      std::string path;
      path.append(baseUrl);
      path.append(text);
      Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
      if (!fakeHost.empty())
      {
        request.setHost(fakeHost);
      }
      session.sendRequest(request);
      Poco::Net::HTTPResponse response;
      std::stringstream stream;
      stream << session.receiveResponse(response).rdbuf();
      if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
      {
        return stream.str();
      }
      else
      {
        logger.error("unexpected response code " + std::to_string(response.getStatus()));
      }
    }
    catch (const Poco::Exception &error)
    {
      logger.error("request error " + error.displayText());
    }
    sleep(1);
  }
}

}
