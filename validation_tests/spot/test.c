#include <iostream>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>

int main()
{
  std::cout << spot::parse_formula("[]<>p0 || <>[]p1") << '\n';
  spot::formula f = spot::parse_formula("& & G p0 p1 p2");
  print_latex_psl(std::cout, f) << '\n';
  print_lbt_ltl(std::cout, f) << '\n';
  print_spin_ltl(std::cout, f, true) << '\n';
  return 0;
}
