#ifndef LINK_CLIENT_LINK_H_
#define LINK_CLIENT_LINK_H_

#include <string>
#include <Poco/Exception.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include "link.h"

namespace Link {

class ClientLink : public Link
{
public:
  ClientLink(const std::string &password, const std::string &hostName, int httpPort, const std::string &baseUrl);
  ~ClientLink();
  void start();
  void stop();
  void setIdle(bool idle);
private:
  std::string baseUrl;
  volatile bool idle;
  Poco::Net::HTTPClientSession session;
  Poco::RunnableAdapter<ClientLink> runnable;
  Poco::Thread thread;
  volatile bool running;
  void loop();
  std::string get(const std::string &text);
};

}

#endif
