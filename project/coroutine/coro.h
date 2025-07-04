#pragma once

#include <coroutine>
#include <exception>
#include <new>
#include <optional>
#include <stdexcept>
#include <utility>

#include "utils.h"


template <typename return_type = void> class task;

// promise 基类，定义了协程的初始以及结束时的调度逻辑
//可以通过continuation()记录一个协程句柄，在该协程结束时可以返回该句柄
struct promise_base {
  friend struct final_awaitable;

  // 用作 final_suspend 返回的 awaiter
  struct final_awaitable {
    auto await_ready() const noexcept -> bool { return false; }

    // 确保协程的栈式调用正确返回
    template <typename promise_type>
    auto await_suspend(std::coroutine_handle<promise_type> coroutine) noexcept
        -> std::coroutine_handle<> {
        /*
        上面的 coroutine_handle 是 C++ 标准库提供的类模板。
        这个类是用户代码跟系统协程调度真正交互的地方，有下面这些成员函数我们等会就会用到：
        destroy：销毁协程done：判断协程是否已经执行完成
        resume：让协程恢复执行
        promise：获得协程相关的 promise 对象（是协程和调用者的主要交互对象；一般类型名称为 promise_type）
        from_promise（静态）：通过 promise 对象的引用来生成一个协程句柄
        */
        //获得promise_type对象，本例中就是该promise_base的子类
      auto &promise = coroutine.promise();
      if (promise.m_continuation != nullptr) {
        return promise.m_continuation;
      } else {
        return std::noop_coroutine();
      }
    }

    auto await_resume() noexcept -> void {}
  };

  promise_base() noexcept = default;
  ~promise_base() = default;

  // 协程创建后立刻处于 suspend 状态
  auto initial_suspend() noexcept { return std::suspend_always{}; }
    //返回储存的m_continuation句柄，如果没有存储，则返回std::noop_coroutine()
  auto final_suspend() noexcept { return final_awaitable{}; }

  // 记录当前协程执行结束后应当恢复的协程
  auto continuation(std::coroutine_handle<> continuation) noexcept -> void {
    m_continuation = continuation;
  }

protected:
  std::coroutine_handle<> m_continuation{nullptr};
};

// 忽略复杂类型的存取
template <typename return_type> struct promise final : public promise_base {
public:
  using task_type = task<return_type>;
  using coroutine_handle = std::coroutine_handle<promise<return_type>>;

  promise() noexcept {}
  promise(const promise &) = delete;
  promise(promise &&other) = delete;
  promise &operator=(const promise &) = delete;
  promise &operator=(promise &&other) = delete;
  ~promise() = default;
    //进入协程后，先创建promise_type对象，然后调用promise对象的该方法构造协程返回值对象
  auto get_return_object() noexcept -> task_type;
    //协程返回时，如果co_runturn返回了值，调用该方法
  auto return_value(return_type &value) -> void {
    m_value = std::make_optional<return_type>(value);
  }
    //处理返回值的移动语义
  auto return_value(return_type &&value) -> void {
    m_value = std::make_optional<return_type>(std::move(value));
  }

  auto unhandled_exception() noexcept -> void {
    m_except = std::current_exception();
  }
    //获取返回值
  auto result() -> return_type {
    if (m_value.has_value()) {
      return *m_value;
    } else {
      throw std::runtime_error{
          "The return value was never set, did you execute the coroutine?"};
    }
  }

private:
  std::optional<return_type> m_value;
  std::exception_ptr m_except;
};
//无返回值promise的偏特化
template <> struct promise<void> : public promise_base {
  using task_type = task<void>;
  using coroutine_handle = std::coroutine_handle<promise<void>>;

  promise() noexcept = default;
  promise(const promise &) = delete;
  promise(promise &&other) = delete;
  promise &operator=(const promise &) = delete;
  promise &operator=(promise &&other) = delete;
  ~promise() = default;

  auto get_return_object() noexcept -> task_type;

  auto return_void() noexcept -> void {}

  auto unhandled_exception() noexcept -> void {
    m_exception_ptr = std::current_exception();
  }

  auto result() -> void {
    if (m_exception_ptr) {
      std::rethrow_exception(m_exception_ptr);
    }
  }

private:
  std::exception_ptr m_exception_ptr{nullptr};
};

// 面向用户的协程对象
template <typename return_type> class [[nodiscard]] task {
public:
  using task_type = task<return_type>;
  //定义promise_type就是promise
  using promise_type = promise<return_type>;
  using coroutine_handle = std::coroutine_handle<promise_type>;

  struct awaitable_base {
    awaitable_base(coroutine_handle coroutine) noexcept
        : m_coroutine(coroutine) {}
    //没设置协程句柄或者协程已经完成，视为ready
    auto await_ready() const noexcept -> bool {
      return !m_coroutine || m_coroutine.done();
    }

    // 连接调用与调用者之间的调用关系
    // 调用者 a  执行co_await 该类型，则会把a的协程句柄保存下来，然后返回自己的协程句柄
    auto await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept
        -> std::coroutine_handle<> {
      m_coroutine.promise().continuation(awaiting_coroutine);
      return m_coroutine;
    }
    //note：promise_base可以存储一个协程句柄，在其执行完成后返回存储的句柄
    coroutine_handle m_coroutine{nullptr};
  };

  task() noexcept : m_coroutine(nullptr) {}

  explicit task(coroutine_handle handle) : m_coroutine(handle) {}
  task(const task &) = delete;
  //只移语义
  task(task &&other) noexcept
      : m_coroutine(std::exchange(other.m_coroutine, nullptr)) {}

  ~task() {
    if (m_coroutine != nullptr) {
      m_coroutine.destroy();
    }
  }

  auto operator=(const task &) -> task & = delete;
    //只移赋值
  auto operator=(task &&other) noexcept -> task & {
    if (std::addressof(other) != this) {
      if (m_coroutine != nullptr) {
        m_coroutine.destroy();
      }

      m_coroutine = std::exchange(other.m_coroutine, nullptr);
    }

    return *this;
  }

  auto is_ready() const noexcept -> bool {
    return m_coroutine == nullptr || m_coroutine.done();
  }

  auto resume() -> bool {
    if (!m_coroutine.done()) {
      m_coroutine.resume();
    }
    return !m_coroutine.done();
  }

  auto destroy() -> bool {
    if (m_coroutine != nullptr) {
      m_coroutine.destroy();
      m_coroutine = nullptr;
      return true;
    }

    return false;
  }

  auto operator co_await() noexcept {
    struct awaitable : public awaitable_base {
        //继承了awaitable_base的await_ready和await_suspend方法
      auto await_resume() -> decltype(auto) {
        //这个result()的值通过promise的return_value()设置
        return this->m_coroutine.promise().result();
      }
    };

    return awaitable{m_coroutine};
  }

  auto handle() -> coroutine_handle { return m_coroutine; }

private:
  coroutine_handle m_coroutine{nullptr};
};

//使用本promise对象构造task对象
template <typename return_type>
inline auto promise<return_type>::get_return_object() noexcept
    -> task<return_type> {
  return task<return_type>{coroutine_handle::from_promise(*this)};
}

inline auto promise<void>::get_return_object() noexcept -> task<> {
  return task<>{coroutine_handle::from_promise(*this)};
}