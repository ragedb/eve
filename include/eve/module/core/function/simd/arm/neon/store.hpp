//==================================================================================================
/**
  EVE - Expressive Vector Engine
  Copyright 2018 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef EVE_MODULE_CORE_FUNCTION_SIMD_ARM_NEON_STORE_HPP_INCLUDED
#define EVE_MODULE_CORE_FUNCTION_SIMD_ARM_NEON_STORE_HPP_INCLUDED

#include <eve/detail/overload.hpp>
#include <eve/detail/abi.hpp>
#include <eve/memory/aligned_ptr.hpp>
#include <eve/arch/limits.hpp>
#include <eve/forward.hpp>

namespace eve { namespace detail
{
  template<typename T, typename N>
  EVE_FORCEINLINE void store_ ( EVE_SUPPORTS(neon128_)
                              , pack<T,N,neon64_> const& value, T* ptr
                              , std::enable_if_t<std::is_arithmetic_v<T>>* = 0
                              ) noexcept
  {
    if constexpr(N::value*sizeof(T) == limits<neon64_>::bytes)
    {
      #if defined(__aarch64__)
      if constexpr( std::is_same_v<T,double> )  vst1_f64(ptr,value);
      #endif

      if constexpr( std::is_same_v<T,float> ) vst1_f32(ptr,value);

      if constexpr( std::is_integral_v<T> )
      {
        if constexpr( std::is_signed_v<T> )
        {
          if constexpr( sizeof(T) == 8 )  vst1_s64(ptr,value);
          if constexpr( sizeof(T) == 4 )  vst1_s32(ptr,value);
          if constexpr( sizeof(T) == 2 )  vst1_s16(ptr,value);
          if constexpr( sizeof(T) == 1 )  vst1_s8(ptr,value);
        }
        else
        {
          if constexpr( sizeof(T) == 8 )  vst1_u64(ptr,value);
          if constexpr( sizeof(T) == 4 )  vst1_u32(ptr,value);
          if constexpr( sizeof(T) == 2 )  vst1_u16(ptr,value);
          if constexpr( sizeof(T) == 1 )  vst1_u8(ptr,value);
        }
      }
    }
    else
      apply<N::value>( [&](auto... I) { ((*ptr++ = value[I]), ...); } );
  }

  template<typename T, typename N>
  EVE_FORCEINLINE void store_ ( EVE_SUPPORTS(neon128_)
                              , pack<T,N,neon128_> const& value, T* ptr
                              , std::enable_if_t<std::is_arithmetic_v<T>>* = 0
                              ) noexcept
  {
    #if defined(__aarch64__)
    if constexpr( std::is_same_v<T,double> )  vst1q_f64(ptr,value);
    #endif

    if constexpr( std::is_same_v<T,float> )  vst1q_f32(ptr,value);

    if constexpr( std::is_integral_v<T> )
    {
      if constexpr( std::is_signed_v<T> )
      {
        if constexpr( sizeof(T) == 8 )  vst1q_s64(ptr,value);
        if constexpr( sizeof(T) == 4 )  vst1q_s32(ptr,value);
        if constexpr( sizeof(T) == 2 )  vst1q_s16(ptr,value);
        if constexpr( sizeof(T) == 1 )  vst1q_s8(ptr,value);
      }
      else
      {
        if constexpr( sizeof(T) == 8 )  vst1q_u64(ptr,value);
        if constexpr( sizeof(T) == 4 )  vst1q_u32(ptr,value);
        if constexpr( sizeof(T) == 2 )  vst1q_u16(ptr,value);
        if constexpr( sizeof(T) == 1 )  vst1q_u8(ptr,value);
      }
    }
  }

#if defined(EVE_COMP_IS_MSVC)
  template<typename T, typename S, std::size_t N>
  EVE_FORCEINLINE void store_ ( EVE_SUPPORTS(neon128_)
                              , pack<T,S,neon64_> const& value, aligned_ptr<T,N> ptr
                              , std::enable_if_t<std::is_arithmetic_v<T>>* = 0
                              ) noexcept
  {
    if constexpr( N >= limits<neon64_>::bytes )
    {
      #if defined(__aarch64__)
      if constexpr( std::is_same_v<T,double> )  vst1_f64_ex(ptr,value,64);
      #endif

      if constexpr( std::is_same_v<T,float> )  vst1_f32_ex(ptr,value,64);

      if constexpr( std::is_integral_v<T> )
      {
        if constexpr( std::is_signed_v<T> )
        {
          if constexpr( sizeof(T) == 8 )  vst1_s64_ex(ptr,value,64);
          if constexpr( sizeof(T) == 4 )  vst1_s32_ex(ptr,value,64);
          if constexpr( sizeof(T) == 2 )  vst1_s16_ex(ptr,value,64);
          if constexpr( sizeof(T) == 1 )  vst1_s8_ex(ptr,value,64);
        }
        else
        {
          if constexpr( sizeof(T) == 8 )  vst1_u64_ex(ptr,value,64);
          if constexpr( sizeof(T) == 4 )  vst1_u32_ex(ptr,value,64);
          if constexpr( sizeof(T) == 2 )  vst1_u16_ex(ptr,value,64);
          if constexpr( sizeof(T) == 1 )  vst1_u8_ex(ptr,value,64);
        }
      }
    }
    else
      store(value, ptr.get());
  }

  template<typename T, typename S, std::size_t N>
  EVE_FORCEINLINE void store_ ( EVE_SUPPORTS(neon128_)
                              , pack<T,S,neon128_> const& value, aligned_ptr<T,N> ptr
                              , std::enable_if_t<std::is_arithmetic_v<T>>* = 0
                              ) noexcept
  {
    if constexpr( N >= limits<neon128_>::bytes )
    {
      #if defined(__aarch64__)
      if constexpr( std::is_same_v<T,double> )  vst1q_f64_ex(ptr,value,128);
      #endif

      if constexpr( std::is_same_v<T,float> )  vst1q_f32_ex(ptr,value,128);

      if constexpr( std::is_integral_v<T> )
      {
        if constexpr( std::is_signed_v<T> )
        {
          if constexpr( sizeof(T) == 8 )  vst1q_s64_ex(ptr,value,128);
          if constexpr( sizeof(T) == 4 )  vst1q_s32_ex(ptr,value,128);
          if constexpr( sizeof(T) == 2 )  vst1q_s16_ex(ptr,value,128);
          if constexpr( sizeof(T) == 1 )  vst1q_s8_ex(ptr,value,128);
        }
        else
        {
          if constexpr( sizeof(T) == 8 )  vst1q_u64_ex(ptr,value,128);
          if constexpr( sizeof(T) == 4 )  vst1q_u32_ex(ptr,value,128);
          if constexpr( sizeof(T) == 2 )  vst1q_u16_ex(ptr,value,128);
          if constexpr( sizeof(T) == 1 )  vst1q_u8_ex(ptr,value,128);
        }
      }
    }
    else
      store(value, ptr.get());
  }
#else
  template<typename T, typename S, std::size_t N>
  EVE_FORCEINLINE void store_ ( EVE_SUPPORTS(neon128_)
                              , pack<T,S,neon64_> const& value, aligned_ptr<T,N> ptr
                              , std::enable_if_t<std::is_arithmetic_v<T>>* = 0
                              ) noexcept
  {
    store(value, ptr.get());
  }

  template<typename T, typename S, std::size_t N>
  EVE_FORCEINLINE void store_ ( EVE_SUPPORTS(neon128_)
                              , pack<T,S,neon128_> const& value, aligned_ptr<T,N> ptr
                              , std::enable_if_t<std::is_arithmetic_v<T>>* = 0
                              ) noexcept
  {
    store(value, ptr.get());
  }
#endif
} }

#endif