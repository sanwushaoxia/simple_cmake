#include <iostream>
#include <vector>

using namespace std;

class Stack {
private:
    vector<int> vec;
public:
    /* 默认构造函数 */
    Stack() = default;
    // Stack(const Stack& other) = default;
    Stack(const Stack& other) {
        if (this != &other) {
            vec = other.vec;
        }
    }
    // Stack& operator=(const Stack& other) = default;
    Stack& operator=(const Stack& other) {
        if (this != &other) {
            vec = other.vec;
        }
        return *this;
    }
    // Stack(Stack&& other) noexcept = default;
    Stack(Stack&& other) noexcept {
        if (this != &other) {
            /* 移动语义 */
            vec = move(other.vec);
        }
    }
    // Stack& operator=(Stack&& other) noexcept = default;
    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) {
            vec = move(other.vec);
        }
        return *this;
    }
    /* 默认析构函数 */
    ~Stack() = default;
    void push(int x) {
        vec.emplace_back(x);
    }
    void pop() {
        if (!vec.empty()) {
            vec.pop_back();
        }
    }
    int top() const {
        return vec.empty() ? 0x7FFFFFFF : vec.back();
    }
    bool empty() const { return vec.empty(); }
    size_t size() const { return vec.size(); }
};

int constructor_class()
{
    Stack stk;
    stk.push(1);
    cout << "stk top val " << stk.top() << endl; // 1
    stk.push(2);
    Stack stk1(stk);
    cout << "stk1 top val " << stk1.top() << endl; // 2
    stk1.push(3);
    Stack stk2 = stk1;
    cout << "stk2 top val " << stk2.top() << endl; // 3
    stk2.push(4);
    Stack stk3 = move(stk2);
    cout << "stk3 top val " << stk3.top() << endl; // 4
    cout << "stk2 top val " << stk2.top() << endl; // 
    return 0;
}
