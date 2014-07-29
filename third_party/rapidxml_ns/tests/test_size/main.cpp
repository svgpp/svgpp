#define RAPIDXML_NO_EXCEPTIONS
#include "../../rapidxml_ns.hpp"

int error;

namespace rapidxml_ns
{
    void parse_error_handler(const char *, void *) 
    { 
        exit(0);
    }
}

using namespace rapidxml_ns;

int main(int argc, char *argv[])
{
    xml_document<char> doc;
    doc.parse<0>(argv[0]);
    return 1;
}
