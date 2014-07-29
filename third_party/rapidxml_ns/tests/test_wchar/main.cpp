#include "../../rapidxml_ns_utils.hpp"
#include "../test_utils.hpp"

using namespace std;
using namespace rapidxml_ns;

int main()
{
    xml_document<wchar_t> doc;
    file<wchar_t> xml("../xml_files/simple_all.xml");
    doc.parse<0>(xml.data());
    return test::final_results();
}
