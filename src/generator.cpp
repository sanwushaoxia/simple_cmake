#include <iostream>
#include <coroutine> // 协程
#include <concepts> // convertible_to
#include <exception> // 用于异常处理

// 1. 定义 Generator 类型
template<typename T>
class Generator {
public:
    // 2. 嵌套的 promise_type，控制协程行为
    struct promise_type {
        T value_;                      // 存储 co_yield 的值
        std::exception_ptr exception_; // 存储未处理的异常

        /* 通过 promise_type 获取协程句柄 */
        /*
        调用时机：协程刚被调用时，但在执行任何用户代码之前。
        作用：构造并返回协程的“句柄对象”（如 Task、Generator 等），供调用者使用。
        谁调用：编译器在协程入口处调用。
        返回值：通常是 std::coroutine_handle<promise_type> 或封装它的类型（如 Task）。
        注意：此函数不能抛出异常（否则程序终止）。
        */
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        /* 表示协程创建后挂起, 这样第一次调用 next 才会开始执行, 若返回 suspend_never, 则协程创建后会执行到 co_yield */
        /*
        调用时机：在 get_return_object() 之后，用户代码（协程体）执行之前。
        作用：决定协程是否立即开始执行，还是先挂起等待外部 resume。
        返回值：一个 Awaitable 对象（如 std::suspend_always 或 std::suspend_never）。
        */
        std::suspend_always initial_suspend() noexcept { return {}; }
        /* 表示协程结束后挂起, 保证安全访问完最终状态后再 destroy */
        /*
        调用时机：协程即将结束时（无论是正常返回、异常还是被 destroy）。
        作用：决定协程结束后是否挂起，以便进行清理或通知。
        通常返回 std::suspend_always{}，以便调用者有机会析构协程（如调用 .destroy()）
        若返回 suspend_never，协程会自动销毁，但可能无法安全访问 promise 数据
        重要：在此之后，协程不能再被 resume！
        */
        std::suspend_always final_suspend() noexcept { return {}; }

        /* 当协程内部抛出未捕获的异常, 会调用该函数 */
        /*
        调用时机：当协程体中抛出未被捕获的异常时。
        作用：捕获异常并存储（通常用 std::current_exception()）。
        */
        void unhandled_exception() { exception_ = std::current_exception(); }

        /* 当协程中执行 co_yield expr 时, 会调用该函数 */
        template<std::convertible_to<T> From>
        std::suspend_always yield_value(From&& from) { // From&& 是通用引用, 具体是左值引用还是右值引用取决于实参
            value_ = std::forward<From>(from); // 完美转发引用类型
            return {};
        }

        /*
        调用时机：当协程执行到 co_return; 时
        作用：存储返回值（如果有的话）。
        return_void 和 return_value 不能同时存在，否则编译错误。
        */
        void return_void() {}

        /*
        调用时机：当协程执行到 co_return expr; 时
        */
        // return_value
    };

    using handle_type = std::coroutine_handle<promise_type>;

    /* 构造函数 */
    explicit Generator(handle_type h) : coro_(h) {}
    /* 析构函数 */
    ~Generator() { if (coro_) coro_.destroy(); } // 销毁协程帧

    /* 拷贝构造函数 */
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    /* 移动构造函数 */
    Generator(Generator&& other) noexcept : coro_(other.coro_) {
        other.coro_ = nullptr;
    }

    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (coro_) coro_.destroy();
            coro_ = other.coro_;
            other.coro_ = nullptr;
        }
        return *this;
    }

    // 获取生成的 int 的值
    T value() const { return coro_.promise().value_; }

    // 恢复协程执行（移动到下一个 yield）
    bool next() {
        coro_.resume(); // 恢复协程执行
        if (coro_.done()) { // 判断协程执行是否结束, 即是否执行到 final_suspend
            if (coro_.promise().exception_) // 获取 struct promise_type 中的 exception_
                std::rethrow_exception(coro_.promise().exception_); // 抛出 exception_ 记录的错误
            return false;
        }
        return true;
    }

private:
    handle_type coro_; // 协程句柄, 管理协程状态
};

/* 协程函数 */
Generator<int> range(int beg, int end) {
    for (int i = beg; i < end; ++i) {
        co_yield i; // 每次生成一个 int 并挂起, 实际调用 promise_type::yield_value
    }
}

int generator() {
    auto gen = range(1, 5); // 创建生成器
    while (gen.next()) { // 执行协程直到 co_yield
        std::cout << gen.value() << std::endl; // 获取生成的 int 的值
    }
    return 0;
}