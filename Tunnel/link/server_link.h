#ifndef LINK_SERVER_LINK_H_
#define LINK_SERVER_LINK_H_

#include <string>
#include <Poco/Mutex.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "link.h"

namespace Link {

class ServerLink: public Link
{
public:
  ServerLink(const std::string &password, int httpPort, const std::string &baseUrl);
  ~ServerLink();
  void start();
  void stop();
private:
  class Handler : public Poco::Net::HTTPRequestHandler
  {
  public:
    Handler(ServerLink &serverLink);
    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
  private:
    ServerLink &serverLink;
  };
  class HandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
  {
  public:
    HandlerFactory(ServerLink &serverLink);
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &request);
  private:
    ServerLink &serverLink;
  };
  std::string baseUrl;
  Poco::Net::HTTPServer server;
  Poco::Mutex mutex;
  void get(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
};

}

#endif
