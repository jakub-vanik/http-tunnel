#include "server_link.h"

namespace Link {

ServerLink::ServerLink(const std::string &password, int httpPort, const std::string &baseUrl) :
    Link(200, 10, password), server(new HandlerFactory(*this), httpPort)
{
  this->baseUrl = baseUrl;
}

ServerLink::~ServerLink()
{
}

void ServerLink::start()
{
  server.start();
}

void ServerLink::stop()
{
  server.stop();
}

ServerLink::Handler::Handler(ServerLink &serverLink) :
    serverLink(serverLink)
{
}

void ServerLink::Handler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
{
  serverLink.get(request, response);
}

ServerLink::HandlerFactory::HandlerFactory(ServerLink &serverLink) :
    serverLink(serverLink)
{
}

Poco::Net::HTTPRequestHandler *ServerLink::HandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
{
  return new Handler(serverLink);
}

void ServerLink::get(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
{
  std::string uri = request.getURI();
  if (!uri.compare(0, baseUrl.length(), baseUrl))
  {
    if (mutex.tryLock(10))
    {
      bool emptyRequest = true;
      if (uri.length() > baseUrl.length())
      {
        std::string message = uri.substr(baseUrl.length(), uri.length() - baseUrl.length());
        downloadQueue.push(message);
        emptyRequest = false;
      }
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setChunkedTransferEncoding(true);
      std::ostream &stream = response.send();
      int linesRemains = emptyRequest ? 100 : 10;
      int timeout = 50;
      while (linesRemains > 0)
      {
        std::string message;
        if (uploadQueue.pop(message, timeout))
        {
          stream << message << "\n";
          linesRemains--;
          timeout = 5;
        }
        else
        {
          linesRemains = 0;
        }
      }
      mutex.unlock();
    }
    else
    {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setChunkedTransferEncoding(true);
      std::ostream &stream = response.send();
      stream << "error\n";
    }
  }
}

}
