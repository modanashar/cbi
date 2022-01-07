#include "catch.hpp"
#include "cbi/cbi.h"


TEST_CASE("bounded-member-functions")
{
	SECTION("1 to 10")
	{
		using num_t = cbi::Bounded<int32_t, 1, 10>;

		static_assert(num_t::lower_bound() == 1);
		static_assert(std::same_as<decltype(num_t::lower_bound()), int32_t>);

		static_assert(num_t::upper_bound() == 10);
		static_assert(std::same_as<decltype(num_t::upper_bound()), int32_t>);

		static_assert(num_t::underlying_min() == std::numeric_limits<int32_t>::min());
		static_assert(num_t::underlying_max() == std::numeric_limits<int32_t>::max());
		static_assert(num_t::width() == 9);
		static_assert(num_t::positive_portion() == 9);
		static_assert(num_t::negative_portion() == 0);
	}

	SECTION("-5 to 10")
	{
		using num_t = cbi::Bounded<int32_t, -5, 10>;

		static_assert(num_t::lower_bound() == -5);
		static_assert(std::same_as<decltype(num_t::lower_bound()), int32_t>);

		static_assert(num_t::upper_bound() == 10);
		static_assert(std::same_as<decltype(num_t::upper_bound()), int32_t>);

		static_assert(num_t::underlying_min() == std::numeric_limits<int32_t>::min());
		static_assert(num_t::underlying_max() == std::numeric_limits<int32_t>::max());
		static_assert(num_t::width() == 15);
		static_assert(num_t::positive_portion() == 10);
		static_assert(num_t::negative_portion() == 5);
	}
}

TEST_CASE("add small bounds")
{
	constexpr cbi::Bounded<int32_t, 1, 10> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst + sec;

	using expected_t = cbi::Bounded<int32_t, 2, 16>;
	
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 4);

	const std::optional shrinked = res.shrink_bounds<1, 5>();
	REQUIRE(shrinked.has_value());
	REQUIRE(shrinked->get() == 4);
}

TEST_CASE("add large bounds")
{
	constexpr cbi::Bounded<int32_t, 1, std::numeric_limits<int32_t>::max()> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst + sec;

	using expected_t = cbi::Bounded<int64_t, 2, std::numeric_limits<int32_t>::max() + 6ll>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 4);
}

TEST_CASE("add very large bounds")
{
	constexpr cbi::Bounded<int64_t, 1, std::numeric_limits<int64_t>::max()> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst + sec;

	using expected_t = cbi::Bounded<int64_t, 2, std::numeric_limits<int64_t>::max()>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 4);
}

TEST_CASE("mul small bounds")
{
	constexpr cbi::Bounded<int32_t, 1, 10> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst * sec;

	using expected_t = cbi::Bounded<int32_t, 1, 60>;

	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 4);
}

TEST_CASE("mul large bounds")
{
	constexpr cbi::Bounded<int32_t, 1, std::numeric_limits<int32_t>::max()> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst * sec;

	using expected_t = cbi::Bounded<int64_t, 1, std::numeric_limits<int32_t>::max() * 6ll>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 4);
}

TEST_CASE("mul very large bounds")
{
	constexpr cbi::Bounded<int64_t, 1, std::numeric_limits<int64_t>::max()> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst * sec;

	using expected_t = cbi::Bounded<int64_t, 1, std::numeric_limits<int64_t>::max()>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 4);
}

TEST_CASE("mul very large negative bounds")
{
	constexpr cbi::Bounded<int64_t, std::numeric_limits<int64_t>::min(), 5> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 2, 6> sec{ 2 };
	auto res = fst * sec;

	using expected_t = cbi::Bounded<int64_t, std::numeric_limits<int64_t>::min(), 30>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 4);
}


TEST_CASE("sub small bounds")
{
	constexpr cbi::Bounded<int32_t, 1, 10> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst - sec;

	using expected_t = cbi::Bounded<int32_t, 0, 4>;

	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 0);
}

TEST_CASE("sub large bounds")
{
	constexpr cbi::Bounded<int32_t, 1, std::numeric_limits<int32_t>::max()> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst - sec;

	using expected_t = cbi::Bounded<int32_t, 0, std::numeric_limits<int32_t>::max() - 6ll>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 0);
}

TEST_CASE("sub very large bounds")
{
	constexpr cbi::Bounded<int64_t, 1, std::numeric_limits<int64_t>::max()> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst - sec;

	using expected_t = cbi::Bounded<int64_t, 0, std::numeric_limits<int64_t>::max() - 6>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 0);
}

TEST_CASE("sub very large bounds inverted")
{
	constexpr cbi::Bounded<int32_t, 1, 6> fst{ 2 };
	constexpr cbi::Bounded<int64_t, 1, std::numeric_limits<int64_t>::max()> sec{ 2 };
	auto res = fst - sec;

	using expected_t = cbi::Bounded<int64_t, 6ll - std::numeric_limits<int64_t>::max(), 0>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 0);
}

TEST_CASE("div small bounds")
{
	constexpr cbi::Bounded<int32_t, 1, 10> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst / sec;

	using expected_t = cbi::Bounded<int32_t, 0, 10>;

	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 1);
}

TEST_CASE("div large bounds")
{
	constexpr cbi::Bounded<int32_t, 1, std::numeric_limits<int32_t>::max()> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst / sec;

	using expected_t = cbi::Bounded<int32_t, 0, std::numeric_limits<int32_t>::max()>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 1);
}

TEST_CASE("div very large bounds")
{
	constexpr cbi::Bounded<int64_t, 1, std::numeric_limits<int64_t>::max()> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
	auto res = fst / sec;

	using expected_t = cbi::Bounded<int64_t, 0, std::numeric_limits<int64_t>::max()>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 1);
}

TEST_CASE("div very large bounds inverted")
{
	constexpr cbi::Bounded<int32_t, 1, 6> fst{ 2 };
	constexpr cbi::Bounded<int64_t, 1, std::numeric_limits<int64_t>::max()> sec{ 2 };
	auto res = fst / sec;

	using expected_t = cbi::Bounded<int32_t, 0, 6>;
	static_assert(std::same_as<expected_t, decltype(res)>);
	REQUIRE(res.get() == 1);
}
