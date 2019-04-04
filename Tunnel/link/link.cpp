#include "link.h"

namespace Link {

Link::Link(int uploadQueueCapacity, int downloadQueueCapacity, const std::string &password) :
    logger(Poco::Logger::get("link")), uploadQueue(uploadQueueCapacity), downloadQueue(downloadQueueCapacity)
{
  random.seed();
  cipher = Poco::Crypto::CipherFactory::defaultFactory().createCipher(Poco::Crypto::CipherKey("aes-128-cbc", password, "T68#ysmPZLF+7fsg"));
}

Link::~Link()
{
}

bool Link::receive(Message &message, Poco::BasicFIFOBuffer<char> &data)
{
  Poco::BasicFIFOBuffer<char> packet(sizeof(Message) + 1024);
  Poco::FIFOBufferStream packetStream(packet);
  if (!receive(packetStream))
  {
    return false;
  }
  packetStream.read((char *) &message, sizeof(Message));
  if (!packetStream.eof())
  {
    Poco::FIFOBufferStream dataStream(data);
    Poco::StreamCopier::copyStream(packetStream, dataStream);
    dataStream.flush();
  }
  return true;
}

bool Link::receive(std::ostream &data)
{
  std::string line;
  if (downloadQueue.pop(line, 100))
  {
    std::stringstream stream(line);
    Poco::Base64Decoder decoder(stream);
    try
    {
      cipher->decrypt(decoder, data);
    }
    catch (const Poco::Exception &error)
    {
      logger.error("decryption error " + error.displayText());
      return false;
    }
    data.flush();
    return true;
  }
  return false;
}

void Link::send(Message &message, Poco::BasicFIFOBuffer<char> &data)
{
  message.randomSalt = (int) random.next(0xffffffff);
  Poco::BasicFIFOBuffer<char> packet(sizeof(Message) + 1500);
  Poco::FIFOBufferStream packetStream(packet);
  packetStream.write((char *) &message, sizeof(Message));
  Poco::FIFOBufferStream dataStream(data);
  Poco::StreamCopier::copyStream(dataStream, packetStream);
  packetStream.flush();
  send(packetStream);
}

void Link::send(std::istream &data)
{
  std::stringstream stream;
  Poco::Base64Encoder encoder(stream);
  encoder.rdbuf()->setLineLength(0);
  cipher->encrypt(data, encoder);
  encoder.close();
  uploadQueue.push(stream.str());
}

}
