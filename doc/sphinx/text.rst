Character Data
====================

Character data content of elements, that supports it according to SVG Specification 
(**text**, **desc**, **title**, **style** etc), is passed to the user code by the
*Text Events Policy*.

::

  struct text_events_policy_concept
  {
    template<class Range>
    static void set_text(Context & context, Range const & text);
  };