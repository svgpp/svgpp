// Revision $DateTime: 2007/02/25 13:06:01 $
//! \file test_utils.hpp This file contains rapidxml testing utilities

#ifndef TEST_UTILS_HPP_INCLUDED
#define TEST_UTILS_HPP_INCLUDED

#include <iostream>
#include <cstdlib>

class test
{

public:

    static int &succeeded()
    {
        static int n;
        return n;
    }

    static int &failed()
    {
        static int n;
        return n;
    }

    static int final_results()
    {
        if (failed() + succeeded() > 0)
        {
            std::cout << "\n" 
                      << (failed() ? "*** Failure (" : "*** Success (")
                      << succeeded() << "/" << failed() + succeeded() << " checks succeeded)\n";
        }
        else
        {
            std::cout << "*** Success\n";
        }
        return (failed() == 0) ? 0 : 1;
    }

};

#define CHECK(c)                                        \
    if (!(c))                                           \
    {                                                   \
        std::cout << "Check " << #c << " failed.\n";    \
        ++test::failed();                               \
    }                                                   \
    else                                                \
        ++test::succeeded();

#define REQUIRE(c)                                                              \
    if (!(c))                                                                   \
    {                                                                           \
        std::cout << "Requirement " << #c << " failed, aborting test.\n";       \
        ++test::failed();                                                       \
        std::exit(test::final_results());                                       \
    }                                                                           \
    else                                                                        \
        ++test::succeeded();

#endif
