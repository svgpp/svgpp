.. _transform-section:

Transform 
=================

Transformation parsing is controlled by *Transform Policy* and *Transform Events Policy*. 
*Transform Policy* configures the adapter 
that simplifies application handling of coordinate system transformations (e.g. adapter
may substitute simple transformation steps translate/scale/rotate/skew with corresponding
transformation matrices, more convenient in some cases). 
*Transform Events Policy* defines how parsed data is passed to the user code.


Transform Events Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Transform Events Policy* depends on compile-time settings in *Transform Policy*. 
If ``policy::transform::raw`` *Transform Policy* is used that preserves input data at much as possible
then *Transform Events Policy* becomes::

  struct transform_events_policy
  {
    typedef /*...*/ context_type; 

    static void transform_matrix(context_type & context, const boost::array<number_type, 6> & matrix);
    static void transform_translate(context_type & context, number_type tx, number_type ty);
    static void transform_translate(context_type & context, number_type tx);
    static void transform_scale(context_type & context, number_type sx, number_type sy);
    static void transform_scale(context_type & context, number_type scale);
    static void transform_rotate(context_type & context, number_type angle);
    static void transform_rotate(context_type & context, number_type angle, number_type cx, number_type cy);
    static void transform_skew_x(context_type & context, number_type angle);
    static void transform_skew_y(context_type & context, number_type angle);
  };

Depending on *Transform Policy*, some of the methods aren't called by SVG++ and therefore shouldn't be implemented.

.. note::
  Transformation matrix is passed as array of size 6 ``[a b c d e f]``, corresponding to this matrix:

  .. image:: http://www.w3.org/TR/SVG11/images/coords/Matrix.png

:ref:`Named class template parameter <named-params>` for *Transform Events Policy* is ``transform_events_policy``.

Default *Transform Events Policy* (``policy::transform_events::forward_to_method``) forwards calls to its
static methods to methods of ``context`` object::

  struct forward_to_method
  {
    typedef Context context_type; 

    template<class Number>
    static void transform_matrix(context_type & context, const boost::array<Number, 6> & matrix)
    {
      context.transform_matrix(matrix);
    }

    /*...*/
  };

Example of handling transforms with default settings (``src/samples/sample_transform01.cpp``):
  
.. literalinclude:: ../../src/samples/sample_transform01.cpp 

In this example sequential transforms are joined in user code (``src/samples/sample_transform02.cpp``):

.. literalinclude:: ../../src/samples/sample_transform02.cpp 


Transform Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

  struct transform_policy_concept
  {
    static const bool join_transforms = /* true or false */;
    static const bool no_rotate_about_point = /* true or false */;
    static const bool no_shorthands = /* true or false */; 
    static const bool only_matrix_transform = /* true or false */;
  };

*Transform Policy* is a class with ``bool`` static member constants. If they all are ``false``
(as in ``policy::transform::raw``), then adapter isn't used and the parser passes parsed values as is. 
Setting some members to ``true`` programmer may simplify the application:

  ``join_transforms = true``
    All transformations in SVG attribute are joined in single transformation matrix. 
    Values of other *Transform Policy* members are ignored. 
    *Transform Events Policy* in this case contains only one method::

      struct transform_events_policy
      {
        typedef /*...*/ context_type; 

        static void transform_matrix(context_type & context, const boost::array<number_type, 6> & matrix);
      }

  ``no_rotate_about_point = true``
    **rotate(<rotate-angle> <cx> <cy>)** substituted with **translate(<cx>, <cy>) rotate(<rotate-angle>) translate(-<cx>, -<cy>)**.
    ``transform_rotate`` method of *Transform Events Policy* with parameters ``cx`` and ``cy`` is not used.

  ``no_shorthands = true``
    Calls to ``transform_translate`` and ``transform_scale`` with one number substituted with 
    corresponding calls with two numbers.

  ``only_matrix_transform = true``
    Each transformation step is substituted with corresponding call to ``transform_matrix``. 
    Therefore only this method is used in *Transform Events Policy*.

File ``svgpp/policy/transform.hpp`` contains some predefined *Transform Policies*. 
``policy::transform::matrix``, used by default, sets ``join_transforms = true``.

:ref:`Named class template parameter <named-params>` for *Transform Policy* is ``transform_policy``.