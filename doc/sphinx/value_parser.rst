Value Parsers
--------------------------------

To choose proper ``value_parser`` for the attribute, ``traits::attribute_type`` metafunction should be used::

  template<class Element, class Attribute> 
  struct attribute_type
  {
    typedef /*unspecified*/ type;
  };

The returned type can be:

* One of type tags. E.g. **width**, as many others attributes has *<length>* type (corresponds to ``tag::type::length``)::

    BOOST_MPL_ASSERT(( boost::is_same< 
      traits::attribute_type<tag::element::rect, tag::attribute::width>::type, 
      tag::type::length> ));

* Attribute tag. E.g. **viewBox** has its own syntax::

    BOOST_MPL_ASSERT(( boost::is_same< 
      traits::attribute_type<tag::element::svg, tag::attribute::viewBox>::type, 
      tag::attribute::viewBox> ));
  
* Pair *<element tag, attribute tag>*. E.g. **type** attribute may get different values in 
  elements **animateTransform**, **feColorMatrix**, **feTurbulence** etc.::

    BOOST_MPL_ASSERT(( boost::is_same< 
      traits::attribute_type<tag::element::animateTransform, tag::attribute::type>::type, 
      boost::mpl::pair<tag::element::animateTransform, tag::attribute::type> > ));


Attribute value parsers interface::

  template<class ValueType, class Args..>
  struct value_parser
  {
    template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
    static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                      PropertySource source);
  };

``AttributeTag tag``
  Is passed to context, isn't used by ``value_parser`` itself

``Context & context``
  Reference to the context that will :ref:`receive <passing-values>` parsed value.

``AttributeValue const & attribute_value``
  Attribute :ref:`string <passing-string>` value

``PropertySource source``
  One of two possible types: ``tag::source::attribute`` or ``tag::source::css``, depending on
  whether value if from SVG attribute or CSS property.

