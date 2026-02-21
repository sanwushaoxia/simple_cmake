#include <iostream>
#include <memory>

using namespace std;

class B;
class A {
public:
    std::shared_ptr<B> b_ptr;
    ~A() { cout << "A destroyed\n"; }
};

class B {
public:
    std::weak_ptr<A> a_ptr;
    ~B() { cout << "B destroyed\n"; }
};

int weakPtr() {
    auto a = make_shared<A>();
    auto b = make_shared<B>();
    
    a->b_ptr = b;  // A 持有 B
    b->a_ptr = a;  // B 持有 A → 形成循环引用！

    auto b_a_ptr = b->a_ptr.lock();
    if (b_a_ptr) {
        cout << a << ' ' << b_a_ptr << endl;
    }

    // 此时：
    cout << a.use_count() << endl;
    cout << b.use_count() << endl;

    return 0; // a 和 b 离开作用域，use_count 降为 1，但不会归零！
}