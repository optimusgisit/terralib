/*
 * Defining a typemap for std::auto_ptr
 */

%define AUTO_PTR_TYPEMAPS(TYPE...)

%typemap(out) (std::auto_ptr< TYPE >) %{
  SWIG_NewPointerObj(L, (void *) $result.release(), $descriptor(TYPE *), 1); 
  SWIG_arg++;
%}

%template () std::auto_ptr< TYPE >;

%enddef

%define UNIQUE_PTR_TYPEMAPS(TYPE...)

%typemap(out) (std::unique_ptr< TYPE >) %{
  SWIG_NewPointerObj(L, (void *) $result.release(), $descriptor(TYPE *), 1); 
  SWIG_arg++;
%}

%template () std::unique_ptr< TYPE >;

%enddef

namespace std
{
  %template (StrVector) vector <string>;
  %template (StrStrMap) map <string, string>;
}


/*
 * Defining a typemap for std::map< std::string, std::string >
 */

%typemap(in) std::map< std::string, std::string >, const std::map< std::string, std::string > & (std::map< std::string, std::string > aux)
{
  lua_pushnil(L);
  
  while(lua_next(L, -2) != 0)
  {
    if(lua_isstring(L, -1) && lua_isstring(L, -2))
    {
      std::string key(lua_tostring(L, -2)), 
                  value(lua_tostring(L, -1)); 
      
      aux[key] = value;
    }
    
    lua_pop(L, 1);
  }
  
  $1 = &aux;
}

%typemap(out) const std::map< std::string, std::string >&
{
  lua_newtable(L);
  
  std::map<std::string, std::string>::const_iterator it;
    
  for(it = $result->begin(); it != $result->end(); ++it)
  {
    const char* key = it->first.c_str();
    const char* value = it->second.c_str();
    
    lua_pushstring(L, key);
    lua_pushstring(L, value);
    lua_settable(L, -3);
  }
  
  return 1;
}

%typemap(out) std::map< std::string, std::string >
{
  lua_newtable(L);
  
  std::map<std::string, std::string>::const_iterator it;
    
  for(it = $result.begin(); it != $result.end(); ++it)
  {
    const char* key = it->first.c_str();
    const char* value = it->second.c_str();
    
    lua_pushstring(L, key);
    lua_pushstring(L, value);
    lua_settable(L, -3);
  }
  
  return 1;
}

%typemap(out) std::map< std::string, std::string >&
{
  lua_newtable(L);
  
  std::map<std::string, std::string>::const_iterator it;
    
  for(it = $result->begin(); it != $result->end(); ++it)
  {
    const char* key = it->first.c_str();
    const char* value = it->second.c_str();
    
    lua_pushstring(L, key);
    lua_pushstring(L, value);
    lua_settable(L, -3);
  }
  
  return 1;
}

/*
 * Defining a typemap for std::vector< std::string >
 */

%typemap(in) std::vector< std::string >, const std::vector< std::string > & (std::vector< std::string > aux)
{
  lua_pushnil(L);
  
  while(lua_next(L, -2) != 0)
  {
    if(lua_isstring(L, -1))
    {
      std::string value(lua_tostring(L, -1)); 
      aux.push_back(value);
    }
    
    lua_pop(L, 1);
  }
  
  $1 = &aux;
}

%typemap(out) std::vector<std::string>
{
  size_t s = $result.size();
  
  lua_createtable(L, 0, 1);
  
  for(size_t i = 0; i < s; i++)
    lua_pushstring(L, $result[i].c_str());
  
  return (int) s;
}

namespace std 
{
  template<class T> class auto_ptr { };
  template<class T> class unique_ptr { };
}