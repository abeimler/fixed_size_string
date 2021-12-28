#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>
#include <string_view>
#ifdef FIXED_SIZE_STRING_ENABLE_IOSTREAM
#include <iostream>
#endif

namespace fss {
    template <class CharT, std::size_t max_length, class Traits = std::char_traits<CharT>>
    class basic_str {
    public:
        using Buffer = std::array<CharT, max_length + 1>;
        using traits_type = Traits;
        using value_type = typename Buffer::value_type;
        using reference = typename Buffer::reference;
        using const_reference = typename Buffer::const_reference;
        using pointer = typename Buffer::pointer;
        using const_pointer = typename Buffer::const_pointer;
        using iterator = typename Buffer::iterator;
        using const_iterator = typename Buffer::const_iterator;
        using reverse_iterator = typename Buffer::reverse_iterator;
        using const_reverse_iterator = typename Buffer::const_reverse_iterator;

        inline static constexpr std::size_t MAX_LENGTH = max_length;

        constexpr basic_str() noexcept = default;

        template <std::size_t N>
        /*implicit*/ consteval basic_str(const char(&str)[N]) : active_length_(std::min(N - 1, max_length)) {
            assert(str[N - 1] == '\0');
            Traits::copy(buffer_.data(), str, active_length_);
            buffer_[active_length_] = '\0';
        }

        /*implicit*/ constexpr basic_str(const CharT* str) : active_length_(std::min(Traits::length(str), max_length)) {
            Traits::copy(buffer_.data(), str, active_length_);
        }

        template <std::size_t N>
        explicit constexpr basic_str(const std::array<CharT, N>& buffer)
            : active_length_(std::min<std::size_t>(Traits::length(buffer.data()), max_length)) {
            Traits::copy(buffer_.data(), buffer.data(), active_length_);
            buffer_[active_length_] = '\0';
        }

        constexpr basic_str(const CharT* str, std::size_t length) : active_length_(std::min(length, max_length)) {
            Traits::copy(buffer_.data(), str, active_length_);
        }

        constexpr const CharT* c_str() const noexcept { return buffer_.data(); }

        constexpr const CharT* data() const noexcept { return buffer_.data(); }

        constexpr CharT* data() noexcept { return buffer_.data(); }

        constexpr const Buffer& buffer() const noexcept { return buffer_; }

        constexpr std::basic_string_view<CharT, Traits> str() const noexcept {
            return std::basic_string_view<CharT, Traits>(buffer_.data(), active_length_);
        }

        constexpr auto length() const noexcept { return active_length_; }

        constexpr auto max_size() const noexcept { return max_length; }

        constexpr auto empty() const noexcept { return active_length_ == 0; }

        constexpr void clear() noexcept {
            active_length_ = 0;
            buffer_[0] = '\0';
        }

        constexpr void reset(const CharT* str) {
            active_length_ = std::min(Traits::length(str), max_length);
            reset_(str, active_length_);
        }

        constexpr void reset(const CharT* str, std::size_t length) {
            active_length_ = std::min(length, max_length);
            reset_(str, active_length_);
        }

        constexpr void append(const CharT* str) {
            auto to_copy = std::min(Traits::length(str), (max_length - active_length_));
            append_(str, to_copy);
        }

        constexpr void append(const CharT* str, std::size_t length) {
            auto to_copy = std::min(length, (max_length - active_length_));
            append_(str, to_copy);
        }

        constexpr void remove_prefix(std::size_t length) {
            std::copy(std::next(buffer_.cbegin(), length), std::next(buffer_.cbegin(), active_length_), buffer_.begin());
            active_length_ -= length;
            buffer_[active_length_] = '\0';
        }

        constexpr void remove_suffix(std::size_t length) noexcept {
            active_length_ = active_length_ - length;
            buffer_[active_length_] = '\0';
        }

        /* implemented as a member to avoid implicit conversion */
        constexpr bool operator==(const basic_str& rhs) const {
            return (max_size() == rhs.max_size()) && (length() == rhs.length()) &&
                std::equal(buffer_.cbegin(), std::next(buffer_.cbegin(), length()), rhs.buffer_.cbegin());
        }

        constexpr bool operator!=(const basic_str& rhs) const { return !(*this == rhs); }

        constexpr void swap(basic_str& rhs) noexcept {
            std::swap(active_length_, rhs.active_length_);
            std::swap(buffer_, rhs.buffer_);
        }

        constexpr void fill(const CharT& value) {
            buffer_.fill(value);
            active_length_ = buffer_.size() - 1;
            buffer_[active_length_] = '\0';
        }

        constexpr iterator begin() noexcept { return buffer_.begin(); }

        constexpr const_iterator cbegin() noexcept { return buffer_.cbegin(); }

        constexpr reverse_iterator rbegin() noexcept { return std::next(buffer_.begin(), active_length_); }

        constexpr const_reverse_iterator crbegin() noexcept { return std::next(buffer_.cbegin(), active_length_); }

        constexpr iterator end() noexcept { return std::next(buffer_.begin(), active_length_ + 1); }

        constexpr const_iterator cend() noexcept { return std::next(buffer_.cbegin(), active_length_ + 1); }

        constexpr reverse_iterator rend() noexcept { return std::next(buffer_.begin(), -1); }

        constexpr const_reverse_iterator rcend() noexcept { return std::next(buffer_.cbegin(), -1); }

        constexpr void repair() {
            active_length_ = Traits::length(buffer_.data());
        }

    private:
        constexpr void reset_(const CharT* str, std::size_t length) {
            Traits::copy(buffer_.data(), str, length);
            buffer_[length] = '\0';
        }

        constexpr void append_(const CharT* str, std::size_t to_copy) {
            std::copy(str, str + to_copy, std::next(buffer_.begin(), active_length_));
            active_length_ += to_copy;
            buffer_[active_length_] = '\0';
        }

    private:
        std::size_t active_length_{ 0 };
        Buffer buffer_{};
    };

    template <class CharT, std::size_t max_length, class Traits = std::char_traits<CharT>>
    inline constexpr void swap(const basic_str<CharT, max_length>& lhs, const basic_str<CharT, max_length>& rhs) noexcept {
        rhs.swap(lhs);
    }

#ifdef FIXED_SIZE_STRING_ENABLE_IOSTREAM
    template <class CharT, std::size_t max_length, class Traits = std::char_traits<CharT>>
    inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
        const basic_str<CharT, max_length>& str) {
        return os << str.c_str();
    }
#endif

    template <std::size_t max_length>
    using fixed_size_str = basic_str<char, max_length>;

    template <std::size_t max_length>
    using fixed_size_wstr = basic_str<wchar_t, max_length>;

    template <std::size_t max_length>
    using fixed_size_u8str = basic_str<char8_t, max_length>;

    template <std::size_t max_length>
    using fixed_size_u16str = basic_str<char16_t, max_length>;

    template <std::size_t max_length>
    using fixed_size_u32str = basic_str<char32_t, max_length>;

} // namesapce fss