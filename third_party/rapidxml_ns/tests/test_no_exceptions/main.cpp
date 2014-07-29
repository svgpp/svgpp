// This test ensures that RAPIDXML_NO_EXCEPTIONS flag works properly

#define RAPIDXML_NO_EXCEPTIONS
#include "../../rapidxml_ns.hpp"
#include <iostream>

using namespace std;
using namespace rapidxml_ns;

void rapidxml_ns::parse_error_handler(const char *, void *)
{
    cout << "*** Success\n";
    exit(0);    // Cannot return from this function
}

int main()
{
    try
    {
        char xml[] = "<root>where's my closing markup...?";
        xml_document<char> doc;
        doc.parse<0>(xml);
        cout << "*** Failed - rapidxml_ns::parse_error_handler not called despite RAPIDXML_NO_EXCEPTIONS defined\n";
        return 1;
    }
    catch (...)
    {
        cout << "*** Failed - exception thrown despite RAPIDXML_NO_EXCEPTIONS defined\n";
        return 1;
    }

}
