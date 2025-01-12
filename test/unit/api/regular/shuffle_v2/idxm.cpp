//==================================================================================================
/**
  EVE - Expressive Vector Engine
  Copyright : EVE Project Contributors
  SPDX-License-Identifier: BSL-1.0
**/
//==================================================================================================
#include "test.hpp"

namespace
{

template<std::size_t N>
constexpr std::array<std::ptrdiff_t, N>
to_idxs(std::array<int, N> x)
{
  std::array<std::ptrdiff_t, N> r;
  std::copy(x.begin(), x.end(), r.begin());
  return r;
}

constexpr int we_ = -2;
constexpr int na_ = -1;

TTS_CASE("are_below_ignoring_specials")
{
  auto test = []<std::size_t N>(std::array<int, N> _in, std::ptrdiff_t ub, bool expected)
  {
    auto in     = to_idxs(_in);
    bool actual = eve::detail::idxm::are_below_ignoring_specials(in, ub);

    TTS_EQUAL(expected, actual) << tts::as_string(in);
  };

  test(std::array {0, 1}, 1, false);
  test(std::array {0, 1}, 2, true);
  test(std::array {na_, 1}, 2, true);
  test(std::array {we_, 0}, 1, true);
  test(std::array {we_, we_}, 1, true);
};

TTS_CASE("to_pattern")
{
  constexpr auto arr    = to_idxs(std::array {0, 1, we_, na_});
  auto           actual = eve::detail::idxm::to_pattern<arr>();
  TTS_EQUAL((eve::pattern<0, 1, eve::we_, eve::na_>), actual);
};

TTS_CASE("swap_xy")
{
  auto test =
      []<std::size_t N>(std::array<int, N> _expected, std::array<int, N> _input, std::ptrdiff_t s)
  {
    std::array<std::ptrdiff_t, N> expected = to_idxs(_expected);
    std::array<std::ptrdiff_t, N> input    = to_idxs(_input);

    TTS_EQUAL(expected, eve::detail::idxm::swap_xy(input, s)) << tts::as_string(input) << " " << s;
    TTS_EQUAL(input, eve::detail::idxm::swap_xy(expected, s))
        << tts::as_string(expected) << " " << s;
  };

  // T::size() == 1
  test(std::array {we_}, std::array {we_}, 1);
  test(std::array {0}, std::array {1}, 1);
  test(std::array {0, 0}, std::array {1, 1}, 1);

  // T::size() == 2
  test(std::array {3, we_}, std::array {1, we_}, 2);
  test(std::array {3, na_}, std::array {1, na_}, 2);

  test(std::array {3, 1}, std::array {1, 3}, 2);
  test(std::array {2, 1}, std::array {0, 3}, 2);
  test(std::array {2, 1}, std::array {0, 3}, 2);
};

TTS_CASE("validate_pattern")
{
  auto test = [](auto g, auto p, bool expected)
  {
    using T     = eve::wide<std::int32_t, eve::fixed<2>>;
    bool actual = eve::detail::idxm::validate_pattern(g, p, eve::as<T> {}, eve::as<T> {});
    TTS_EQUAL(expected, actual) << "p: " << p << " g: " << g;
  };
  test(eve::lane<1>, eve::pattern<0, 1, 2, 3>, true);
  test(eve::lane<1>, eve::pattern<0, 1, 2, 4>, false);

  test(eve::lane<1>, eve::pattern<0, 1, 2, eve::we_>, true);
  test(eve::lane<1>, eve::pattern<0, 1, 2, eve::na_>, true);
  test(eve::lane<2>, eve::pattern<0, 1, 2, 3>, false);
  test(eve::lane<1>, eve::pattern<0, 1, 2, -5>, false);
};

TTS_CASE("fix_indexes_to_fundamental")
{
  auto test =
      []<std::ptrdiff_t Fundamental, std::size_t N1, std::size_t N2>(eve::fixed<Fundamental>,
                                                                     std::array<int, N1> _p,
                                                                     std::ptrdiff_t      cardinal,
                                                                     std::array<int, N2> _expected)
  {
    auto p        = to_idxs(_p);
    auto expected = to_idxs(_expected);
    auto actual   = eve::detail::idxm::fix_indexes_to_fundamental<Fundamental>(p, cardinal);

    TTS_EQUAL(expected, actual) << tts::as_string(p) << ", " << cardinal;
  };

  test(eve::lane<4>, std::array {0, 1, 2, 3}, 4, std::array {0, 1, 2, 3});
  test(eve::lane<8>, std::array {0, 1, 2, 3}, 4, std::array {0, 1, 2, 3, we_, we_, we_, we_});
  // 2 wides
  test(eve::lane<4>, std::array {0, 2}, 2, std::array {0, 4, we_, we_});
  test(eve::lane<4>, std::array {3, 1}, 2, std::array {5, 1, we_, we_});
  // 3 wides
  // 0, 1, 2, 3, 4, 5 =>
  // 0, 1, -, -, 4, 5, -, -, 8, 9, -, -
  test(eve::lane<4>, std::array {2, 5}, 2, std::array {4, 9, we_, we_});
};

TTS_CASE("is_repeating_pattern")
{
  auto yes_test = []<std::size_t N>(std::array<int, N> _full, std::array<int, N / 2> _half)
  {
    auto full = to_idxs(_full);
    auto half = to_idxs(_half);

    auto res = eve::detail::idxm::is_repeating_pattern(full);
    TTS_EXPECT(res) << tts::as_string(full);
    if( res ) { TTS_EQUAL(*res, half) << tts::as_string(full); }
  };

  auto no_test = []<std::size_t N>(std::array<int, N> _full)
  {
    auto full = to_idxs(_full);
    auto res  = eve::detail::idxm::is_repeating_pattern(full);
    TTS_EXPECT_NOT(res) << tts::as_string(full) << " res: " << tts::as_string(*res);
  };

  yes_test(std::array {0, 1}, std::array {0});
  yes_test(std::array {0, we_}, std::array {0});
  yes_test(std::array {we_, 1}, std::array {0});
  yes_test(std::array {we_, we_}, std::array {we_});
  yes_test(std::array {na_, na_}, std::array {na_});
  yes_test(std::array {we_, na_}, std::array {na_});
  yes_test(std::array {na_, we_}, std::array {na_});

  no_test(std::array {0, 0});
  no_test(std::array {1, 1});
  no_test(std::array {na_, 1});

  yes_test(std::array {1, 0, 3, 2}, std::array {1, 0});
  yes_test(std::array {1, we_, 3, 2}, std::array {1, 0});
  yes_test(std::array {we_, 0, 3, we_}, std::array {1, 0});
  yes_test(std::array {we_, we_, 3, 2}, std::array {1, 0});
  yes_test(std::array {na_, 0, na_, we_}, std::array {na_, 0});

  no_test(std::array {1, 0, 2, 3});
  no_test(std::array {na_, 0, we_, 3});
};

TTS_CASE("is_identity")
{
  auto test = []<std::size_t N>(std::array<int, N> _in, bool expected)
  {
    auto in     = to_idxs(_in);
    bool actual = eve::detail::idxm::is_identity(in);
    TTS_EQUAL(expected, actual) << tts::as_string(in);
  };
  test(std::array {0}, true);
  test(std::array {we_}, true);
  test(std::array {1}, false);
  // We assume that the pattern is simplified
  test(std::array {0, 1}, false);
};

TTS_CASE("is_zero")
{
  auto test = []<std::size_t N>(std::array<int, N> _in, bool expected)
  {
    auto in     = to_idxs(_in);
    bool actual = eve::detail::idxm::is_zero(in);
    TTS_EQUAL(expected, actual) << tts::as_string(in);
  };
  test(std::array {na_}, true);
  test(std::array {we_}, true);
  test(std::array {0}, false);
  // We assume that the pattern is simplified
  test(std::array {na_, na_}, false);
};

TTS_CASE("has_zeroes")
{
  auto test = []<std::size_t N>(std::array<int, N> _in, bool expected)
  {
    auto in     = to_idxs(_in);
    bool actual = eve::detail::idxm::has_zeroes(in);

    TTS_EQUAL(expected, actual) << tts::as_string(in);
  };

  test(std::array {1, 2}, false);
  test(std::array {we_, 1}, false);
  test(std::array {we_, 1, na_}, true);
};

TTS_CASE("is_in_order")
{
  auto test = []<std::size_t N>(std::array<int, N> _in, std::ptrdiff_t expected)
  {
    auto in     = to_idxs(_in);
    auto actual = eve::detail::idxm::is_in_order(in).value_or(-1);

    TTS_EQUAL(expected, actual) << tts::as_string(in);
  };

  test(std::array {1, 2}, 1);
  test(std::array {0, 1}, 0);
  test(
      std::array {
          we_,
          0,
      },
      -1);
  test(std::array {we_, we_, 1}, -1);
  test(std::array {we_, 1, we_}, 0);

  test(std::array {1, we_, 3}, 1);
  test(std::array {1, we_, 2}, -1);
};

TTS_CASE("is_rotate")
{
  auto test = []<std::size_t N>(std::array<int, N> _in, std::ptrdiff_t expected)
  {
    auto in     = to_idxs(_in);
    auto actual = eve::detail::idxm::is_rotate(in);

    if( expected == -1 )
    {
      TTS_EXPECT_NOT(actual) << tts::as_string(in) << " actual: " << *actual;
      return;
    }

    if( actual ) { TTS_EQUAL(*actual, expected) << tts::as_string(in); }
    else { TTS_EQUAL(-1, expected) << tts::as_string(in); }
  };

  TTS_EQUAL(eve::detail::idxm::is_rotate({}), 0);

  test(std::array {0, 1}, 0);
  test(std::array {1, 0}, 1);
  test(std::array {1, we_}, 1);

  test(std::array {0, 1, 2, 3}, 0);
  test(std::array {0, we_, 2, 3}, 0);
  test(std::array {we_, we_, 2, 3}, 0);

  test(std::array {3, 0, 1, 2}, 1);
  test(std::array {3, we_, 1, 2}, 1);
  test(std::array {2, 3, we_, 1}, 2);

  test(std::array {0, 0}, -1);
  test(std::array {0, 0, 0, 0}, -1);
  test(std::array {1, 2, 3, 1}, -1);
};

TTS_CASE("slice_pattern")
{
  TTS_TYPE_IS((kumi::tuple<eve::pattern_t<0>>),
              decltype(eve::detail::idxm::slice_pattern<1>(eve::pattern<0>)));

  TTS_TYPE_IS((kumi::tuple<eve::pattern_t<0>>),
              decltype(eve::detail::idxm::slice_pattern<2>(eve::pattern<0>)));

  TTS_TYPE_IS((kumi::tuple<eve::pattern_t<1>, eve::pattern_t<0>>),
              decltype(eve::detail::idxm::slice_pattern<1>(eve::pattern<1, 0>)));

  TTS_TYPE_IS((kumi::tuple<eve::pattern_t<0, 1>, eve::pattern_t<2, 3>, eve::pattern_t<4, 5>>),
              decltype(eve::detail::idxm::slice_pattern<2>(eve::pattern<0, 1, 2, 3, 4, 5>)));
};

TTS_CASE("drop_unused_regiters")
{
  // Can't use eq because not printing properly
  TTS_EXPECT((kumi::tuple {eve::pattern<0>, std::array {0}}
              == eve::detail::idxm::drop_unused_wides<1>(eve::pattern<0>)));
  TTS_EXPECT((kumi::tuple {eve::pattern<0>, std::array {1}}
              == eve::detail::idxm::drop_unused_wides<1>(eve::pattern<1>)));
  TTS_EXPECT((kumi::tuple {eve::pattern<0, 1>, std::array {0, 1}}
              == eve::detail::idxm::drop_unused_wides<1>(eve::pattern<0, 1>)));

  TTS_EXPECT((kumi::tuple {eve::pattern<0, 1, 0, 1>, std::array {0}}
              == eve::detail::idxm::drop_unused_wides<2>(eve::pattern<0, 1, 0, 1>)));

  TTS_EXPECT((kumi::tuple {eve::pattern<0, 2, 0, 3>, std::array {1, 2}}
              == eve::detail::idxm::drop_unused_wides<2>(eve::pattern<2, 4, 2, 5>)));

  TTS_EXPECT((kumi::tuple {eve::pattern<0, 2, eve::we_, eve::na_>, std::array {1, 2}}
              == eve::detail::idxm::drop_unused_wides<2>(eve::pattern<2, 4, eve::we_, eve::na_>)));

  TTS_EXPECT((kumi::tuple {eve::pattern<eve::we_>, std::array {0}}
              == eve::detail::idxm::drop_unused_wides<2>(eve::pattern<eve::we_>)));

  TTS_EXPECT((kumi::tuple {eve::pattern<eve::na_>, std::array {0}}
              == eve::detail::idxm::drop_unused_wides<2>(eve::pattern<eve::na_>)));
};

TTS_CASE("matches 1")
{
  auto test = [](auto _in, std::initializer_list<std::ptrdiff_t> p, bool expected)
  {
    auto in     = to_idxs(_in);
    bool actual = eve::detail::idxm::matches(to_idxs(_in), p);
    TTS_EQUAL(expected, actual) << "in: " << tts::as_string(in) << " p: " << tts::as_string(p);
  };

  test(std::array {we_}, {0}, true);
  test(std::array {we_}, {1}, true);
  test(std::array {na_, na_}, {na_, na_}, true);
  test(std::array {1, we_}, {we_, 1}, true);
  test(std::array {1, 2}, {1, 2}, true);
  test(std::array {we_}, {1, 2}, false);
  test(std::array {we_, na_}, {1, na_}, true);
  test(std::array {1, 2, 3, 4}, {1, 2, we_, 4}, true);
  test(std::array {1, 2, 3, 4}, {1, 2, 4, we_}, false);
};

TTS_CASE("matches many")
{
  auto test = [](auto                                  _in,
                 std::initializer_list<std::ptrdiff_t> p0,
                 std::initializer_list<std::ptrdiff_t> p1,
                 bool                                  expected)
  {
    auto in     = to_idxs(_in);
    bool actual = eve::detail::idxm::matches(to_idxs(_in), p0, p1);
    TTS_EQUAL(expected, actual) << "in: " << tts::as_string(in) << " p0: " << tts::as_string(p0)
                                << " p1: " << tts::as_string(p1);
  };

  test(std::array {1}, {0}, {we_}, true);
  test(std::array {1}, {1}, {na_}, true);
  test(std::array {1, 2}, {1, na_}, {na_, 2}, false);
};

TTS_CASE("repace_we")
{
  auto test = [](auto _in, std::ptrdiff_t with, auto _expected)
  {
    auto in       = to_idxs(_in);
    auto expected = to_idxs(_expected);
    auto actual = eve::detail::idxm::replace_we(in, with);
    TTS_EQUAL(expected, actual) << "with: " << with;
  };

  test(std::array{0, 1}, 1, std::array{0, 1});
  test(std::array{0, we_}, 1, std::array{0, 1});
  test(std::array{we_, we_}, 1, std::array{1, 1});
};

TTS_CASE("is_blend")
{
  auto test = [](auto _in, std::ptrdiff_t cardinal, bool expected)
  {
    auto in       = to_idxs(_in);
    auto actual = eve::detail::idxm::is_blend(in, cardinal);
    TTS_EQUAL(expected, actual) << tts::as_string(in) << "cardinal: " << cardinal;
  };
  test(std::array{0, 1}, 2, true);
  test(std::array{2, 1}, 2, true);
  test(std::array{2, 3}, 2, true);
  test(std::array{0, 3}, 2, true);
  test(std::array{4, we_, 2, 3}, 4, true);
  test(std::array{4, na_, 2, 3}, 4, false);
  test(std::array{4, 5, 2, 3}, 2, false);
};

TTS_CASE("expand_group")
{
  auto test = []<std::ptrdiff_t G>(auto _in, eve::fixed<G>, auto _expected)
  {
    auto in       = to_idxs(_in);
    auto expected = to_idxs(_expected);
    auto actual = eve::detail::idxm::expand_group<G>(in);
    TTS_EQUAL(expected, actual) << "G: " << G;
  };

  test(std::array{0, 1}, eve::lane<1>, std::array{0, 1});
  test(std::array{0, na_}, eve::lane<2>, std::array{0, 1, na_, na_});
  test(std::array{1, 0}, eve::lane<2>, std::array{2, 3, 0, 1});
  test(std::array{0, 1}, eve::lane<4>, std::array{0, 1, 2, 3, 4, 5, 6, 7});
};

}
