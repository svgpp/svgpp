#include "../../rapidxml_ns.hpp"
#include "../../rapidxml_ns_print.hpp"
#include "../../rapidxml_ns_utils.hpp"
#include <iostream>

int main()
{
    using namespace rapidxml_ns;
    using namespace std;

    xml_document<> doc;
    file<> f("test.xml");
    doc.parse<0>(f.data());
    static char buffer[4096];
    print(buffer, doc);
    cout << buffer;
    //print(cout, doc);
}
