#ifndef LINK_LINK_H_
#define LINK_LINK_H_

#include <iosfwd>
#include <sstream>
#include <string>
#include <Poco/Base64Decoder.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Crypto/Cipher.h>
#include <Poco/Crypto/CipherFactory.h>
#include <Poco/Crypto/CipherKey.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/FIFOBufferStream.h>
#include <Poco/Logger.h>
#include <Poco/Random.h>
#include <Poco/StreamCopier.h>
#include "message.h"
#include "queue.h"

namespace Link {

class Link
{
public:
  Link(int uploadQueueCapacity, int downloadQueueCapacity, const std::string &password);
  virtual ~Link();
  virtual void start() = 0;
  virtual void stop() = 0;
  bool receive(Message &message, Poco::BasicFIFOBuffer<char> &data);
  bool receive(std::ostream &data);
  void send(Message &message, Poco::BasicFIFOBuffer<char> &data);
  void send(std::istream &data);
protected:
  Poco::Logger &logger;
  Poco::Random random;
  Queue uploadQueue;
  Queue downloadQueue;
  Poco::Crypto::Cipher *cipher;
};

}

#endif
