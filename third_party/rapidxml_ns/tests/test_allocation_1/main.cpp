#define RAPIDXML_STATIC_POOL_SIZE 64 * 1024
#define RAPIDXML_DYNAMIC_POOL_SIZE 1 * 1024

#include "../test_utils.hpp"
#include "../../rapidxml_ns.hpp"
#include "../../rapidxml_ns_utils.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring>

using namespace std;
using namespace rapidxml_ns;

#define MAGIC "Dead beef!"

void test_small_dynamic()
{
    // Load file
    file<> f("../xml_files/simple_all.xml");

    char magic1[] = MAGIC;       // Static pool memory corruption detection
    xml_document<> doc;
    char magic2[] = MAGIC;       // Static pool memory corruption detection

    // Verify no memory corruption
    CHECK(strcmp(magic1, MAGIC) == 0);
    CHECK(strcmp(magic2, MAGIC) == 0);

    // Parse multiple times
    for (int i = 0; i < 100; ++i)
        doc.parse<parse_non_destructive>(f.data());
    
    // Verify no memory corruption
    CHECK(strcmp(magic1, MAGIC) == 0);
    CHECK(strcmp(magic2, MAGIC) == 0);

    doc.clear();
    
    // Verify no memory corruption
    CHECK(strcmp(magic1, MAGIC) == 0);
    CHECK(strcmp(magic2, MAGIC) == 0);

    // Parse multiple times
    for (int i = 0; i < 100; ++i)
        doc.parse<parse_non_destructive>(f.data());

    // Verify no memory corruption
    CHECK(strcmp(magic1, MAGIC) == 0);
    CHECK(strcmp(magic2, MAGIC) == 0);
}

int main()
{
    test_small_dynamic();
    return test::final_results();
}
