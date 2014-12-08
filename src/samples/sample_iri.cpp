#include <svgpp/svgpp.hpp>

using namespace svgpp;

struct Context
{
  template<class String>
  void set(tag::attribute::xlink::href, String const & iri)
  {
    std::cout << "IRI: " << std::string(boost::begin(iri), boost::end(iri)) << "\n";
  }

  template<class String>
  void set(tag::attribute::xlink::href, tag::iri_fragment, String const & iri)
  {
    std::cout << "Fragment: " << std::string(boost::begin(iri), boost::end(iri)) << "\n";
  }
};

int main()
{
  Context context;
  value_parser<tag::type::iri>::parse(tag::attribute::xlink::href(), context,
    std::string("http://foo.com/bar#123"), tag::source::attribute());
  value_parser<tag::type::iri>::parse(tag::attribute::xlink::href(), context,
    std::string("#rect1"), tag::source::attribute());
  // Output:
  //  "IRI: http://foo.com/bar#123"
  //  "Fragment: rect1"
  return 0;
}