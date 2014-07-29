#include "../../rapidxml_ns.hpp"
#include "../../rapidxml_ns_print.hpp"
#include "../../rapidxml_ns_utils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>

using namespace rapidxml_ns;
using namespace std;

xml_document<char> doc;
xml_node<char> toc(node_element);
vector<int> numbering(3, 0);

void add_to_toc(int level, xml_node<char> *title, xml_attribute<char> *id, bool not_numbered)
{
    char *style_class;
    char *fulltitle = doc.allocate_string(0, title->value_size() + 100);
    if (level == 0)
    {
        style_class = "toc1";
        if (not_numbered)
        {
            sprintf(fulltitle, title->value());
        }
        else
        {
            sprintf(fulltitle, "%i. %s", ++numbering[0], title->value());
            numbering[1] = 0;
            numbering[2] = 0;
        }
    }
    else if (level == 1)
    {
        style_class = "toc2";
        if (not_numbered)
        {
            sprintf(fulltitle, title->value());
        }
        else
        {
            sprintf(fulltitle, "%i.%i %s", numbering[0], ++numbering[1], title->value());
            numbering[2] = 0;
        }
    }
    else if (level == 2)
    {
        style_class = "toc3";
        if (not_numbered)
        {
            sprintf(fulltitle, title->value());
        }
        else
        {
            sprintf(fulltitle, "%i.%i.%i %s", numbering[0], numbering[1], ++numbering[2], title->value());
        }
    }
    else
        throw std::exception("Invalid TOC level");
    xml_node<char> *tocentry = doc.allocate_node(node_element, "a", fulltitle);
    char *link = doc.allocate_string(0, id->value_size() + 1);
    memcpy(link + 1, id->value(), id->value_size());
    link[0] = '#';
    tocentry->append_attribute(doc.allocate_attribute("href", link, 0, id->value_size() + 1));
    tocentry->append_attribute(doc.allocate_attribute("class", style_class));
    toc.append_node(tocentry);
    toc.append_node(doc.allocate_node(node_element, "br"));
    xml_node<char> *title_data = title->first_node();
    if (title_data)
        title->remove_node(title_data);
    title->value(fulltitle);
}

