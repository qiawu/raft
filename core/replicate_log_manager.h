
#ifndef REPLICATE_LOG_MANAGER_H
#define REPLICATE_LOG_MANAGER_H 

#include "utils/status.h"

namespace raft {
  struct LogEntryPos {
    uint32_t index_;
    uint32_t term_;
  };
  class ReplicateLogManager {
    public:
      ReplicateLogManager();
      Status Init();
      Status AddLog();
      Status CommitLog();
      bool IsMoreUpdateToDate();

      LogEntryPos GetCurLogEntryPos();
      LogEntryPos GetPrevLogEntryPos();
      LogEntryPos GetLastCommittedLogEntryPos();

    private:
      Status Recover();
      Status PersistLog();
  };
}
#endif /* REPLICATE_LOG_MANAGER_H */
