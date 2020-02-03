#ifndef MESSAGE_H
#define MESSAGE_H 

#include "node_address.h"
namespace raft {
  enum MessageType {
    ElectionTimeout,
    ClientReq,
    ClientResp,
    VoteReq,
    VoteResp,
    AppendEntryReq,
    General
  };
  struct Message {
    Message(const std::string& msg, MessageType type): msg_(msg), type_(type) {}
    std::string msg_;
    MessageType type_;
  };
  struct GeneralMessage : public Message {
    GeneralMessage(const std::string& msg): Message(msg, MessageType::General) {}
  };
  struct ElectionTimeoutMessage: public Message {
    ElectionTimeoutMessage(const std::string& msg): Message(msg, MessageType::ElectionTimeout) {}
  };
  struct ClientRequestMessage : public Message {
    ClientRequestMessage(const std::string& msg): Message(msg, MessageType::ClientReq) {}
  };
  struct ClientResponseMessage: public Message {
    ClientResponseMessage(const std::string& msg): Message(msg, MessageType::ClientResp) {}
  };
  struct VoteRequestMessage : public Message {
    VoteRequestMessage(const std::string& msg, const std::string& node_name, uint32_t cur_term, uint32_t cur_index): Message(msg, MessageType::VoteReq), node_name_(node_name), cur_term_(cur_term), cur_index_(cur_index) {}
    std::string node_name_;
    uint32_t cur_term_;
    uint32_t cur_index_;
  };
  struct VoteResponseMessage: public Message {
    VoteResponseMessage(const std::string& msg, bool grant_or_deny): Message(msg, MessageType::VoteResp), grant_or_deny_(grant_or_deny) {}
    bool grant_or_deny_;
  };
  struct AppendEntryMessage : public Message {
    AppendEntryMessage(const std::string& msg, uint32_t cur_term, uint32_t cur_index, uint32_t prev_term, uint32_t prev_index, uint32_t commited_term, uint32_t commited_index): Message(msg, MessageType::AppendEntryReq), cur_term_(cur_term), cur_index_(cur_index), prev_term_(prev_term), prev_index_(prev_index), commited_term_(commited_term), commited_index_(commited_index) {}
    uint32_t cur_term_;
    uint32_t cur_index_;
    uint32_t prev_term_;
    uint32_t prev_index_;
    uint32_t commited_term_;
    uint32_t commited_index_;
  };
}

#endif /* MESSAGE_H */
