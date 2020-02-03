#ifndef MESSAGE_H
#define MESSAGE_H 

#include "node_address.h"
namespace raft {
  struct Message {
    Message(const std::string& msg): msg_(msg) {}
    std::string msg_;
  };
  struct ClientRequestMessage : public Message {
    ClientRequestMessage(const std::string& msg): Message(msg) {}
  };
  struct ClientResponseMessage: public Message {
    ClientResponseMessage(const std::string& msg): Message(msg) {}
  };
  struct VoteRequestMessage : public Message {
    VoteRequestMessage(const std::string& msg, uint32_t cur_term, uint32_t cur_index): Message(msg), cur_term_(cur_term), cur_index_(cur_index) {}
    uint32_t cur_term_;
    uint32_t cur_index_;
  };
  struct VoteResponseMessage: public Message {
    VoteResponseMessage(const std::string& msg, bool grant_or_deny): Message(msg), grant_or_deny_(grant_or_deny) {}
    bool grant_or_deny_;
  };
  struct AppendEntryMessage : public Message {
    AppendEntryMessage(const std::string& msg, uint32_t cur_term, uint32_t cur_index, uint32_t prev_term, uint32_t prev_index, uint32_t commited_term, uint32_t commited_index): Message(msg), cur_term_(cur_term), cur_index_(cur_index), prev_term_(prev_term), prev_index_(prev_index), commited_term_(commited_term), commited_index_(commited_index) {}
    uint32_t cur_term_;
    uint32_t cur_index_;
    uint32_t prev_term_;
    uint32_t prev_index_;
    uint32_t commited_term_;
    uint32_t commited_index_;
  };
}

#endif /* MESSAGE_H */
