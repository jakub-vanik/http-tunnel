#include "service.h"

Service::Service()
{
  proxy = false;
  proxyPort = 1080;
  relay = false;
  relayPort = 1080;
  timeout = 300;
  password = "yK!xhk+S*59fnJua";
  hostName = "127.0.0.1";
  httpPort = 8080;
  baseUrl = "/";
  verbosity = 0;
}

Service::~Service()
{
}

int Service::main(const std::vector<std::string> &args)
{
  setupLogger();
  if (proxy)
  {
    runProxy();
    return 0;
  }
  if (relay)
  {
    runRelay();
    return 0;
  }
  return 0;
}

void Service::defineOptions(Poco::Util::OptionSet &options)
{
  options.addOption(Poco::Util::Option("proxy", "P", "start proxy", false).argument("value"));
  options.addOption(Poco::Util::Option("relay", "R", "start relay", false).argument("value"));
  options.addOption(Poco::Util::Option("timeout", "t", "timeout", false).argument("value"));
  options.addOption(Poco::Util::Option("password", "s", "password", false).argument("value"));
  options.addOption(Poco::Util::Option("host", "h", "host name", false).argument("value"));
  options.addOption(Poco::Util::Option("port", "p", "http port", false).argument("value"));
  options.addOption(Poco::Util::Option("base", "b", "base URL", false).argument("value"));
  options.addOption(Poco::Util::Option("verbosity", "v", "verbosity", false).argument("value"));
}

void Service::handleOption(const std::string &name, const std::string &value)
{
  if (name == "proxy")
  {
    proxy = true;
    proxyPort = std::stoi(value);
  }
  if (name == "relay")
  {
    relay = true;
    relayPort = std::stoi(value);
  }
  if (name == "timeout")
  {
    timeout = std::stoi(value);
  }
  if (name == "password")
  {
    password = value;
  }
  if (name == "host")
  {
    hostName = value;
  }
  if (name == "port")
  {
    httpPort = std::stoi(value);
  }
  if (name == "base")
  {
    baseUrl = value;
  }
  if (name == "verbosity")
  {
    verbosity = std::stoi(value);
  }
}

void Service::setupLogger()
{
  Poco::AutoPtr<Poco::ConsoleChannel> consoleChannel(new Poco::ConsoleChannel());
  Poco::AutoPtr<Poco::PatternFormatter> paternFormatter(new Poco::PatternFormatter());
  paternFormatter->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: %t");
  Poco::AutoPtr<Poco::FormattingChannel> formattingChannel(new Poco::FormattingChannel(paternFormatter, consoleChannel));
  Poco::AutoPtr<Poco::AsyncChannel> asyncChannel(new Poco::AsyncChannel(formattingChannel));
  Poco::Logger::root().setChannel(asyncChannel);
  Poco::Logger::root().setLevel(verbosity);
}

void Service::runProxy()
{
  Proxy *proxy = new Proxy(proxyPort, password, hostName, httpPort, baseUrl);
  proxy->start();
  waitForTerminationRequest();
  proxy->stop();
  delete proxy;
}

void Service::runRelay()
{
  Relay *relay = new Relay(relayPort, timeout, password, httpPort, baseUrl);
  relay->start();
  waitForTerminationRequest();
  relay->stop();
  delete relay;
}
