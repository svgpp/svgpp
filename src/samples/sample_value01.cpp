#include <svgpp/svgpp.hpp>

using namespace svgpp;

struct Context
{
  void set(tag::attribute::amplitude, double value)
  {
    std::cout << value << "\n";
  }
};

int main()
{
  Context context;
  value_parser<tag::type::number>::parse(tag::attribute::amplitude(), context, std::string("3.14"), tag::source::attribute());
  return 0;
}