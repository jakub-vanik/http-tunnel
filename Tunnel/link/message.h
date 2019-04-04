#ifndef LINK_MESSAGE_H_
#define LINK_MESSAGE_H_

namespace Link {

#pragma pack(push, 1)

class Message
{
public:
  int randomSalt;
  int connectionId;
  enum MessageType
  {
    RESET,
    OPEN,
    CLOSE,
    DATA
  };
  MessageType messageType;
};

#pragma pack(pop)

}

#endif
