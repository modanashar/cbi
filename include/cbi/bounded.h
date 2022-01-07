#pragma once
#include <concepts>
#include "cbi/details.h"

namespace cbi
{
	template <typename T>
	concept signed_bounded = std::signed_integral<typename T::underlying_type> && requires(T t)
	{
		T(std::declval<typename T::underlying_type>());
		{T::upper_bound()} -> std::same_as<typename T::underlying_type>;
		{T::lower_bound()} -> std::same_as<typename T::underlying_type>;
		{T::underlying_max()} -> std::same_as<typename T::underlying_type>;
		{T::underlying_min()} -> std::same_as<typename T::underlying_type>;
	};


	template <std::signed_integral Underlying,
		Underlying LowerBound = std::numeric_limits<Underlying>::min(),
		Underlying UpperBound = std::numeric_limits<Underlying>::max()
	>
		struct Bounded
	{
		static_assert(LowerBound <= UpperBound);
	private:
		Underlying value;

	public:
		using underlying_type = Underlying;

		constexpr explicit Bounded(Underlying value) : value(value)
		{
			const bool cond = LowerBound <= value && value <= UpperBound;
			if (std::is_constant_evaluated())
			{
				if (!cond)
					throw std::logic_error{ "value doesn't fit into given bounds" };
			}
			else
			{
				assert(cond);
			}
		}

		template <Underlying NewLowerBound, Underlying NewUpperBound>
		[[nodiscard]] constexpr std::optional<Bounded<Underlying, NewLowerBound, NewUpperBound>>
		shrink_bounds() const noexcept
		{
			if (NewLowerBound <= value && value <= NewUpperBound)
				return std::optional{ Bounded<Underlying, NewLowerBound, NewUpperBound>{value} };
			return std::nullopt;
		}

		template <std::signed_integral NewType>
		[[nodiscard]] constexpr Bounded<NewType, LowerBound, UpperBound>
		cast_underlying() const noexcept
		{
			static_assert(details::fits_in<NewType>(LowerBound, UpperBound));
			return Bounded<NewType, LowerBound, UpperBound>{static_cast<NewType>(value)};
		}

		template <Underlying NewLowerBound, Underlying NewUpperBound>
		[[nodiscard]] constexpr Bounded<Underlying, NewLowerBound, NewUpperBound>
		expand_bounds() const noexcept
		{
			static_assert(NewLowerBound <= LowerBound);
			static_assert(NewUpperBound >= UpperBound);
			return Bounded<Underlying, NewLowerBound, NewUpperBound>{value};
		}

		template <Underlying NewLowerBound, Underlying NewUpperBound>
		[[nodiscard]] constexpr operator Bounded<Underlying, NewLowerBound, NewUpperBound>() const noexcept
		{
			static_assert(NewLowerBound <= LowerBound);
			static_assert(NewUpperBound >= UpperBound);
			return Bounded<Underlying, NewLowerBound, NewUpperBound>{value};
		}

		[[nodiscard]] constexpr auto get() const noexcept { return value; }
		[[nodiscard]] static constexpr auto lower_bound() noexcept { return LowerBound; }
		[[nodiscard]] static constexpr auto upper_bound() noexcept { return UpperBound; }
		[[nodiscard]] static constexpr auto width() noexcept { return upper_bound() - lower_bound(); }
		[[nodiscard]] static constexpr auto underlying_min() noexcept { return std::numeric_limits<Underlying>::min(); }
		[[nodiscard]] static constexpr auto underlying_max() noexcept { return std::numeric_limits<Underlying>::max(); }

		[[nodiscard]] static constexpr auto negative_portion() noexcept
		{
			if constexpr (lower_bound() >= 0) return 0;
			else if constexpr (upper_bound() >= 0) return 0 - lower_bound();
			else return width();
		}
		[[nodiscard]] static constexpr auto positive_portion() noexcept
		{
			if constexpr (upper_bound() <= 0) return 0;
			else if constexpr (lower_bound() <= 0) return upper_bound();
			else return width();
		}
	};

	static_assert(signed_bounded<Bounded<int8_t>>);
	static_assert(signed_bounded<Bounded<int16_t>>);
	static_assert(signed_bounded<Bounded<int32_t>>);
	static_assert(signed_bounded<Bounded<int64_t>>);
	static_assert(std::is_standard_layout_v<Bounded<int8_t>>);
	static_assert(std::is_standard_layout_v<Bounded<int16_t>>);
	static_assert(std::is_standard_layout_v<Bounded<int32_t>>);
	static_assert(std::is_standard_layout_v<Bounded<int64_t>>);


	template <
		std::signed_integral Underlying,
		Underlying LowerBound = std::numeric_limits<Underlying>::min(),
		Underlying UpperBound = std::numeric_limits<Underlying>::max()
	>
	[[nodiscard]] constexpr auto make_bounded(Underlying value) noexcept -> std::optional<Bounded<Underlying, LowerBound, UpperBound>>
	{
		const bool cond = LowerBound <= value && value <= UpperBound;
		if (!cond) return std::nullopt;
		return std::optional{ Bounded<Underlying, LowerBound, UpperBound>{value} };
	}

