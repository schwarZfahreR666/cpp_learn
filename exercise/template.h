/*
 * Written by Wu Yongwei <wuyongwei AT gmail DOT com>.
 *
 * Using this file requires a C++17-compliant compiler.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 *
 */

#ifndef OUTPUT_CONTAINER_H
#define OUTPUT_CONTAINER_H

#include <ostream>      // std::ostream
#include <type_traits>  // std::false_type/true_type/decay_t/is_same_v
#include <utility>      // std::declval/pair

// Type trait to detect std::pair

template <typename T>
struct is_pair : std::false_type {};
template <typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};
//如果匹配pair类型，则模板value为true，编译期常量is_pair_v<T>等于其value值
template <typename T>
inline constexpr bool is_pair_v = is_pair<T>::value;

// Type trait to detect whether an output function already exists
template <typename T>
struct has_output_function {
    template <class U>
    static auto output(U* ptr)
        //检测是否能成功运行ostream <<，decltype支持在编译期模拟运行表达式
        -> decltype(std::declval<std::ostream&>() << *ptr,
                    std::true_type());
    template <class U>
    static std::false_type output(...);
    static constexpr bool value =
        decltype(output<T>(nullptr))::value;  //若匹配第一个output成功，则赋值为std::true_type::value
};
template <typename T>
inline constexpr bool has_output_function_v =
    has_output_function<T>::value;
/* NB: Visual Studio 2017 (or below) may have problems with
 *     has_output_function_v<T>: you should then use
 *     has_output_function<T>::value instead, or upgrade to
 *     Visual Studio 2019. */

// Output function for std::pair
template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& pr);

// Element output function for containers that define a key_type and
// have its value type as std::pair
template <typename T, typename Cont>
auto output_element(std::ostream& os, const T& element,
                    const Cont&, std::true_type)
    //该cont定义了key_type，并且最后一个参数为std::true_type,则可以匹配成功该模板
    -> decltype(std::declval<typename Cont::key_type>(), os);
// Element output function for other containers
template <typename T, typename Cont>
auto output_element(std::ostream& os, const T& element,
                    const Cont&, ...)
    -> decltype(os);

// Main output function, enabled only if no output function already exists
template <typename T,
        //没有output函数(<<)时才生效
          typename = std::enable_if_t<!has_output_function_v<T>>>
auto operator<<(std::ostream& os, const T& container)
    //必须支持begin()、end()函数
    -> decltype(container.begin(), container.end(), os)
{
    using std::decay_t;
    using std::is_same_v;
    //通过解引用该容器的迭代器，获得element类型
    //使用decay_t可以把推断类型中的const/volatile/ref等去掉
    using element_type = decay_t<decltype(*container.begin())>;
    constexpr bool is_char_v = is_same_v<element_type, char>;
    //编译期即可确定是否走该分支
    if constexpr (!is_char_v) {
        os << '{';
    }
    auto end = container.end();
    bool on_first_element = true;
    for (auto it = container.begin(); it != end; ++it) {
        if constexpr (is_char_v) {
            if (*it == '\0') {
                break;
            }
        } else {
            if (!on_first_element) {
                os << ", ";
            } else {
                os << ' ';
                on_first_element = false;
            }
        }
        //这里通过is_pair<element_type>()是否是std::true_type类型去匹配output_element函数
        //也就是容器里存的是pair
        output_element(os, *it, container, is_pair<element_type>());
    }
    if constexpr (!is_char_v) {
        if (!on_first_element) {  // Not empty
            os << ' ';
        }
        os << '}';
    }
    return os;
}

//下面三个函数的输出可能还是容器，所以要放到最下面，确保可以看到上面的通用输出函数

//上面pair out函数的定义
template <typename T, typename Cont>
auto output_element(std::ostream& os, const T& element,
                    const Cont&, std::true_type)
    -> decltype(std::declval<typename Cont::key_type>(), os)
{
    os << element.first << " => " << element.second;
    return os;
}

//其他非pair容器类型out函数的定义
template <typename T, typename Cont>
auto output_element(std::ostream& os, const T& element,
                    const Cont&, ...)
    -> decltype(os)
{
    os << element;
    return os;
}

//打印pair类型
template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& pr)
{
    os << '(' << pr.first << ", " << pr.second << ')';
    return os;
}

#endif // OUTPUT_CONTAINER_H
