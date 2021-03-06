/*
 * Defining a typemap for std::auto_ptr
 */

%include std_auto_ptr.i

/*
    The typemaps here allow to handle functions returning std::auto_ptr<>,
    which is the most common use of this type. If you have functions taking it
    as parameter, these typemaps can't be used for them and you need to do
    something else (e.g. use shared_ptr<> which SWIG supports fully).
 */

%define %unique_ptr(TYPE)
%typemap (out) std::unique_ptr<TYPE > %{
   %set_output(SWIG_NewPointerObj($1.release(), $descriptor(TYPE *), SWIG_POINTER_OWN | %newpointer_flags));
%}
%template() std::unique_ptr<TYPE >;
%enddef

namespace std {
   template <class T> class unique_ptr {};
} 

namespace std
{
  %template (StrVector) vector <string>;
  %template (StrStrMap) map <string, string>;
}
