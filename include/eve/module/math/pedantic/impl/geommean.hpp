//==================================================================================================
/*
  EVE - Expressive Vector Engine
  Copyright : EVE Project Contributors
  SPDX-License-Identifier: BSL-1.0
*/
//==================================================================================================
#pragma once

#include <eve/module/core.hpp>
#include <eve/traits/common_value.hpp>

namespace eve::detail
{
template<floating_ordered_value T, floating_ordered_value U>
EVE_FORCEINLINE auto
geommean_(EVE_SUPPORTS(cpu_),
          pedantic_type const&,
          T a,
          U b) noexcept
-> common_value_t<T, U>
{
  return arithmetic_call(pedantic(geommean), a, b);
}

template<floating_ordered_value T>
EVE_FORCEINLINE T
geommean_(EVE_SUPPORTS(cpu_), pedantic_type const&, T a, T b) noexcept requires has_native_abi_v<T>
{
  auto m  = max(a, b);
  auto im = if_else(is_nez(m), rec(m), m);
  auto z  = min(a, b) * im;
  return if_else(is_nltz(a) || is_nltz(b), sqrt(z) * m, allbits);
}
//================================================================================================
// Masked case
//================================================================================================
template<conditional_expr C, floating_ordered_value U, floating_ordered_value V>
EVE_FORCEINLINE auto
geommean_(EVE_SUPPORTS(cpu_),
          C const& cond,
          pedantic_type const&,
          U const& t,
          V const& f) noexcept
-> decltype(geommean[c](t, f))
{
  return mask_op(cond, pedantic(eve::geommean), t, f);
}

}
