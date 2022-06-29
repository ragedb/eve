//==================================================================================================
/**
  EVE - Expressive Vector Engine
  Copyright : EVE Contributors & Maintainers
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include "test.hpp"
#include "measures.hpp"
#include <eve/detail/meta.hpp>
#include <eve/module/complex.hpp>
#include <complex>

template < typename T >
auto cv(std::complex < T > sc)
{
  return eve::complex<T>(sc.real(), sc.imag());
}

TTS_CASE_WITH( "Check behavior of cos on scalar"
        , tts::bunch<eve::test::scalar::ieee_reals>
        , tts::generate( tts::randoms(-10, 10)
                             , tts::randoms(-10, 10))
        )
  <typename T>(T const& a0, T const& a1 )
{
  using e_t = typename T::value_type;
  using c_t = std::complex<e_t>;
  for(auto e : a0)
  {
    for(auto f : a1)
    {
      std::cout << "e = " << e << " f =  " <<  f << std::endl;
      TTS_ULP_EQUAL(eve::cos(eve::complex<e_t>(e, f)),  cv(std::cos(c_t(e, f))), 2);
      TTS_ULP_EQUAL(eve::pedantic(eve::cos)(eve::complex<e_t>(e, f)),  cv(std::cos(c_t(e, f))), 2);
      TTS_ULP_EQUAL(eve::cos(eve::complex<e_t>(e, f)),  cv(std::cos(c_t(e, f))), 2.0);
    }
  }
};

TTS_CASE_WITH( "Check behavior of cos on wide"
        , eve::test::simd::ieee_reals
        , tts::generate( tts::randoms(-10, 10)
                             , tts::randoms(-10, 10))
        )
  <typename T>(T const& a0, T const& a1 )
{
  using e_t = typename T::value_type;
  using ce_t = eve::complex<e_t>;
  using z_t = eve::as_complex_t<T>;
  using c_t = std::complex<e_t>;
  auto std_ch = [](auto x, auto y){return std::cos(c_t(x, y)); };
  auto init_with_std = [std_ch](auto a0,  auto a1){
    z_t b;
    for(int i = 0; i !=  eve::cardinal_v<T>; ++i)
    {
      ce_t z = cv(std_ch(a0.get(i), a1.get(i)));
      b.set(i, z);
    }
    return b;
  };
  TTS_ULP_EQUAL(eve::cos(z_t{a0,a1}), init_with_std(a0, a1), 2);
};


TTS_CASE_WITH( "Check behavior of pedantic(cos) on wide"
        , eve::test::simd::ieee_reals
        , tts::generate(tts::randoms(-10, 10)
                             , tts::randoms(-10, 10))
        )
  <typename T>(T const& a0, T const& a1 )
{
  using e_t = typename T::value_type;
  using ce_t = eve::complex<e_t>;
  using z_t = eve::as_complex_t<T>;
  using c_t = std::complex<e_t>;
  z_t b;
  for(int i = 0; i !=  eve::cardinal_v<T>; ++i)
  {
    ce_t z = cv(std::cos(c_t(a0.get(i), a1.get(i))));
    b.set(i, z);
  }
  TTS_ULP_EQUAL(eve::pedantic(eve::cos)(z_t{a0,a1}), b, 2.0);
};

TTS_CASE_TPL( "Check return types of eve::cos", tts::bunch<eve::test::scalar::ieee_reals>)
  <typename T>(tts::type<T>)
{
  using e_t = typename T::value_type;
  using c_t = eve::complex<e_t>;
  using eve::as;
  const int N = 24;
  e_t pid = eve::downward(eve::pi)(as<e_t>()); // nearest pi representation less than mathematical pi
  e_t piu = eve::upward(eve::pi)(as<e_t>());   // nearest pi representation greater than mathematical pi
  std::array<c_t, N> inputs =
    { c_t(eve::zero(as<e_t>()),eve::zero(as<e_t>())),//0
      c_t(eve::inf(as<e_t>()),eve::zero(as<e_t>())), //1
      c_t(eve::minf(as<e_t>()),eve::zero(as<e_t>())),//2
      c_t(eve::nan(as<e_t>()),eve::zero(as<e_t>())), //3
      c_t(eve::zero(as<e_t>()),eve::inf(as<e_t>())), //4
      c_t(eve::inf(as<e_t>()),eve::inf(as<e_t>())),  //5
      c_t(eve::minf(as<e_t>()),eve::inf(as<e_t>())), //6
      c_t(eve::nan(as<e_t>()),eve::inf(as<e_t>())),  //7
      c_t(eve::zero(as<e_t>()),eve::minf(as<e_t>())),//8
      c_t(eve::inf(as<e_t>()),eve::minf(as<e_t>())), //9
      c_t(eve::minf(as<e_t>()),eve::minf(as<e_t>())),//10
      c_t(eve::nan(as<e_t>()),eve::minf(as<e_t>())), //11
      c_t(eve::zero(as<e_t>()),eve::nan(as<e_t>())), //12
      c_t(eve::inf(as<e_t>()),eve::nan(as<e_t>())),  //13
      c_t(eve::minf(as<e_t>()),eve::nan(as<e_t>())), //14
      c_t(eve::nan(as<e_t>()),eve::nan(as<e_t>())),  //15
      c_t(eve::zero(as<e_t>()),pid),                 //16
      c_t(eve::inf(as<e_t>()),pid),                  //17
      c_t(eve::minf(as<e_t>()),pid),                 //18
      c_t(eve::nan(as<e_t>()),pid),                  //19
      c_t(eve::zero(as<e_t>()),piu),                 //20
      c_t(eve::inf(as<e_t>()),piu),                  //21
      c_t(eve::minf(as<e_t>()),piu),                 //22
      c_t(eve::nan(as<e_t>()),piu),                  //23
    };

  auto cosine = eve::pedantic(eve::cos);
  auto tcosine= [](auto z){
    return eve::pedantic(eve::cosh)(eve::i*z);
  };
  for(int i=0; i < N; ++i)
  {
    TTS_IEEE_EQUAL(cosine(-inputs[i]), cosine(inputs[i]));
    TTS_IEEE_EQUAL(cosine(inputs[i]), tcosine(inputs[i]));
  }
};