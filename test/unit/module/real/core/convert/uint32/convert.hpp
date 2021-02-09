//==================================================================================================
/**
  EVE - Expressive Vector Engine
  Copyright 2020 Joel FALCOU
  Copyright 2020 Jean-Thierry LAPRESTE

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <eve/function/converter.hpp>
#include <eve/constant/valmin.hpp>
#include <eve/constant/valmax.hpp>
#include <type_traits>

TTS_CASE_TPL("Check eve::convert return type", EVE_TYPE)
{
#if defined(EVE_SIMD_TESTS)
  using target_t = eve::wide<std::uint32_t, eve::fixed<EVE_CARDINAL>>;
#else
  using target_t = std::uint32_t;
#endif

  TTS_EXPR_IS(eve::convert(T(), eve::as<std::uint32_t>()), target_t);
  TTS_EXPR_IS(eve::uint32(T())     , target_t);
}

TTS_CASE_TPL("Check eve::convert behavior", EVE_TYPE)
{
#if defined(EVE_SIMD_TESTS)
  using target_t = eve::wide<std::uint32_t, eve::fixed<EVE_CARDINAL>>;
#else
  using target_t = std::uint32_t;
#endif

  using v_t = eve::element_type_t<T>;

  TTS_EQUAL(eve::convert((T(0))          , eve::as<std::uint32_t>()), static_cast<target_t>(0) );
  TTS_EQUAL(eve::convert((T(42.69))      , eve::as<std::uint32_t>()), static_cast<target_t>(v_t(42.69)) );

  if constexpr(eve::integral_value<T>)
  {
    // with floating value this test produces undefined behaviour
    TTS_EQUAL(eve::convert(eve::valmin(eve::as<T>()), eve::as<std::uint32_t>()), static_cast<target_t>(eve::valmin(eve::as<v_t>())) );
    TTS_EQUAL(eve::convert(eve::valmax(eve::as<T>()), eve::as<std::uint32_t>()), static_cast<target_t>(eve::valmax(eve::as<v_t>())) );
  }
}

TTS_CASE_TPL("Check eve::convert logical behavior", EVE_TYPE)
{
#if defined(EVE_SIMD_TESTS)
  using target_t = eve::logical<eve::wide<std::uint32_t, eve::fixed<EVE_CARDINAL>>>;
#else
  using target_t = eve::logical<std::uint32_t>;
#endif

  TTS_EQUAL(eve::convert(eve::logical<T>(true)  , eve::as<eve::logical<std::uint32_t>>()), target_t(true) );
  TTS_EQUAL(eve::convert(eve::logical<T>(false) , eve::as<eve::logical<std::uint32_t>>()), target_t(false) );

#if defined(EVE_SIMD_TESTS)
  eve::logical<T> mixed( [](auto i, auto) { return i%2 == 0;});
  target_t        ref( [](auto i, auto) { return i%2 == 0;});

  TTS_EQUAL(eve::convert(mixed, eve::as<eve::logical<std::uint32_t>>()), ref );
#endif
}
