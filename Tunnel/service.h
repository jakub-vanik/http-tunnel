#ifndef SERVICE_H_
#define SERVICE_H_

#include <string>
#include <vector>
#include <Poco/AsyncChannel.h>
#include <Poco/AutoPtr.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Logger.h>
#include <Poco/PatternFormatter.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/ServerApplication.h>

#include "proxy.h"
#include "relay.h"

class Service : public Poco::Util::ServerApplication
{
public:
  Service();
  ~Service();
private:
  bool proxy;
  int proxyPort;
  bool relay;
  int relayPort;
  int timeout;
  std::string password;
  std::string hostName;
  int httpPort;
  std::string baseUrl;
  int verbosity;
  int main(const std::vector<std::string> &args);
  void defineOptions(Poco::Util::OptionSet &options);
  void handleOption(const std::string &name, const std::string &value);
  void setupLogger();
  void runProxy();
  void runRelay();
};

#endif
