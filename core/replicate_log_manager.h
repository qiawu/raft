
#ifndef REPLICATE_LOG_MANAGER_H
#define REPLICATE_LOG_MANAGER_H 

#include "utils/status.h"

namespace raft {
  struct LogEntryPos {
    LogEntryPos(uint32_t term, uint32_t index) : term_(term), index_(index) {}
    LogEntryPos IncreTerm() {
      return LogEntryPos(term_ + 1, index_);
    }
    LogEntryPos IncreIndex() {
      return LogEntryPos(term_, index_ + 1);
    }
    uint32_t term_;
    uint32_t index_;
  };
  class ReplicateLogManager {
    public:
      ReplicateLogManager(): cur_(0, 0) {};
      Status Init();
      Status AddLog();
      Status CommitLog();
      bool IsMoreUpdateToDate(LogEntryPos new_pos);

      Status StartNewTerm();
      LogEntryPos GetCurLogEntryPos();
      LogEntryPos GetPrevLogEntryPos();
      LogEntryPos GetLastCommittedLogEntryPos();

    private:
      Status Recover();
      Status PersistLog();

      LogEntryPos cur_;
  };
}
#endif /* REPLICATE_LOG_MANAGER_H */
