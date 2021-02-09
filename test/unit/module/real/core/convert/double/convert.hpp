//==================================================================================================
/**
  EVE - Expressive Vector Engine
  Copyright 2020 Joel FALCOU
  Copyright 2020 Jean-Thierry LAPRESTE

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <eve/function/convert.hpp>
#include <eve/function/converter.hpp>
#include <eve/constant/valmin.hpp>
#include <eve/constant/valmax.hpp>
#include <type_traits>

TTS_CASE_TPL("Check eve::convert return type", EVE_TYPE)
{
#if defined(EVE_SIMD_TESTS)
  using target_t = eve::wide<double, eve::fixed<EVE_CARDINAL>>;
#else
  using target_t = double;
#endif

  TTS_EXPR_IS(eve::convert(T(), eve::as<double>()), target_t);
  TTS_EXPR_IS(eve::float64(T()), target_t);
}

TTS_CASE_TPL("Check eve::convert arithmetic behavior", EVE_TYPE)
{
#if defined(EVE_SIMD_TESTS)
  using target_t = eve::wide<double, eve::fixed<EVE_CARDINAL>>;
#else
  using target_t = double;
#endif
  using v_t = eve::element_type_t<T>;

  TTS_EQUAL(eve::convert(eve::valmin(eve::as<T>()), eve::as<double>()), static_cast<target_t>(eve::valmin(eve::as<v_t>())) );
  TTS_EQUAL(eve::convert((T(0))          , eve::as<double>()), static_cast<target_t>(0) );
  TTS_EQUAL(eve::convert((T(42.69))      , eve::as<double>()), static_cast<target_t>(v_t(42.69)) );
  TTS_EQUAL(eve::convert(eve::valmax(eve::as<T>()), eve::as<double>()), static_cast<target_t>(eve::valmax(eve::as<v_t>())) );
}

TTS_CASE_TPL("Check eve::convert logical behavior", EVE_TYPE)
{
#if defined(EVE_SIMD_TESTS)
  using target_t = eve::logical<eve::wide<double, eve::fixed<EVE_CARDINAL>>>;
#else
  using target_t = eve::logical<double>;
#endif

  TTS_EQUAL(eve::convert(eve::logical<T>(true)  , eve::as<eve::logical<double>>()), target_t(true) );
  TTS_EQUAL(eve::convert(eve::logical<T>(false) , eve::as<eve::logical<double>>()), target_t(false) );

#if defined(EVE_SIMD_TESTS)
  eve::logical<T> mixed( [](auto i, auto) { return i%2 == 0;});
  target_t        ref( [](auto i, auto) { return i%2 == 0;});

  TTS_EQUAL(eve::convert(mixed, eve::as<eve::logical<double>>()), ref );
#endif
}
