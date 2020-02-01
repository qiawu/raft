
#include "Configuration.h"

#include <fstream>
#include <iostream>

#include "logger.h"
#include "utils.h"

raft::Status raft::Configuration::Parse() {
  // TODO: handle file exception
  std::ifstream fs;
  fs.open(conf_path_);
  if (!fs) {
    std::string msg = Utils::StringFormat("failed to open conf: %s", conf_path_.c_str());
    return Status::Error(msg);
  }

  std::string line;
  uint32_t sect_cnt = 0;
  for (std::string line; std::getline(fs, line);) {
    line = Utils::Trim(line);
    if (line.empty()) {
      continue;
    }
    // skip comments
    if (line[0] == '#') {
      continue;
    }

    uint32_t len = line.size();
    if (line[0] == '[' && line[len - 1] == ']') {
      std::string sect_name = line.substr(1, len - 2);
      sections_.push_back(sect_name);
      props_.push_back(std::map<std::string, std::string>());
      sect_cnt++;
    } else {
      auto kvs = Utils::Split(line, ": ");
      if (kvs.size() != 2 || sect_cnt <= 0) {
        std::string msg = Utils::StringFormat("invalid config format: %s, conf: %s", line.c_str(), conf_path_.c_str());
        return Status::InvalidFormat(msg);
      }
      props_[sect_cnt - 1].insert(make_pair(kvs[0], kvs[1]));
    }
  }
  fs.close();
  return Status::OK();
}

void raft::Configuration::PrettyPrint() {
  int n = sections_.size();
  std::cout << "total: " << n << std::endl;
  for (int i = 0; i < n; ++i) {
    std::cout << "section: " << sections_[i] << std::endl;
    for (auto kv : props_[i]) {
      std::cout << kv.first << ":" << kv.second << std::endl;
    }
  }
}
