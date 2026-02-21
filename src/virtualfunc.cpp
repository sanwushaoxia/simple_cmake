#include <iostream>

class Base {
public:
    // virtual void show() {
    //     std::cout << "Base Class" << std::endl;
    // }
    virtual void show() = 0;
};

// 基类的 show 是虚函数, 说明基类是可以实例化的, 派生类可重写该函数也可不重写, 主要用于实现默认行为并支持运行时多态
// 基类的 show 是纯虚函数, 说明基类是不可实例化的, 是抽象类, 派生类必须重写该函数, 否则也不可实例化, 也是抽象类, 主要用于提供接口规范, 强迫派生类重写

class Derived : public Base {
public:
    virtual void show() override {
        std::cout << "Derived Class" << std::endl;
    }
};

int virtualFunc()
{
    Base *p;
    Derived *q = new Derived();
    p = q;
    p->show();
    return 0;
}