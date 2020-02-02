#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H 

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class BlockingQueue {
public:
    void Push(T const& _data) {
        {
            std::lock_guard<std::mutex> lock(guard_);
            queue_.push(_data);
        }
        signal_.notify_one();
    }

    bool Empty() const {
        std::lock_guard<std::mutex> lock(guard_);
        return queue_.empty();
    }

    bool TryPop(T& _value) {
        std::lock_guard<std::mutex> lock(guard_);
        if (queue_.empty()) {
            return false;
        }

        _value = queue_.front();
        queue_.pop();
        return true;
    }

    void WaitAndPop(T& _value) {
        std::unique_lock<std::mutex> lock(guard_);
        while (queue_.empty()) {
            signal_.wait(lock);
        }

        _value = queue_.front();
        queue_.pop();
    }

    bool TryWaitAndPop(T& _value, int _milli) {
        std::unique_lock<std::mutex> lock(guard_);
        while (queue_.empty()) {
            signal_.wait_for(lock, std::chrono::milliseconds(_milli));
            return false;
        }

        _value = queue_.front();
        queue_.pop();
        return true;
    }

private:
    std::queue<T> queue_;
    mutable std::mutex guard_;
    std::condition_variable signal_;
};

#endif /* BLOCKING_QUEUE_H */
