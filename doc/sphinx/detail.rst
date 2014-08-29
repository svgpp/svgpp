Traits
-------

Metafunction child_element_types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Synopsis
"""""""""""

::

  namespace traits
  {
    template<class ElementTag>
    struct child_element_types
    {
      typedef boost::mpl::set<...> type;
    };
  }

Parameter
"""""""""""

``ElementTag`` - element tag

Return type
"""""""""""

`Associative Sequence`_, containing tags of elements that can be childs of element corresponding to ``ElementTag``.


Metafunction element_required_attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Synopsis
"""""""""""

::

  namespace traits
  {
    template<class ElementTag>
    struct element_required_attributes
    {
      typedef unspecified type;
    };
  }

Parameter
"""""""""""

``ElementTag`` - element tag

Return type
"""""""""""

Forward Sequence (Boost.MPL), containing tags of attributes that are required for an element corresponding to ``ElementTag``.