void process_contextless(xml_node<char> *node)
{
    // Translate contextless doxygen tags
    if (string(node->name()) == "ulink")
    {
        node->name("a");
        xml_attribute<char> *attr = node->first_attribute("url");
        if (attr)
            attr->name("href");
    }
    else if (string(node->name()) == "ref")
    {
        node->name("a");
        xml_attribute<char> *attr = node->first_attribute("refid");
        if (attr)
        {
            attr->name("href");
            char *value = doc.allocate_string(0, attr->value_size() + 1);
            std::memcpy(value + 1, attr->value(), attr->value_size());
            value[0] = '#';
            attr->value(value, attr->value_size() + 1);
        }
    }
    else if (string(node->name()) == "computeroutput")
    {
        node->name("code");
    }
    else if (string(node->name()) == "verbatim")
    {
        node->name("pre");
    }
    else if (string(node->name()) == "linebreak")
    {
        node->name("br");
    }
    else if (string(node->name()) == "hruler")
    {
        node->name("hr");
    }
    else if (string(node->name()) == "heading")
    {
        xml_attribute<char> *attr = node->first_attribute("level");
        if (attr)
        {
            int level = atoi(attr->value());
            if (level == 0) node->name("h0");
            else if (level == 1) node->name("h1");
            else if (level == 2) node->name("h2");
            else if (level == 3) node->name("h3");
            else if (level == 4) node->name("h4");
            else if (level == 5) node->name("h5");
            else if (level == 6) node->name("h6");
            else if (level == 7) node->name("h7");
            else if (level == 8) node->name("h8");
            else node->name("h9");
        }
    }
    else if (string(node->name()) == "toc")
    {
        if (toc.parent() != 0)
            throw std::exception("More than one <toc> tag found.");
        toc.name("toc-contents");
        node->append_node(&toc);
    }
    else if (string(node->name()) == "sect1")
    {
        xml_node<char> *title = node->first_node("title");
        if (title)
        {
            title->name("h2");
            xml_attribute<char> *id = node->first_attribute("id");
            if (id)
            {
                xml_attribute<char> *not_numbered = node->first_attribute("not-numbered");
                add_to_toc(0, title, id, not_numbered != 0);
                id->parent()->remove_attribute(id);
                title->append_attribute(id);
            }
        }
    }
    else if (string(node->name()) == "sect2")
    {
        xml_node<char> *title = node->first_node("title");
        if (title)
        {
            title->name("h3");
            xml_attribute<char> *id = node->first_attribute("id");
            if (id)
            {
                xml_attribute<char> *not_numbered = node->first_attribute("not-numbered");
                add_to_toc(1, title, id, not_numbered != 0);
                id->parent()->remove_attribute(id);
                title->append_attribute(id);
            }
        }
    }
    else if (string(node->name()) == "sect3")
    {
        xml_node<char> *title = node->first_node("title");
        if (title)
        {
            title->name("h4");
            xml_attribute<char> *id = node->first_attribute("id");
            if (id)
            {
                xml_attribute<char> *not_numbered = node->first_attribute("not-numbered");
                add_to_toc(2, title, id, not_numbered != 0);
                id->parent()->remove_attribute(id);
                title->append_attribute(id);
            }
        }
    }
    else if (string(node->name()) == "itemizedlist")
    {
        node->name("ul");
    }
    else if (string(node->name()) == "listitem")
    {
        node->name("li");
    }
    else if (string(node->name()) == "table")
    {
        node->append_attribute(doc.allocate_attribute("border", "1"));
        node->append_attribute(doc.allocate_attribute("cellpadding", "3pt"));
    }
    else if (string(node->name()) == "row")
    {
        node->name("tr");
    }
    else if (string(node->name()) == "entry")
    {
        if (node->first_attribute("thead") && string(node->first_attribute("thead")->value()) == "yes")
            node->name("th");
        else
            node->name("td");
        xml_node<char> *rowspan = node->first_node("rowspan");
        if (rowspan)
        {
            xml_attribute<char> *attr = rowspan->first_attribute("count");
            if (attr)
            {
                xml_attribute<char> *a = doc.allocate_attribute();
                a->name("rowspan");
                a->value(attr->value());
                node->append_attribute(a);
            }
        }
    }
    else if (string(node->name()) == "emphasis")
    {
        node->name("i");
    }
    else if (string(node->name()) == "bold")
    {
        node->name("b");
    }
    else if (string(node->name()) == "space")
    {
        node->type(node_data);
        int n = 1;
        if (node->first_attribute("count"))
            n = atoi(node->first_attribute("count")->value());
        char *data = doc.allocate_string(0, n);
        memset(data, ' ', n);
        node->value(data, n);
    }

    // Process recursively
    xml_node<char> *child = node->first_node();
    while (child)
    {
        xml_node<char> *next = child->next_sibling();
        process_contextless(child);
        child = next;
    }
}

void process_para(xml_node<char> *node)
{
    if (string(node->name()) == "parameterlist")
    {
        node->parent()->remove_node(node);
        return;
    }
    else if (string(node->name()) == "simplesect")
    {
        node->parent()->remove_node(node);
        return;
    }

    // Process recursively
    xml_node<char> *child = node->first_node();
    while (child)
    {
        xml_node<char> *next = child->next_sibling();
        process_para(child);
        child = next;
    }
}

void process(xml_node<char> *node)
{
     if (string(node->name()) == "para")
     {
         process_para(node);       
     }
     else
     {
         xml_node<char> *child = node->first_node();
         while (child)
         {
             xml_node<char> *next = child->next_sibling();
             process(child);
             child = next;
         }
     }
}

int main(int argc, char *argv[])
{
    try
    {
        file<char> in(cin);
        doc.parse<0>(in.data());
        process(&doc);
        process_contextless(&doc);
        print(cout, doc, print_no_indenting);
    }
    catch (exception &e)
    {
        cerr << "Postprocessor error: " << e.what() << "\n";
    }
}
