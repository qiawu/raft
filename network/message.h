#ifndef MESSAGE_H
#define MESSAGE_H 

#include "network/raft.grpc.pb.h"
#include "node_address.h"

namespace raft {
  enum class MessageType {
    ElectionTimeout,
    ClientReq,
    ClientResp,
    VoteReq,
    VoteResp,
    ReplicateEntryReq,
    ReplicateEntryResp,
    General
  };

  enum class Membership {
    Leader,
    Follower,
    Candidate
  };

  struct Message {
    Message(const std::string& msg, MessageType type): 
      msg_(msg), type_(type) {}

    std::string msg_;
    MessageType type_;

    virtual ~Message() {};
  };

  struct GeneralMessage : public Message {
    GeneralMessage(const std::string& msg): 
      Message(msg, MessageType::General) {}
  };

  struct MembershipSwitchMessage: public Message {
    MembershipSwitchMessage(const std::string& msg, Membership target): 
      Message(msg, MessageType::ElectionTimeout), target_(target) {}

    Membership target_;
  };

  struct ElectionTimeoutMessage: public Message {
    ElectionTimeoutMessage(const std::string& msg): 
      Message(msg, MessageType::ElectionTimeout) {}
  };

  struct ClientRequestMessage : public Message {
    ClientRequestMessage(const std::string& msg): 
      Message(msg, MessageType::ClientReq) {}

    ClientRequestMessage(const ClientRequest& proto_msg): 
      Message(proto_msg.message(), MessageType::ClientReq) {}
  };
  struct ClientResponseMessage: public Message {
    ClientResponseMessage(const std::string& msg): 
      Message(msg, MessageType::ClientResp) {}

    ClientResponseMessage(const ClientResponse& proto_msg): 
      Message(proto_msg.message(), MessageType::ClientReq) {}
  };

  struct VoteRequestMessage : public Message {
    VoteRequestMessage(
        const std::string& msg, 
        const std::string& node_name, 
        uint32_t cur_term, 
        uint32_t cur_index): 
      Message(msg, MessageType::VoteReq), 
      node_name_(node_name), 
      cur_term_(cur_term), 
      cur_index_(cur_index) {}

    VoteRequestMessage(const VoteRequest& proto_msg):
      Message(proto_msg.message(), MessageType::VoteReq), 
      node_name_(proto_msg.node_name()), 
      cur_term_(proto_msg.cur_term()), 
      cur_index_(proto_msg.cur_index()) {}

    std::string node_name_;
    uint32_t cur_term_;
    uint32_t cur_index_;
  };

  struct VoteResponseMessage: public Message {
    VoteResponseMessage(
        const std::string& msg, 
        std::string vote_name,
        uint32_t vote_term,
        uint32_t vote_index,
        bool grant_or_deny): 
      Message(msg, MessageType::VoteResp), 
      vote_name_(vote_name),
      vote_term_(vote_term),
      vote_index_(vote_index),
      grant_or_deny_(grant_or_deny) {}

    VoteResponseMessage(const VoteResponse& proto_msg):
      Message(proto_msg.message(), MessageType::VoteResp), 
      vote_name_(proto_msg.vote_name()),
      vote_term_(proto_msg.vote_term()),
      vote_index_(proto_msg.vote_index()),
      grant_or_deny_(proto_msg.grant_or_deny()) {}

    std::string vote_name_;
    uint32_t vote_term_;
    uint32_t vote_index_;
    bool grant_or_deny_;
  };
  struct ReplicateRequestMessage: public Message {
    ReplicateRequestMessage(
        const std::string& msg, 
        uint32_t cur_term, 
        uint32_t cur_index, 
        uint32_t prev_term, 
        uint32_t prev_index, 
        uint32_t commited_term, 
        uint32_t commited_index): 
      Message(msg, MessageType::ReplicateEntryReq), 
      cur_term_(cur_term), 
      cur_index_(cur_index), 
      prev_term_(prev_term), 
      prev_index_(prev_index), 
      commited_term_(commited_term), 
      commited_index_(commited_index) {}

    ReplicateRequestMessage(const ReplicateRequest& proto_msg):
      Message(proto_msg.message(), MessageType::ReplicateEntryReq), 
      leader_name_(proto_msg.leader_name()), 
      cur_term_(proto_msg.cur_term()), 
      cur_index_(proto_msg.cur_index()), 
      prev_term_(proto_msg.prev_term()), 
      prev_index_(proto_msg.prev_index()), 
      commited_term_(proto_msg.commited_term()), 
      commited_index_(proto_msg.commited_index()) {}

    std::string leader_name_;
    uint32_t cur_term_;
    uint32_t cur_index_;
    uint32_t prev_term_;
    uint32_t prev_index_;
    uint32_t commited_term_;
    uint32_t commited_index_;
  };

  struct ReplicateResponseMessage: public Message {
    ReplicateResponseMessage(
        const std::string& msg, 
        uint32_t append_term, 
        uint32_t append_index): 
      Message(msg, MessageType::ReplicateEntryResp), 
      append_term_(append_term), 
      append_index_(append_index) {}

    ReplicateResponseMessage(const ReplicateResponse& proto_msg):
      Message(proto_msg.message(), MessageType::ReplicateEntryResp), 
      leader_name_(proto_msg.leader_name()), 
      append_term_(proto_msg.append_term()), 
      append_index_(proto_msg.append_index()),
      succeed_or_fail_(proto_msg.succeed_or_fail()) {}

    std::string leader_name_;
    uint32_t append_term_;
    uint32_t append_index_;
    bool succeed_or_fail_;
  };
}

#endif /* MESSAGE_H */
