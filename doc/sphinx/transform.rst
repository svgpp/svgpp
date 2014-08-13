Transform 
=================

load_transform Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

load_transform policy зависит от compile-time настроек, задаваемых посредством transform policy. Если используются настройки ``policy::transform::raw``, максимально сохраняющие структуру входных данных, то load_transform policy имеет вид::

  struct load_transform_policy
  {
    typedef /*...*/ context_type; 

    static void append_transform_matrix(context_type & context, const boost::array<number_type, 6> & matrix);
    static void append_transform_translate(context_type & context, number_type tx, number_type ty);
    static void append_transform_translate(context_type & context, number_type tx);
    static void append_transform_scale(context_type & context, number_type sx, number_type sy);
    static void append_transform_scale(context_type & context, number_type scale);
    static void append_transform_rotate(context_type & context, number_type angle);
    static void append_transform_rotate(context_type & context, number_type angle, number_type cx, number_type cy);
    static void append_transform_skew_x(context_type & context, number_type angle);
    static void append_transform_skew_y(context_type & context, number_type angle);
  };

Другие конфигурации transform policy могут уменьшать количество методов. 

Настройки ``policy::transform::matrix``, используемые по умолчанию, объединяют all transforms in list in single matrix transform и load_transform policy имеет вид::

  struct load_transform_policy
  {
    typedef /*...*/ context_type; 

    static void set_transform_matrix(context_type & context, const boost::array<number_type, 6> & matrix);
  };

load_transform policy по умолчанию (policy::load_transform::forward_to_method) переадресует вызовы статических методов на вызовы методов объекта context с остальными параметрами::

  struct forward_to_method
  {
    typedef Context context_type; 

    template<class Number>
    static void append_transform_matrix(context_type & context, const boost::array<Number, 6> & matrix)
    {
      context.append_transform_matrix(matrix);
    }

    /*...*/
  };

Пример с использованием настроек по умолчанию::
  
  #include <svgpp/svgpp.hpp>

  struct Context
  {
    void set_transform_matrix(const boost::array<double, 6> & matrix)
    {
      for(auto n: matrix)
        std::cout << n << " ";
      std::cout << "\n";
    }
  };

  void func()
  {
    Context context;
    value_parser<tag::type::transform_list>::parse(tag::attribute::transform(), context, 
      std::string("translate(-10,-20) scale(2) rotate(45) translate(5,10)"), tag::source::attribute());
  }

Пример, в котором умножение последовательных transforms происходит в user code::

  #include <svgpp/svgpp.hpp>
  #include <boost/numeric/ublas/matrix.hpp>
  #include <boost/numeric/ublas/io.hpp>

  namespace ublas = boost::numeric::ublas;

  typedef ublas::scalar_matrix<double> matrix_t;

  struct load_transform_policy
  {
    typedef matrix_t context_type;

    static void append_transform_matrix(matrix_t & transform, const boost::array<number_type, 6> & matrix)
    {
      matrix_t m(3, 3);
      m(0, 0) = matrix[0]; m(1, 0) = matrix[1]; m(0, 1) = matrix[2]; 
      m(1, 1) = matrix[3]; m(0, 2) = matrix[4]; m(1, 2) = matrix[5]; m(2, 2) = 1.0;
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_translate(matrix_t & transform, number_type tx, number_type ty)
    {
      matrix_t m = ublas::identity_matrix(3, 3);
      m(0, 2) = tx; m(1, 2) = ty; 
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_scale(matrix_t & transform, number_type sx, number_type sy)
    {
      matrix_t m(3, 3);
      m(0, 0) = sx; m(1, 1) = sy; m(2, 2) = 1; 
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_rotate(matrix_t & transform, number_type angle)
    {
      matrix_t m(3, 3);
      m(0, 0) =  std::cos(angle); m(1, 0) = std::sin(angle); 
      m(0, 1) = -std::sin(angle); m(1, 1) = std::cos(angle); m(2, 2) = 1; 
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_skew_x(matrix_t & transform, number_type angle)
    {
      matrix_t m = ublas::identity_matrix(3, 3);
      m(0, 1) = std::tan(angle);
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_skew_y(matrix_t & transform, number_type angle)
    {
      matrix_t m = ublas::identity_matrix(3, 3);
      m(1, 0) = std::tan(angle);
      transform = ublas::prod(transform, matrix);
    }
  };

  void func()
  {
    matrix_t transform(ublas::identity_matrix(3, 3));
    value_parser<
      tag::type::transform_list,
      transform_policy<policy::transform::minimal<double> >,
      load_transform_policy<load_transform_policy>
    >::parse(tag::attribute::transform(), transform, 
      std::string("translate(-10,-20) scale(2) rotate(45) translate(5,10)"), tag::source::attribute());
    std::cout << context.transform << "\n";
  }
