
namespace raft {
  class ReplicateLogManager {
    public:
      ReplicateLogManager();
      void AddLog();
      void CommitLog();
      void IsMoreUpdateToDate();
      void PersistLog();
  };
}
