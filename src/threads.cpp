#include <thread>
#include <mutex>
#include <queue>
#include <iostream>
#include <condition_variable>

using namespace std;

mutex mtx;
queue<int> que;
condition_variable cv;
bool finished = false;

void producer()
{
    for (int i = 0; i != 10; ++i) {
        unique_lock<mutex> lock(mtx);
        que.push(i);
        lock.unlock();
        cout << "producer : " << i << endl;
        cv.notify_one();
    }
    {
        lock_guard<mutex> lock(mtx);
        finished = true;
    }
    cv.notify_one();
}

void consumer()
{
    while (true) {
        unique_lock<mutex> lock(mtx);
        this_thread::sleep_for(100ms);
        cv.wait(lock, []()->bool{return !que.empty() || finished;});
        if (!que.empty()) {
            int i = que.front();
            que.pop();
            lock.unlock();
            cout << "consumer : " << i << endl;
        } else if (finished) {
            lock.unlock();
            break;
        }
    }
}

void producer_consumer_model()
{
    thread t1(producer), t2(consumer);
    t1.join();
    t2.join();
    return;
}
