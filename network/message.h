#ifndef MESSAGE_H
#define MESSAGE_H 

namespace raft {
  struct Message {
    Message(const NodeAddress& sender, const std::string& msg): sender_(sender), msg_(msg) {}
    NodeAddress sender_;
    std::string msg_;
  };
  struct ClientRequestMessage : public Message {
  };
  struct VoteRequestMessage : public Message {
    VoteRequestMessage(const NodeAddress& sender, const std::string& msg, uint32_t cur_term, uint32_t cur_index): Message(sender, msg), cur_term_(cur_term), cur_index_(cur_index) {}
    uint32_t cur_term_;
    uint32_t cur_index_;
  };
  struct AppendEntryMessage : public Message {
    AppendEntryMessage(const NodeAddress& sender, const std::string& msg, uint32_t cur_term, uint32_t cur_index, uint32_t prev_term, uint32_t prev_index, uint32_t commited_term, uint32_t commited_index): Message(sender, msg), cur_term_(cur_term), cur_index_(cur_index), prev_term_(prev_term), prev_index_(prev_index), commited_term_(commited_term), commited_index_(commited_index) {}
    uint32_t cur_term_;
    uint32_t cur_index_;
    uint32_t prev_term_;
    uint32_t prev_index_;
    uint32_t commited_term_;
    uint32_t commited_index_;
  };
}

#endif /* MESSAGE_H */
