#ifndef MESSAGE_H
#define MESSAGE_H 

namespace raft {
  struct Message {
    std::string msg_;
  };
  struct ClientRequestMessage : public Message {
  };
  struct VoteRequestMessage : public Message {
    std::string ip;
    uint32_t cur_term;
    uint32_t cur_index;
  };
  struct AppendEntryMessage : public Message {
    std::string ip;
    uint32_t cur_term;
    uint32_t cur_index;
    uint32_t prev_term;
    uint32_t prev_index;
    uint32_t commited_term;
    uint32_t commited_index;
  };
}

#endif /* MESSAGE_H */
