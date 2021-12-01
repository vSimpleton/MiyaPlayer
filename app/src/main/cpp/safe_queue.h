#ifndef MIYAPLAYER_SAFE_QUEUE_H
#define MIYAPLAYER_SAFE_QUEUE_H

#include <queue>
#include <pthread.h>

using namespace std;

template<typename T>
class SafeQueue {
private:
    typedef void (*ReleaseCallback)(T *); // 函数指针定义 做回调 用来释放T里面的内容的

private:
    queue<T> queue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int work;// 标记队列是否工作
    ReleaseCallback releaseCallback;

public:
    SafeQueue() {
        pthread_mutex_init(&mutex, nullptr); // 初始化互斥锁
        pthread_cond_init(&cond, nullptr); // 初始化条件变量
    }

    ~SafeQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    /**
     * 压缩数据包入队
     */
    void insertToQueue(T value) {
        pthread_mutex_lock(&mutex);
        if (work) {
            queue.push(value);
            pthread_cond_signal(&cond); // 入队后要发起通知唤醒
        } else {
            if (releaseCallback) {
                releaseCallback(&value); // 让外界释放value
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    /**
     * 压缩数据包出队
     */
    int popToQueue(T &value) {
        int ret = 0; // false
        pthread_mutex_lock(&mutex);
        while (work && queue.empty()) {
            // 若正在工作中，且队列里没有数据，则阻塞
            pthread_cond_wait(&cond, &mutex);
        }
        if (!queue.empty()) {
            value = queue.front();
            queue.pop();
            ret = 1; // true
        }
        pthread_mutex_unlock(&mutex);
        return ret;
    }

    /**
     * 设置队列是否正在工作的状态
     */
    void setWorkStatus(int workStatus) {
        pthread_mutex_lock(&mutex);
        this->work = workStatus;

        // 发起通知唤醒
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    /**
     * 清空队列中的所有数据
     */
    void clear() {
        pthread_mutex_lock(&mutex);

        unsigned int size = queue.size();
        for (int i = 0; i < size; ++i) {
            T value = queue.front();
            if (releaseCallback) {
                releaseCallback(&value);
            }
            queue.pop();
        }

        pthread_mutex_unlock(&mutex);
    }

    void setReleaseCallback(ReleaseCallback releaseCallback) {
        this->releaseCallback = releaseCallback;
    }

    int empty(){
        return queue.empty();
    }

    int size(){
        return queue.size();
    }

};

#endif //MIYAPLAYER_SAFE_QUEUE_H
