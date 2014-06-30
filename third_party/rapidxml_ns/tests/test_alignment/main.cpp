#include "../test_utils.hpp"
#include "../../rapidxml_ns.hpp"
#include "../../rapidxml_ns_utils.hpp"
#include <cstring>
#include <map>

using namespace std;
using namespace rapidxml_ns;

void test_alignment()
{
    memory_pool<> pool;

    char *ptr = 0;
    for (int i = 0; i < 100; ++i)
    {
        char *new_ptr = pool.allocate_string(0, 1);
        if (ptr)
        {
            CHECK(new_ptr - ptr == RAPIDXML_ALIGNMENT);
        }
        ptr = new_ptr;
    }

    map<char *, int> allocs;
    for (int size = 1; size <= 128; ++size)
    {
        for (int i = 0; i < 100; ++i)
        {
            char *ptr = pool.allocate_string(0, size);
            CHECK((std::size_t(ptr) & (RAPIDXML_ALIGNMENT - 1)) == 0);
            REQUIRE(allocs.count(ptr) == 0);
            allocs[ptr] = size;
            memset(ptr, (unsigned char)size, size);
        }
    }

    for (map<char *, int>::iterator it = allocs.begin(); it != allocs.end(); ++it)
        for (int i = 0; i < it->second; ++i)
            CHECK((unsigned char)it->first[i] == it->second);
}

int main()
{
    test_alignment();
    return test::final_results();
}
