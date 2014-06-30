// This test performs comparative parsing between TinyXML and rapidxml
#include "../test_utils.hpp"
#include "../../rapidxml.hpp"
#include "../../rapidxml_print.hpp"
#include "../other_parsers/tinyxml/tinyxml.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstring>
#include <stdexcept>
#include <memory>

using namespace rapidxml;
using namespace std;

template<int Flags> 
void convert_tinyxml_to_rapidxml_declaration(TiXmlDeclaration *src, xml_node<char> *dest, xml_document<char> *doc)
{
    if (*src->Version())
    {
        xml_attribute<char> *attr = doc->allocate_attribute();
        attr->name("version", strlen("version"));
        attr->value(src->Version(), strlen(src->Version()));
        dest->append_attribute(attr);
    }
    if (*src->Encoding())
    {
        xml_attribute<char> *attr = doc->allocate_attribute();
        attr->name("encoding", strlen("encoding"));
        attr->value(src->Encoding(), strlen(src->Encoding()));
        dest->append_attribute(attr);
    }
    if (*src->Standalone())
    {
        xml_attribute<char> *attr = doc->allocate_attribute();
        attr->name("standalone", strlen("standalone"));
        attr->value(src->Standalone(), strlen(src->Standalone()));
        dest->append_attribute(attr);
    }
}

template<int Flags> 
void convert_tinyxml_to_rapidxml_attributes(TiXmlElement *src, xml_node<char> *dest, xml_document<char> *doc)
{
    for (TiXmlAttribute *src_attr = src->FirstAttribute(); src_attr; src_attr = src_attr->Next())
    {
        xml_attribute<char> *dest_attr = doc->allocate_attribute();
        dest_attr->name(src_attr->NameTStr().c_str(), src_attr->NameTStr().size());
        dest_attr->value(src_attr->ValueStr().c_str(), src_attr->ValueStr().size());
        dest->append_attribute(dest_attr);
    }
}

template<int Flags> 
void convert_tinyxml_to_rapidxml(TiXmlNode *src, xml_node<char> *dest, xml_document<char> *doc)
{
    // For all nodes
    for (TiXmlNode *src_child = src->FirstChild(); src_child; src_child = src_child->NextSibling())
    {
        
        // Create proper node type
        xml_node<char> *dest_child = 0;
        switch (src_child->Type())
        {

        // Declaration
        case TiXmlNode::DECLARATION:
            if (Flags & parse_declaration_node)
            {
                dest_child = doc->allocate_node(node_declaration);
                dest_child->name(src_child->Value(), src_child->ValueStr().size());
                convert_tinyxml_to_rapidxml_declaration<Flags>(src_child->ToDeclaration(), dest_child, doc);
                convert_tinyxml_to_rapidxml<Flags>(src_child, dest_child, doc);
            }
            break;

        // Element
        case TiXmlNode::ELEMENT:
            dest_child = doc->allocate_node(node_element);
            dest_child->name(src_child->Value(), src_child->ValueStr().size());
            convert_tinyxml_to_rapidxml_attributes<Flags>(src_child->ToElement(), dest_child, doc);
            convert_tinyxml_to_rapidxml<Flags>(src_child, dest_child, doc);
            break;

        // Text
        case TiXmlNode::TEXT:
            if (!(Flags & parse_no_data_nodes))
            {
                dest_child = doc->allocate_node(node_data);
                dest_child->value(src_child->Value(), src_child->ValueStr().size());
            }
            if (!(Flags & parse_no_element_values))
                if (!dest->value())
                    dest->value(src_child->Value(), src_child->ValueStr().size());
            break;

        // Comment
        case TiXmlNode::COMMENT:
            if (Flags & parse_comment_nodes)
            {
                dest_child = doc->allocate_node(node_comment);
                dest_child->value(src_child->Value(), src_child->ValueStr().size());
            }
            break;

        // Unknown
        case TiXmlNode::UNKNOWN:
            break;

        default:
            throw runtime_error("unknown node produced by tinyxml");
        }
        
        // If child was created, append it
        if (dest_child)
            dest->append_node(dest_child);
    }
}

struct two_xmls
{
    TiXmlDocument ti_doc;
    bool tinyxml_fails;
    vector<char> data;
    xml_document<char> doc1;
    xml_document<char> doc2;
};

