#include <svgpp/svgpp.hpp>

using namespace svgpp;

namespace svgpp { namespace policy { namespace value_events 
{
  template<>
  struct default_policy<boost::optional<double> >
  {
    template<class AttributeTag>
    static void set(boost::optional<double> & context, AttributeTag tag, tag::source::any const &, double value) 
    {
      context = value;
    }
  };
}}}

int main()
{
  boost::optional<double> context;
  value_parser<tag::type::number>::parse(tag::attribute::amplitude(), context, std::string("3.14"), tag::source::attribute());
  if (context)
    std::cout << *context << "\n";
  return 0;
}