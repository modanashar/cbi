#pragma once

namespace cbi
{

	namespace details
	{
		template <std::signed_integral T> struct next_size {};

		template <> struct next_size<int8_t> { using type = int16_t; static constexpr bool value = true; };
		template <> struct next_size<int16_t> { using type = int32_t; static constexpr bool value = true; };
		template <> struct next_size<int32_t> { using type = int64_t; static constexpr bool value = true; };
		template <> struct next_size<int64_t> { using type = int64_t; static constexpr bool value = false; };
		template <std::signed_integral T> constexpr auto next_size_t = next_size<T>::type;
		template <std::signed_integral T> constexpr auto next_size_v = next_size<T>::value;

		template <std::signed_integral T>
		constexpr auto fits_in(std::intmax_t low, std::intmax_t high)
		{
			return std::numeric_limits<T>::min() <= low && high <= std::numeric_limits<T>::max();
		}
		

		[[nodiscard]] constexpr std::optional<std::intmax_t>
		limited_add(const std::intmax_t fst, const std::intmax_t sec)
		{
			if (fst > 0 && sec > std::numeric_limits<std::intmax_t>::max() - fst) 
				return std::nullopt;
			if (fst < 0 && sec < std::numeric_limits<std::intmax_t>::min() - fst) 
				return std::nullopt;
			return fst + sec;
		}

		[[nodiscard]] constexpr std::optional<std::intmax_t>
		limited_sub(const std::intmax_t fst, const std::intmax_t sec)
		{
			if (sec < 0 && fst > std::numeric_limits<std::intmax_t>::max() + sec) 
				return std::nullopt;
			if (sec > 0 && fst < std::numeric_limits<std::intmax_t>::min() + sec) 
				return std::nullopt;
			return fst - sec;
		}

		[[nodiscard]] constexpr std::optional<std::intmax_t>
		limited_mul(const std::intmax_t fst, const std::intmax_t sec)
		{
			if (fst > 0 && sec > 0 && fst > std::numeric_limits<std::intmax_t>::max() / sec)
			{
				return std::nullopt;
			}
			if (fst < 0 && sec < 0 && fst < std::numeric_limits<std::intmax_t>::min() / sec)
			{
				return std::nullopt;
			}
			if (fst > 0 && sec < 0 && fst > std::numeric_limits<std::intmax_t>::min() / sec)
			{
				return std::nullopt;
			}
			if (fst < 0 && sec > 0 && fst < std::numeric_limits<std::intmax_t>::min() / sec)
			{
				return std::nullopt;
			}
			return fst * sec;
		}
	}
}
