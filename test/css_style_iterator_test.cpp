#include <svgpp/parser/css_style_iterator.hpp>

#include <gtest/gtest.h>

void check_single_property(std::string const & style)
{
  svgpp::css_style_iterator<std::string::const_iterator> it(style.begin(), style.end());
  EXPECT_TRUE(!it.eof());
  EXPECT_EQ("name1", std::string(it->first.begin(), it->first.end()));
  EXPECT_EQ("value1", std::string(it->second.begin(), it->second.end()));
  ++it;
  EXPECT_TRUE(it.eof());
}

void check_single_property_w(std::wstring const & style)
{
  svgpp::css_style_iterator<std::wstring::const_iterator> it(style.begin(), style.end());
  EXPECT_TRUE(!it.eof());
  EXPECT_EQ(L"name1", std::wstring(it->first.begin(), it->first.end()));
  EXPECT_EQ(L"value1", std::wstring(it->second.begin(), it->second.end()));
  ++it;
  EXPECT_TRUE(it.eof());
}

TEST(css_style_iterator, valid1)
{
  std::string styles[] = {
    "name1:value1", 
    " name1: value1 ", 
    ";name1:value1;", 
    "; name1 :value1 ;  ",
    "; error; name1:value1", 
    "; error:  ; name1:value1", 
    "; :  error; name1:value1", 
    "; :  error; name1:value1;error:", 
    "; :  error; name1:value1;error:;", 
    ";;; :  error; ; :;;; name1:value1;error:;;;: ", 
  };
  for(std::string const * it = styles; it != boost::end(styles); ++it)
    check_single_property(*it);
  for(std::string const * it = styles; it != boost::end(styles); ++it)
    check_single_property_w(std::wstring(it->begin(), it->end()));
}

TEST(css_style_iterator, with_space)
{
  std::string style(" name with space : value with space  ;");
  svgpp::css_style_iterator<std::string::const_iterator> it(style.begin(), style.end());
  EXPECT_TRUE(!it.eof());
  EXPECT_EQ("name with space", std::string(it->first.begin(), it->first.end()));
  EXPECT_EQ("value with space", std::string(it->second.begin(), it->second.end()));
  ++it;
  EXPECT_TRUE(it.eof());
}

TEST(css_style_iterator, valid2)
{
  std::string style(";;; :  error; ; :;;; name1:value1;error:;;e;: ;name2 :value2;");
  svgpp::css_style_iterator<std::string::const_iterator> it(style.begin(), style.end());
  EXPECT_TRUE(!it.eof());
  EXPECT_EQ("name1", std::string(it->first.begin(), it->first.end()));
  EXPECT_EQ("value1", std::string(it->second.begin(), it->second.end()));
  ++it;
  EXPECT_TRUE(!it.eof());
  EXPECT_EQ("name2", std::string(it->first.begin(), it->first.end()));
  EXPECT_EQ("value2", std::string(it->second.begin(), it->second.end()));
  ++it;
  EXPECT_TRUE(it.eof());
}