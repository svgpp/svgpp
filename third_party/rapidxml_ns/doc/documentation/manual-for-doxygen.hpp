// This documentation is parsed by Doxygen to produce manual with working links to reference section

//! \namespace rapidxml_ns
//!	<i>Copyright (C) 2006, 2009 Marcin Kalicinski</i>
//!	<br><i>Copyright (C) 2014 Oleg Maximenko</i>
//! <br>
//! <i>See accompanying file <a href="license.txt">license.txt</a> for license information.</i>
//! <hr>
//! <h2>Table of Contents</h2>
//! \xmlonly <toc></toc> \endxmlonly
//! <br>
//!
//! \section what_is_rapidxml What is RapidXml NS?
//!
//! <a href="http://rapidxml.sourceforge.net">RapidXml</a> is an attempt to create the fastest XML DOM parser possible, while retaining useability, 
//! portability and reasonable W3C compatibility. 
//! It is an in-situ parser written in C++, with parsing speed approaching that of <c>strlen()</c> function executed on the same data.
//! <br><br>
//! RapidXML NS is a derivation of RapidXML with XML Namespaces 1.0 support added.
//! <br><br>
//! Entire parser is contained in a single header file, so no building or linking is neccesary. 
//! To use it you just need to copy <c>%rapidxml_ns.hpp</c> file to a convenient place (such as your project directory), and include it where needed.
//! You may also want to use printing functions contained in header <c>%rapidxml_ns_print.hpp</c>.
//!
//! \subsection dependencies_and_compatibility Dependencies And Compatibility
//!
//! RapidXml has <i>no dependencies</i> other than a very small subset of standard C++ library 
//! (<c>&lt;cassert&gt;</c>, <c>&lt;cstdlib&gt;</c>, <c>&lt;new&gt;</c>, <c>&lt;vector&gt;</c>
//! and <c>&lt;exception&gt;</c>, unless exceptions are disabled).
//! It should compile on any reasonably conformant compiler, and was tested on Visual C++ 2003, Visual C++ 2005, Visual C++ 2008, gcc 3, gcc 4, and Comeau 4.3.3.
//! Care was taken that no warnings are produced on these compilers, even with highest warning levels enabled.
//!
//! \subsection character_types_and_encodings Character Types And Encodings
//!
//! RapidXml is character type agnostic, and can work both with narrow and wide characters. 
//! Current version does not fully support UTF-16 or UTF-32, so use of wide characters is somewhat incapacitated.
//! However, it should succesfully parse <c>wchar_t</c> strings containing UTF-16 or UTF-32 if endianness of the data matches that of the machine.
//! UTF-8 is fully supported, including all numeric character references, which are expanded into appropriate UTF-8 byte sequences (unless you enable parse_no_utf8 flag).
//! <br><br>
//! Note that RapidXml performs no decoding - strings returned by name() and value() functions will contain text encoded using the same encoding as source file.
//! Rapidxml understands and expands the following character references:  <c>&amp;apos; &amp;amp; &amp;quot; &amp;lt; &amp;gt; &amp;#...;</c>
//! Other character references are not expanded.
//!
//! \subsection error_handling Error Handling
//!
//! By default, RapidXml uses C++ exceptions to report errors. 
//! If this behaviour is undesirable, RAPIDXML_NO_EXCEPTIONS can be defined to suppress exception code.
//! See parse_error class and parse_error_handler() function for more information. 
//!
//! \subsection memory_allocation Memory Allocation
//!
//! RapidXml uses a special memory pool object to allocate nodes and attributes, because direct allocation using <c>new</c> operator would be far too slow.
//! Underlying memory allocations performed by the pool can be customized by use of memory_pool::set_allocator() function. 
//! See class memory_pool for more information.
//!
//! \subsection w3c_compliance W3C Compliance
//!
//! RapidXml is not a W3C compliant parser, primarily because it ignores DOCTYPE declarations. 
//! There is a number of other, minor incompatibilities as well.
//! Still, it can successfully parse and produce complete trees of all valid XML files in W3C conformance suite 
//! (over 1000 files specially designed to find flaws in XML processors). 
//! In destructive mode it performs whitespace normalization and character entity substitution for a small set of built-in entities.
//!
//! \subsection api_design API Design
//!
//! RapidXml API is minimalistic, to reduce code size as much as possible, and facilitate use in embedded environments.
//! Additional convenience functions are provided in separate headers: <c>rapidxml_ns_utils.hpp</c> and <c>rapidxml_ns_print.hpp</c>.
//! Contents of these headers is not an essential part of the library, and is currently not documented (otherwise than with comments in code).
//!
//! \subsection reliability Reliability
//!
//! RapidXml is very robust and comes with a large harness of unit tests. 
//! Special care has been taken to ensure stability of the parser no matter what source text is thrown at it. 
//! One of the unit tests produces 100,000 randomly corrupted variants of XML document, 
//! which (when uncorrupted) contains all constructs recognized by RapidXml.
//! RapidXml passes this test when it correctly recognizes that errors have been introduced, and does not crash or loop indefinitely.
//! <br><br>
//! Another unit test puts RapidXml head-to-head with another, well estabilished XML parser, 
//! and verifies that their outputs match across a wide variety of small and large documents.
//! <br><br>
//! Yet another test feeds RapidXml with over 1000 test files from W3C compliance suite, and verifies that correct results are obtained.
//! There are also additional tests that verify each API function separately, and test that various parsing modes work as expected.
//!
//! \subsection acknowledgements Acknowledgements
//!
//! I would like to thank Arseny Kapoulkine for his work on <a href="http://code.google.com/p/pugixml">pugixml</a>, 
//! which was an inspiration for this project.
//! Additional thanks go to Kristen Wegner for creating <a href="http://www.codeproject.com/soap/pugxml.asp">pugxml</a>,
//! from which pugixml was derived.
//! Janusz Wohlfeil kindly ran RapidXml speed tests on hardware that I did not have access to, 
//! allowing me to expand performance comparison table.
//!
//! \section two_minute_tutorial Two Minute Tutorial
//!
//! \subsection parsing Parsing
//!
//! The following code causes RapidXml to parse a zero-terminated string named <c>text</c>:
//! \verbatim
using namespace rapidxml_ns;
xml_document<> doc;    // character type defaults to char
doc.parse<0>(text);    // 0 means default parse flags
\endverbatim
//! <c>doc</c> object is now a root of DOM tree containing representation of the parsed XML.
//! Because all RapidXml NS interface is contained inside namespace <c>rapidxml_ns</c>, 
//! users must either bring contents of this namespace into scope, or fully qualify all the names.
//! Class xml_document represents a root of the DOM hierarchy. 
//! By means of public inheritance, it is also an xml_node and a memory_pool.
//! Template parameter of xml_document::parse() function is used to specify parsing flags, 
//! with which you can fine-tune behaviour of the parser.
//! Note that flags must be a compile-time constant.
//!
//! \subsection accessing_dom_tree Accessing The DOM Tree
//!
//! To access the DOM tree, use methods of xml_node and xml_attribute classes:
//! \verbatim
cout << "Name of my first node is: " << doc.first_node()->name() << "\n";
xml_node<> *node = doc.first_node("foobar");
cout << "Node foobar has value " << node->value() << "\n";
for (xml_attribute<> *attr = node->first_attribute();
     attr; attr = attr->next_attribute())
{
    cout << "Node foobar has attribute " << attr->name() << " ";
    cout << "with value " << attr->value() << "\n";
}
\endverbatim
//!
//! \subsection modifying_dom_tree Modifying The DOM Tree
//!
//! DOM tree produced by the parser is fully modifiable. Nodes and attributes can be added/removed,
//! and their contents changed. The below example creates a HTML document, whose sole contents is a link to google.com website:
//! \verbatim
xml_document<> doc;
xml_node<> *node = doc.allocate_node(node_element, "a", "Google");
doc.append_node(node);
xml_attribute<> *attr = doc.allocate_attribute("href", "google.com");
node->append_attribute(attr);
\endverbatim
//! One quirk is that nodes and attributes <i>do not own</i> the text of their names and values.
//! This is because normally they only store pointers to the source text. 
//! So, when assigning a new name or value to the node, care must be taken to ensure proper lifetime of the string.
//! The easiest way to achieve it is to allocate the string from the xml_document memory pool.
//! In the above example this is not necessary, because we are only assigning character constants.
//! But the code below uses memory_pool::allocate_string() function to allocate node name
//! (which will have the same lifetime as the document), and assigns it to a new node:
//! \verbatim
xml_document<> doc;
char *node_name = doc.allocate_string(name);        // Allocate string and copy name into it
xml_node<> *node = doc.allocate_node(node_element, node_name);  // Set node name to node_name
\endverbatim
//! Check \ref reference section for description of the entire interface.
//!
//! \subsection printing Printing XML
//!
//! You can print <c>xml_document</c> and <c>xml_node</c> objects into an XML string. Use print() function or operator <<, which are defined in <c>rapidxml_print.hpp</c> header.
//! \verbatim
using namespace rapidxml_ns;
xml_document<> doc;    // character type defaults to char
// ... some code to fill the document

