#ifndef DOC2MD_MSVC_QT_COMPAT_HPP
#define DOC2MD_MSVC_QT_COMPAT_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1950

#include <cstddef>

// Qt 6.5 仍使用该接口，而 MSVC 19.50 起已将其移除。
// TODO: 升级到原生支持 MSVC 19.50+ 的 Qt 后删除此兼容层。
namespace stdext {

template <typename Iterator>
constexpr Iterator make_checked_array_iterator(Iterator _iterator,
                                               std::size_t) noexcept {
    return _iterator;
}

template <typename Iterator>
constexpr Iterator make_unchecked_array_iterator(Iterator _iterator) noexcept {
    return _iterator;
}

} // namespace stdext

#endif

#endif // DOC2MD_MSVC_QT_COMPAT_HPP
