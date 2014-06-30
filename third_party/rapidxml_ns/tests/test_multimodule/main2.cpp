#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>
#include "../../rapidxml_ns.hpp"
#include "../../rapidxml_ns_print.hpp"

using namespace std;
using namespace rapidxml_ns;

void parse_and_print_2(const char *filename)
{
    // Load data
    ifstream stream(filename, ios::binary);
    stream.unsetf(ios::skipws);
    stream.seekg(0, ios::end);
    size_t size = stream.tellg();
    stream.seekg(0);   
    vector<char> data(size + 1);
    stream.read(&data.front(), static_cast<streamsize>(size));
    data[size] = '\0';

    // Parse and print
    xml_document<char> doc;
    doc.parse<0>(&data.front());
    print(ostream_iterator<char>(cout), doc);
}