	namespace details
	{
		template<
			intmax_t lower_bound,
			intmax_t upper_bound,
			signed_bounded Fst,
			signed_bounded Sec
		>
			consteval auto find_type(Fst, Sec)
		{
			using NextFst = typename details::next_size<typename Fst::underlying_type>::type;
			using NextSec = typename details::next_size<typename Sec::underlying_type>::type;
			constexpr auto fst_has_next = details::next_size<typename Fst::underlying_type>::value;
			constexpr auto sec_has_next = details::next_size<typename Sec::underlying_type>::value;

			if constexpr (details::fits_in<typename Fst::underlying_type>(lower_bound, upper_bound))
			{
				return Bounded<typename Fst::underlying_type, lower_bound, upper_bound>{0};
			}
			else if constexpr (details::fits_in<typename Sec::underlying_type>(lower_bound, upper_bound))
			{
				return Bounded<typename Sec::underlying_type, lower_bound, upper_bound>{0};
			}
			else if constexpr (fst_has_next && details::fits_in<NextFst>(lower_bound, upper_bound))
			{
				return Bounded<NextFst, lower_bound, upper_bound>{0};
			}
			else if constexpr (sec_has_next && details::fits_in<NextSec>(lower_bound, upper_bound))
			{
				return Bounded<NextSec, lower_bound, upper_bound>{0};
			}
			else
			{
				return std::false_type{};
			}
		}
	}

	template <
		signed_bounded Fst,
		signed_bounded Sec
	>
	constexpr auto operator+(Fst fst, Sec sec)
	{
		constexpr auto upper_bound = details::limited_add(Fst::upper_bound(), Sec::upper_bound());
		constexpr auto lower_bound = details::limited_add(Fst::lower_bound(), Sec::lower_bound());

		using ResType = decltype(details::find_type<lower_bound, upper_bound>(fst, sec));
		static_assert(!std::same_as<ResType, std::false_type>, "Couldn't find fitting type");

		return ResType{ static_cast<typename ResType::underlying_type>(fst.get() + sec.get()) };
	}

	template <
		signed_bounded Fst,
		signed_bounded Sec
	>
	constexpr auto operator-(Fst fst, Sec sec)
	{
		constexpr auto upper_bound = details::limited_sub(Fst::upper_bound(), Sec::upper_bound());
		constexpr auto lower_bound = details::limited_sub(Fst::lower_bound(), Sec::lower_bound());

		using ResType = decltype(details::find_type<std::min(lower_bound, upper_bound),
			std::max(lower_bound, upper_bound)>(fst, sec));
		static_assert(!std::same_as<ResType, std::false_type>, "Couldn't find fitting type");

		return ResType{ static_cast<typename ResType::underlying_type>(fst.get() - sec.get()) };
	}

	template <
		signed_bounded Fst,
		signed_bounded Sec
	>
	constexpr auto operator*(Fst fst, Sec sec)
	{
		constexpr auto upper_bound = details::limited_mul(Fst::upper_bound(), Sec::upper_bound());
		constexpr auto lower_bound = details::limited_mul(Fst::lower_bound(), Sec::lower_bound());

		using ResType = decltype(details::find_type<lower_bound, upper_bound>(fst, sec));
		static_assert(!std::same_as<ResType, std::false_type>, "Couldn't find fitting type");

		return ResType{ static_cast<typename ResType::underlying_type>(fst.get() * sec.get()) };
	}

	template <
		signed_bounded Fst,
		signed_bounded Sec
	>
	constexpr auto operator/(Fst fst, Sec sec)
	{
		constexpr auto upper_bound = Sec::lower_bound() != 0
			? Fst::upper_bound() / Sec::lower_bound()
			: (Fst::upper_bound() > 0 && Sec::lower_bound() > 0) || (Fst::upper_bound() < 0 && Sec::lower_bound() < 0)
				? std::numeric_limits<intmax_t>::max()
				: std::numeric_limits<intmax_t>::min();

		constexpr auto lower_bound = Sec::upper_bound() != 0
			? Fst::lower_bound() / Sec::upper_bound()
			: (Fst::lower_bound() > 0 && Sec::upper_bound() > 0) || (Fst::lower_bound() < 0 && Sec::upper_bound() < 0)
				? std::numeric_limits<intmax_t>::max()
				: std::numeric_limits<intmax_t>::min();

		using ResType = decltype(details::find_type<lower_bound, upper_bound>(fst, sec));
		static_assert(!std::same_as<ResType, std::false_type>, "Couldn't find fitting type");

		return ResType{ static_cast<typename ResType::underlying_type>(fst.get() / sec.get()) };
	}
}
