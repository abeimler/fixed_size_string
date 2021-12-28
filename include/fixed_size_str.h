#pragma once

#include <string_view>
#include <algorithm>
#include <string>
#include <array>
#ifdef FIXED_SIZE_STRING_ENABLE_IOSTREAM
#include <ostream>
#endif

namespace fss
{
	template <class CharT
		, std::size_t max_length
		, class Traits = std::char_traits<CharT>>
		class basic_str
	{
	public:
		constexpr basic_str() noexcept = default;

		template <std::size_t N>
		/*implicit*/ consteval basic_str(const char (&str)[N]) : active_length_(std::min(N-1, max_length)) {
			assert(str[N - 1] == '\0');
			Traits::copy(buffer_.data(), str, active_length_);
			buffer_[active_length_] = '\0';
		}

		/*implicit*/ constexpr basic_str(const CharT* str)
			: active_length_(std::min(Traits::length(str), max_length))
		{
			Traits::copy(buffer_.data(), str, active_length_);
		}

		template <std::size_t N>
		explicit constexpr basic_str(const std::array<CharT, N>& buffer)
			: active_length_(std::min(Traits::length(buffer), max_length))
		{
			Traits::copy(buffer_.data(), buffer.cbegin(), active_length_);
			buffer_[active_length_] = '\0';
		}

		constexpr basic_str(const CharT* str, std::size_t length)
			: active_length_(std::min(length, max_length))
		{
			Traits::copy(buffer_.data(), str, active_length_);
		}

		constexpr const CharT* c_str() const noexcept { return buffer_.data(); }

		constexpr const CharT* data() const noexcept { return buffer_.data(); }

		constexpr CharT* data() noexcept { return buffer_.data(); }

		constexpr const auto& buffer() const noexcept { return buffer_; }

		constexpr std::basic_string_view<CharT, Traits> str() const noexcept
		{
			return std::basic_string_view<CharT, Traits>(buffer_.data(), active_length_);
		}

		constexpr auto length() const noexcept { return active_length_; }

		constexpr auto max_size() const noexcept { return max_length; }

		constexpr auto empty() const noexcept { return active_length_ == 0; }

		constexpr void clear() noexcept
		{
			active_length_ = 0;
			buffer_[0] = '\0';
		}

		constexpr void reset(const CharT* str)
		{
			active_length_ = std::min(Traits::length(str), max_length);
			reset_(str, active_length_);
		}

		constexpr void reset(const CharT* str, std::size_t length)
		{
			active_length_ = std::min(length, max_length);
			reset_(str, active_length_);
		}

		constexpr void append(const CharT* str)
		{
			auto to_copy = std::min(Traits::length(str), (max_length - active_length_));
			append_(str, to_copy);
		}

		constexpr void append(const CharT* str, std::size_t length)
		{
			auto to_copy = std::min(length, (max_length - active_length_));
			append_(str, to_copy);
		}

		constexpr void remove_prefix(std::size_t length)
		{
			std::copy(buffer_.cbegin() + length, buffer_.cbegin() + active_length_, buffer_.begin());
			active_length_ -= length;
			buffer_[active_length_] = '\0';
		}

		constexpr void remove_suffix(std::size_t length) noexcept
		{
			active_length_ = active_length_ - length;
			buffer_[active_length_] = '\0';
		}

		/* implemented as a member to avoid implicit conversion */
		constexpr bool operator==(const basic_str& rhs) const
		{
			return (max_size() == rhs.max_size())
				&& (length() == rhs.length())
				&& std::equal(buffer_.cbegin(), buffer_.cbegin() + length(), rhs.buffer_.cbegin());
		}

		constexpr bool operator!=(const basic_str& rhs) const
		{
			return !(*this == rhs);
		}

		constexpr void swap(basic_str& rhs) noexcept
		{
			std::swap(active_length_, rhs.active_length_);
			std::swap(buffer_, rhs.buffer_);
		}

	private:
		constexpr void reset_(const CharT* str, std::size_t length)
		{
			Traits::copy(buffer_.data(), str, length);
			buffer_[length] = '\0';
		}

		constexpr void append_(const CharT* str, std::size_t to_copy)
		{
			std::copy(str, str + to_copy, buffer_.begin() + active_length_);
			active_length_ += to_copy;
			buffer_[active_length_] = '\0';
		}

		std::size_t active_length_{ 0 };
		std::array<CharT, max_length + 1> buffer_{};
	};

	template <class CharT
		, std::size_t max_length
		, class Traits = std::char_traits<CharT>>
		inline constexpr void swap(const basic_str<CharT, max_length>& lhs
			, const basic_str<CharT, max_length>& rhs) noexcept
	{
		rhs.swap(lhs);
	}

	#ifdef FIXED_SIZE_STRING_ENABLE_IOSTREAM
	template <class CharT
		, std::size_t max_length
		, class Traits = std::char_traits<CharT>>
		inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT
			, Traits>& os, const basic_str<CharT, max_length>& str)
	{
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
}
