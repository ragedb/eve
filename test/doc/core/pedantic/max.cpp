#include <eve/module/core.hpp>
#include <eve/wide.hpp>
#include <iostream>

using wide_ft = eve::wide<float, eve::fixed<8>>;
using eve::pedantic;

int main()
{
  wide_ft pf = {0.0f, 1.0f, -1.0f, -2.0f, 2.0f,
                eve::inf(eve::as<float>()), eve::minf(eve::as<float>()), eve::nan(eve::as<float>())};
  wide_ft qf = {4.0f, 1.0f, -1.0f,  0.0f, eve::nan(eve::as<float>()),
                -0.0f, eve::nan(eve::as<float>()), -2.0f};


  std::cout << "---- simd" << '\n'
            << "<- pf                     = " << pf << '\n'
            << "<- qf                     = " << qf << '\n'
            << "-> pedantic(max)(pf, qf)  = " << eve::pedantic(eve::max)(pf, qf) << '\n'
            << "-> numeric(max)(pf, qf)   = " << eve::numeric(eve::max)(pf, qf) << '\n';
  return 0;
}
