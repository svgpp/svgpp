#include "../test_utils.hpp"
#include "../../rapidxml_ns.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdexcept>

using namespace rapidxml_ns;
using namespace std;

// Tests parsing a valid document which is progressively truncated by 1 character
// up to reaching size of 0. This should not crash the parser.
template<int Flags>
void test_progressive_truncations()
{
    // Load file
    string filename("../xml_files/simple_all.xml");
    ifstream stream(filename.c_str(), ios::binary);
    if (!stream)
        throw runtime_error(string("cannot open file ") + filename);
    stream.unsetf(ios::skipws);
    stream.seekg(0, ios::end);
    size_t size = stream.tellg();
    stream.seekg(0);   
    vector<char> original(size + 1);
    stream.read(&original.front(), static_cast<streamsize>(size));

    // Test for crashes with progressive truncation
    cout << "Testing for crashes using progressive trunc, Flags=" << Flags << "...\n";
    int count = 0;
    for (int end = int(size); end >= 0; --end)
    {
        try
        {
            vector<char> data(original);    // Make a copy
            data[end] = '\0';               // Truncate
            rapidxml_ns::xml_document<char> doc;
            doc.parse<Flags>(&data.front());
        }
        catch (rapidxml_ns::parse_error &)
        {
            // Ignore errors
            ++count;
        }
    }

    // Success
    cout << "Progressive truncation succeeded, " << count << " errors in " << original.size() << " tries\n";
}

// Tests parsing a valid document which is mutated by inserting random garbage
// This should not crash the parser
template<int Flags>
void test_random_mutations()
{
    const int num_mutations = 10000;
    const int max_garbage_chars_per_mutation = 100;

    // Initialize random generator
    srand(77);
    
    // Load file
    string filename("../xml_files/simple_all.xml");
    ifstream stream(filename.c_str(), ios::binary);
    if (!stream)
        throw runtime_error(string("cannot open file ") + filename);
    stream.unsetf(ios::skipws);
    stream.seekg(0, ios::end);
    size_t size = stream.tellg();
    stream.seekg(0);   
    vector<char> original(size + 1);
    stream.read(&original.front(), static_cast<streamsize>(size));

    // Perform mutations
    cout << "Testing for crashes using random mutations, Flags=" << Flags << "...\n";
    int count = 0;
    for (int i = 0; i < num_mutations; ++i)
    {
        // Make a copy of data
        vector<char> data(original);
        
        // Mutate
        int num_garbage_chars = rand() % max_garbage_chars_per_mutation + 1;
        for (int j = 0; j < num_garbage_chars; ++j)
            data[rand() % size] = static_cast<char>(rand() % 256);         // Make sure that ending Ch('\0') remains untouched

        // Parse
        try
        {
            rapidxml_ns::xml_document<char> doc;
            doc.parse<Flags>(&data.front());
        }
        catch (rapidxml_ns::parse_error &)
        {
            // Ignore errors
            ++count;
        }

    }
    
    // Success
    cout << "Random Mutations succeeded, " << count << " errors in " << num_mutations << " tries\n";

}

int main()
{
    cout << "NOTE: this test will crash or loop indefinitely when it fails\n\n";
    
    test_progressive_truncations<parse_fastest>();
    test_progressive_truncations<parse_non_destructive>();
    test_progressive_truncations<0>();
    test_progressive_truncations<parse_trim_whitespace>();
    test_progressive_truncations<parse_normalize_whitespace>();
    test_progressive_truncations<parse_trim_whitespace | parse_normalize_whitespace>();
    test_progressive_truncations<parse_full>();
    
    test_random_mutations<parse_fastest>();
    test_random_mutations<parse_non_destructive>();
    test_random_mutations<0>();
    test_random_mutations<parse_trim_whitespace>();
    test_random_mutations<parse_normalize_whitespace>();
    test_random_mutations<parse_trim_whitespace | parse_normalize_whitespace>();
    test_random_mutations<parse_full>();

    return test::final_results();
}
