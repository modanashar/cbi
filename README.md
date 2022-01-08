# cbi
 compile time bounded integers

 A tiny header only library for providing clearer contract-like interfaces for working with intergers.


 # Contents

 * [A First Example](#a-first-example)
 * [A Second Example](#a-second-example)
 * [A Third Example](#a-third-example)
 * [Why bother?](#why-bother?)

# A First Example

 ```cpp
    constexpr cbi::Bounded<int32_t, 1, 10> fst{ 2 };
    constexpr cbi::Bounded<int32_t, 1, 6> sec{ 2 };
    auto res = fst + sec;

    // Bounds are calculated automatically based on the operation
    using expected_t = cbi::Bounded<int32_t, 2, 16>;
    static_assert(std::same_as<expected_t, decltype(res)>);

    assert(res.get() == 4); // extract runtime value

	const std::optional shrinked = res.shrink_bounds<1, 5>(); // shrink bounds
    assert(shrinked.has_value());
 ```

 # A Second Example

 ```cpp
    constexpr cbi::Bounded<int64_t, 1, 5> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 2, 6> sec{ 2 };
	auto res = fst * sec; // + - * / operators are supported

	using expected_t = cbi::Bounded<int64_t, 2, 30>;
	static_assert(std::same_as<expected_t, decltype(res)>);
 ```

 # A Third Example

 ```cpp
    constexpr cbi::Bounded<int64_t, std::numeric_limits<int64_t>::min(), 5> fst{ 2 };
	constexpr cbi::Bounded<int32_t, 2, 6> sec{ 2 };
	auto res = fst * sec; // won't compile - reason: overflow
 ```
 # Why bother?

 The idea behind these bounded integers is to work like compile time contracts that:
 - Force the user of a function to fulfill the contractual requirments on the provided arguments, by forcing them to construct the bounded object, that guarantees a valid contained value.
 - Move the validation of the parameters from the library code to the user code. If an invalid value was given to construct the bounded object it will either fail to compile or trigger an assert, thus forcing the user of a function to fulfill the contract on their side.