template<int Flags>
two_xmls *create_two_xmls(const string &filename, bool tinyxml_fails)
{

    two_xmls *result = new two_xmls;
    result->tinyxml_fails = tinyxml_fails;
    
    // Create TinyXML version
    if (!tinyxml_fails)
    {
        ifstream stream(filename.c_str(), ios::binary);
        if (!stream)
            throw runtime_error(string("cannot open file ") + filename);
        stream.unsetf(ios::skipws);
        stream.seekg(0, ios::end);
        size_t size = stream.tellg();
        stream.seekg(0);   
        vector<char> data(size + 1);
        stream.read(&data.front(), static_cast<streamsize>(size));
        result->ti_doc.Parse(&data.front());
        convert_tinyxml_to_rapidxml<Flags>(&result->ti_doc, &result->doc1, &result->doc1);
    }

    // Create rapidxml version
    {
        ifstream stream(filename.c_str(), ios::binary);
        if (!stream)
            throw runtime_error(string("cannot open file ") + filename);
        stream.unsetf(ios::skipws);
        stream.seekg(0, ios::end);
        size_t size = stream.tellg();
        stream.seekg(0);   
        result->data.resize(size + 1);
        char *data = &result->data.front();
        stream.read(data, static_cast<streamsize>(size));
        result->doc2.parse<Flags>(data);
    }

    return result;

}

bool compare_attributes(xml_attribute<char> *a1, xml_attribute<char> *a2, string &desc)
{
    
    // Compare names
    const char *name1 = a1->name();
    const char *name2 = a2->name();
    if (strcmp(name1, name2) != 0)
    {
        desc = desc + "." + name1;
        return false;
    }
    
    // Compare values
    const char *value1 = a1->value();
    const char *value2 = a2->value();
    if (strcmp(value1, value2) != 0)
    {
        desc = desc + "." + name1;
        return false;
    }

    return true;
}

bool compare_nodes(xml_node<char> *n1, xml_node<char> *n2, string &desc)
{
    
    // Compare names
    const char *name1 = n1->name();
    const char *name2 = n2->name();
    if (strcmp(name1, name2) != 0)
    {
        desc = desc + "." + name1;
        return false;
    }
    
    // Compare values
    const char *value1 = n1->value();
    const char *value2 = n2->value();
    if (strcmp(value1, value2) != 0)
    {
        desc = desc + "." + name1;
        return false;
    }

    // Compare children
    {
        xml_node<char> *ch1 = n1->first_node(), 
                       *ch2 = n2->first_node();
        for (; ch1 && ch2; ch1 = ch1->next_sibling(), ch2 = ch2->next_sibling())
        {
            if (!compare_nodes(ch1, ch2, desc))
            {
                desc = desc + "." + name1;
                return false;
            }
        }
        if (ch1 || ch2)
        {
            desc = desc + "." + name1;
            return false;
        }
    }

    // Compare attributes
    {
        xml_attribute<char> *a1 = n1->first_attribute(), 
                            *a2 = n2->first_attribute();
        for (; a1 && a2; a1 = a1->next_attribute(), a2 = a2->next_attribute())
        {
            if (!compare_attributes(a1, a2, desc))
            {
                desc = desc + "." + name1;
                return false;
            }
        }
        if (a1 || a2)
        {
            desc = desc + "." + name1;
            return false;
        }
    }

    return true;
}

template<int Flags>
void test_xml_file(const string &filename, bool tinyxml_fails)
{
    auto_ptr<two_xmls> tx(create_two_xmls<Flags>(filename, tinyxml_fails));
    string desc;
    if (!tx->tinyxml_fails && !compare_nodes(&tx->doc1, &tx->doc2, desc))
    {
        std::cout << "Test " << filename << " failed at " << desc << "\n";
        std::cout << "Results saved to result_tinyxml.xml and result_rapidxml.xml local directory.\n";
        ofstream stream1("result_tinyxml.xml");
        print(ostream_iterator<char>(stream1), tx->doc1);
        ofstream stream2("result_rapidxml.xml");
        print(ostream_iterator<char>(stream2), tx->doc2);
        throw runtime_error("comparison failed");
    }
    else
        std::cout << "Test " << filename << " succeeded.\n";
}

int main()
{
   
    try
    {
        // Load file list
        ifstream stream("filelist.txt", ios::binary);
        if (!stream)
            throw runtime_error("filelist.txt not found");
        vector<string> files;
        string name;
        while (getline(stream, name))
            if (!name.empty() && name[0] != ';')
                files.push_back(name);

        // Test all files in the list
        for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
        {
            const int Flags = parse_normalize_whitespace | parse_trim_whitespace | parse_no_element_values;
            if ((*it)[0] == '*')    // Skip tinyxml comparison if * found
                test_xml_file<Flags>(string("../") + (it->c_str() + 1), true);
            else
                test_xml_file<Flags>(string("../") + it->c_str(), false);
        }

        cout << "*** Success\n";

        // Success
        return 0;

    }
    catch (rapidxml::parse_error &e)
    {
        cout << "rapidxml::parse_error: " << e.what() << "\n";
        return 1;
    }
    catch (exception &e)
    {
        cout << "Error: " << e.what() << "\n";
        return 1;
    }

}