// Print to stream using operator <<
std::cout << doc;   

// Print to stream using print function, specifying printing flags
print(std::cout, doc, 0);   // 0 means default printing flags

// Print to string using output iterator
std::string s;
print(std::back_inserter(s), doc, 0);

// Print to memory buffer using output iterator
char buffer[4096];                      // You are responsible for making the buffer large enough!
char *end = print(buffer, doc, 0);      // end contains pointer to character after last printed character
*end = 0;                               // Add string terminator after XML
\endverbatim
//! \section differences Differences From Regular XML Parsers
//!
//! RapidXml is an <i>in-situ parser</i>, which allows it to achieve very high parsing speed.
//! In-situ means that parser does not make copies of strings. 
//! Instead, it places pointers to the <i>source text</i> in the DOM hierarchy.
//!
//! \subsection lifetime_of_source_text Lifetime Of Source Text
//!
//! In-situ parsing requires that source text lives at least as long as the document object.
//! If source text is destroyed, names and values of nodes in DOM tree will become destroyed as well.
//! Additionally, whitespace processing, character entity translation, 
//! and zero-termination of strings require that source text be modified during parsing
//! (but see non-destructive mode). 
//! This makes the text useless for further processing once it was parsed by RapidXml. 
//! <br><br>
//! In many cases however, these are not serious issues.
//!
//! \subsection ownership_of_strings Ownership Of Strings
//! 
//! Nodes and attributes produced by RapidXml do not own their name and value strings.
//! They merely hold the pointers to them.
//! This means you have to be careful when setting these values manually, 
//! by using xml_base::name(const Ch *) or xml_base::value(const Ch *) functions.
//! Care must be taken to ensure that lifetime of the string passed is at least as long as lifetime of the node/attribute.
//! The easiest way to achieve it is to allocate the string from memory_pool owned by the document.
//! Use memory_pool::allocate_string() function for this purpose.
//! 
//! \subsection destructive_non_destructive Destructive Vs Non-Destructive Mode
//! 
//! By default, the parser modifies source text during the parsing process.
//! This is required to achieve character entity translation, 
//! whitespace normalization, and zero-termination of strings.
//! <br><br>
//! In some cases this behaviour may be undesirable, for example if source text resides in read only memory, 
//! or is mapped to memory directly from file.
//! By using appropriate parser flags (parse_non_destructive), source text modifications can be disabled.
//! However, because RapidXml does in-situ parsing, it obviously has the following side-effects:
//! - no whitespace normalization is done
//! - no entity reference translation is done
//! - names and values are not zero-terminated, you must use xml_base::name_size() and xml_base::value_size() functions to tell where they end
//!
//! \section performance Performance
//!
//! RapidXml achieves its speed through use of several techniques:
//! - In-situ parsing. 
//! When building DOM tree, RapidXml does not make copies of string data, such as node names and values.
//! Instead, it stores pointers to interior of the source text.
//! - Use of template metaprogramming techniques.
//! This allows it to move much of the work to compile time. 
//! Through magic of the templates, C++ compiler generates a separate copy of parsing code for any combination of parser flags you use.
//! In each copy, all possible decisions are made at compile time and all unused code is omitted.
//! - Extensive use of lookup tables for parsing.
//! - Hand-tuned C++ with profiling done on several most popular CPUs.
//! 
//! This results in a very small and fast code: a parser which is custom tailored to exact needs with each invocation.
//!
//! \subsection performance_charts Comparison With Other Parsers
//!
//! The table below compares speed of RapidXml to some other parsers, and to <c>strlen()</c> function executed on the same data.
//! On a modern CPU (as of 2007), you can expect parsing throughput to be close to 1 GB/s.
//! As a rule of thumb, parsing speed is about 50-100x faster than Xerces DOM, 
//! 30-60x faster than TinyXml, 3-12x faster than pugxml, and about 5% - 30% faster than pugixml,
//! the fastest XML parser I know of.
//!
//! <ul>
//! <li>The test file is a real-world, 50kB large, moderately dense XML file.</li>
//! <li>All timing is done by using RDTSC instruction present in Pentium-compatible CPUs.</li>
//! <li>No profile-guided optimizations are used.</li>
//! <li>All parsers are running in their fastest modes.</li>
//! <li>The results are given in CPU cycles per character, so frequency of CPUs is irrelevant.</li>
//! <li>The results are minimum values from a large number of runs, to minimize effects of operating system activity, task switching, interrupt handling etc.</li>
//! <li>A single parse of the test file takes about 1/10th of a millisecond, so with large number of runs there is a good chance of hitting at least one no-interrupt streak, and obtaining undisturbed results.</li>
//! </ul>
//! <table>
//!     <tr>
//!         <th><center>Platform</center></th>
//!         <th><center>Compiler</center></th>
//!         <th>strlen()</th>
//!         <th>RapidXml</th>
//!         <th>pugixml 0.3</th>
//!         <th>pugxml</th>
//!         <th>TinyXml</th>
//!     </tr>
//!     <tr>
//!         <td><center>Pentium 4</center></td>
//!         <td><center>MSVC 8.0</center></td>
//!         <td><center>2.5</center></td>
//!         <td><center>5.4</center></td>
//!         <td><center>7.0</center></td>
//!         <td><center>61.7</center></td>
//!         <td><center>298.8</center></td>
//!     </tr>
//!     <tr>
//!         <td><center>Pentium 4</center></td>
//!         <td><center>gcc 4.1.1</center></td>
//!         <td><center>0.8</center></td>
//!         <td><center>6.1</center></td>
//!         <td><center>9.5</center></td>
//!         <td><center>67.0</center></td>
//!         <td><center>413.2</center></td>
//!     </tr>
//!     <tr>
//!         <td><center>Core 2</center></td>
//!         <td><center>MSVC 8.0</center></td>
//!         <td><center>1.0</center></td>
//!         <td><center>4.5</center></td>
//!         <td><center>5.0</center></td>
//!         <td><center>24.6</center></td>
//!         <td><center>154.8</center></td>
//!     </tr>
//!     <tr>
//!         <td><center>Core 2</center></td>
//!         <td><center>gcc 4.1.1</center></td>
//!         <td><center>0.6</center></td>
//!         <td><center>4.6</center></td>
//!         <td><center>5.4</center></td>
//!         <td><center>28.3</center></td>
//!         <td><center>229.3</center></td>
//!     </tr>
//!     <tr>
//!         <td><center>Athlon XP</center></td>
//!         <td><center>MSVC 8.0</center></td>
//!         <td><center>3.1</center></td>
//!         <td><center>7.7</center></td>
//!         <td><center>8.0</center></td>
//!         <td><center>25.5</center></td>
//!         <td><center>182.6</center></td>
//!     </tr>
//!     <tr>
//!         <td><center>Athlon XP</center></td>
//!         <td><center>gcc 4.1.1</center></td>
//!         <td><center>0.9</center></td>
//!         <td><center>8.2</center></td>
//!         <td><center>9.2</center></td>
//!         <td><center>33.7</center></td>
//!         <td><center>265.2</center></td>
//!     </tr>
//!     <tr>
//!         <td><center>Pentium 3</center></td>
//!         <td><center>MSVC 8.0</center></td>
//!         <td><center>2.0</center></td>
//!         <td><center>6.3</center></td>
//!         <td><center>7.0</center></td>
//!         <td><center>30.9</center></td>
//!         <td><center>211.9</center></td>
//!     </tr>
//!     <tr>
//!         <td><center>Pentium 3</center></td>
//!         <td><center>gcc 4.1.1</center></td>
//!         <td><center>1.0</center></td>
//!         <td><center>6.7</center></td>
//!         <td><center>8.9</center></td>
//!         <td><center>35.3</center></td>
//!         <td><center>316.0</center></td>
//!     </tr>
//! </table>
//! <i>(*) All results are in CPU cycles per character of source text</i>
//!
//! \section reference Reference
//! 
//! This section lists all classes, functions, constants etc. and describes them in detail.
