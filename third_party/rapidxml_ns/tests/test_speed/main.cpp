#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <stdexcept>

#define RAPIDXML_NO_EXCEPTIONS
#include "../../rapidxml_ns.hpp"
#include "../other_parsers/rapidxml/rapidxml.hpp"
#include "../other_parsers/pugixml/pugixml.hpp"

#include "../other_parsers/tinyxml/tinyxml.h"

#define PUGAPI_VARIANT 0x58475560
#define PUGAPI_VERSION_MAJOR 1
#define PUGAPI_VERSION_MINOR 2
#include "../other_parsers/pugxml/pugxml.h"

using namespace std;

// Buffer used for parsing
vector<char> buffer;

namespace rapidxml
{
    void parse_error_handler(const char *what, void *where_void) 
    { 
        // Print data around error
        char *where = reinterpret_cast<char *>(where_void);
        const char *begin = (std::max)(where - 50, &buffer.front());
        const char *end = (std::min)(where + 50, &buffer.back() + 1);
        int ch = *where;
        std::cout << ch << "\n";
        *where = '#';
        std::cout << "rapidxml::parse_error: " << what << " at \n*** BEGIN (error position marked by # character)\n"
                  << string(begin, end)
                  << "\n*** END\n";
        std::exit(1);
    }
}

namespace rapidxml_ns
{
    void parse_error_handler(const char *what, void *where_void) 
    {
       rapidxml::parse_error_handler(what, where_void);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Timing, only works on x86, MSVC or gcc

#if defined(_MSC_VER)

    typedef __int64 tick_t;
    inline tick_t ticks()
    {
        __asm __emit 0fh __asm __emit 031h   // RDTSC, result conveniently in EDX:EAX
    }

#elif defined(__GNUC__)

    typedef long long tick_t;
    inline tick_t ticks()
    {
        tick_t result;
        __asm__ __volatile__ ("rdtsc" : "=A"(result));
        return result;
    }

#else 
    #error "This test is only supported on MSVC or gcc, under x86"
#endif

///////////////////////////////////////////////////////////////////////////////
// Parsers

template<int Flags>
struct rapidxml_parser
{
    rapidxml::xml_document<char> doc;
    void parse(char *data)
    {
        doc.parse<Flags>(data);
    }
};

template<int Flags>
struct rapidxml_ns_parser
{
  rapidxml_ns::xml_document<char> doc;
  void parse(char *data)
  {
    doc.parse<Flags>(data);
  }
};

struct tinyxml_parser
{
    TiXmlDocument doc;
    void parse(char *data)
    {
        doc.Parse(data, 0, TIXML_ENCODING_UTF8);
    }
};
    
template<int Flags>
struct pugixml_parser
{
    pugi::xml_document doc;
    void parse(char *data)
    {
        doc.parse(data, Flags);
    }
};

template<int Flags>
struct pugxml_parser
{
    pug::xml_parser parser;
    void parse(char *data)
    {
        parser.parse(data, Flags);
    }
};

struct strlen_parser
{
    static size_t length;   // static to prevent it from being optimized away along with the timed strlen() call
    void parse(char *data)
    {
        length = strlen(data);
    }
};
size_t strlen_parser::length;

///////////////////////////////////////////////////////////////////////////////
// Test

template<class Parser>
void test(const char *filename, const char *description)
{
    // Load data
    ifstream stream(filename, ios::binary);
    if (!stream)
        throw runtime_error(string("cannot open file ") + filename);
    stream.unsetf(ios::skipws);
    stream.seekg(0, ios::end);
    size_t size = stream.tellg();
    stream.seekg(0);   
    vector<char> data(size + 1);
    stream.read(&data.front(), static_cast<streamsize>(size));
    data[size] = '\0';

    // Determine minimum CPU cycles it takes to parse test data
    // A very large number of mesurements is taken over several seconds.
    // The smallest one wins.
    // If parsing time is less than average time between interrupts,
    // there is a good chance that there will be at least one measurement
    // undisturbed by context switching and other OS activity.
    // A file of 50kB at 10 cycles/char takes 500,000 cycles to parse.
    // On 2 GHz CPU this is 1/4000 of a second.
    // During 2 seconds (taking into account restoring of the data), this file is parsed 
    // several thousands of times.
    tick_t min = 0;
    clock_t start = std::clock();
    while (std::clock() < start + 2 * CLOCKS_PER_SEC)   // 2 seconds
    {
        buffer = data;     // Make a copy of data (this must be done every time because parsing destroys the data)
        Parser parser;     // Creation and destruction of parser not timed
        char *xml = &buffer.front();
        tick_t t1 = ticks();    // 1st timing
        parser.parse(xml);
        tick_t t2 = ticks();    // 2nd timing
        if (min == 0 || t2 - t1 < min)
            min = t2 - t1;
    }
    
    // Return minimum cycles/character
    cout <<  "        " << fixed << setprecision(1) << double(min) / size << " cycles/char " << description << "\n";

}

void test_all(const char *filename)
{
    using namespace rapidxml;
    printf("\nFile %s\n", filename);
    printf("    strlen:\n");
    test<strlen_parser>(filename, "strlen() on XML data");
    printf("    rapidxml:\n");
    test<rapidxml_parser<parse_fastest> >(filename, "mode=fastest");
    test<rapidxml_parser<parse_default> >(filename, "mode=default");
    printf("    rapidxml_ns:\n");
    test<rapidxml_ns_parser<parse_fastest|rapidxml_ns::parse_no_namespace> >(filename, "mode=fastest");
    test<rapidxml_ns_parser<parse_default> >(filename, "mode=default");
    printf("    pugixml:\n");
    test<pugixml_parser<pugi::parse_minimal> >(filename, "mode=fastest");
    test<pugixml_parser<pugi::parse_default> >(filename, "mode=default");
    printf("    pugxml:\n");
    test<pugxml_parser<pug::parse_minimal> >(filename, "mode=fastest");
    printf("    tinyxml:\n");
    test<tinyxml_parser>(filename, "mode=default");
}

int main()
{
    try
    {
        test_all("../xml_files/speed_test.xml");
        return 0;
    }
    catch (exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
        return 1;
    }
}
