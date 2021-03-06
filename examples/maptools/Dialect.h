// TerraLib
#include <terralib/dataaccess.h>

te::da::SQLDialect* createDialect()
{
  te::da::SQLDialect* mydialect = new te::da::SQLDialect;

  mydialect->insert("+", new te::da::BinaryOpEncoder("+"));
  mydialect->insert("-", new te::da::BinaryOpEncoder("-"));
  mydialect->insert("*", new te::da::BinaryOpEncoder("*"));
  mydialect->insert("/", new te::da::BinaryOpEncoder("/"));
  mydialect->insert("=", new te::da::BinaryOpEncoder("="));
  mydialect->insert("<>", new te::da::BinaryOpEncoder("<>"));
  mydialect->insert(">", new te::da::BinaryOpEncoder(">"));  
  mydialect->insert("<", new te::da::BinaryOpEncoder("<"));
  mydialect->insert(">=", new te::da::BinaryOpEncoder(">="));
  mydialect->insert("<=", new te::da::BinaryOpEncoder("<="));
  mydialect->insert("and", new te::da::BinaryOpEncoder("and"));
  mydialect->insert("or", new te::da::BinaryOpEncoder("or"));
  mydialect->insert("not", new te::da::UnaryOpEncoder("not"));  
  mydialect->insert("upper", new te::da::FunctionEncoder("upper"));
  mydialect->insert("lower", new te::da::FunctionEncoder("lower"));
  mydialect->insert("substring", new te::da::TemplateEncoder("substring", "($1 from $2 for $3)"));
  mydialect->insert("trim", new te::da::TemplateEncoder("trim", "(both $1 from $2)"));
  mydialect->insert("second", new te::da::TemplateEncoder("extract", "(second from $1)"));
  mydialect->insert("year", new te::da::TemplateEncoder("extract", "(year from $1)"));
  mydialect->insert("str", new te::da::TemplateEncoder("cast", "($1 as varchar)"));
  mydialect->insert("sum", new te::da::FunctionEncoder("sum"));

  mydialect->insert("geometrytype", new te::da::FunctionEncoder("geometrytype"));
  mydialect->insert("st_boundary", new te::da::FunctionEncoder("st_boundary"));
  mydialect->insert("st_coorddim", new te::da::FunctionEncoder("st_coorddim"));
  mydialect->insert("st_dimension", new te::da::FunctionEncoder("st_dimension"));
  mydialect->insert("st_endpoint", new te::da::FunctionEncoder("st_endpoint"));
  mydialect->insert("st_envelope", new te::da::FunctionEncoder("st_envelope"));
  mydialect->insert("st_exteriorring", new te::da::FunctionEncoder("st_exteriorring"));
  mydialect->insert("st_geometryn", new te::da::FunctionEncoder("st_geometryn"));
  mydialect->insert("st_geometrytype", new te::da::FunctionEncoder("st_geometrytype"));
  mydialect->insert("st_interiorringn", new te::da::FunctionEncoder("st_interiorringn"));
  mydialect->insert("st_isclosed", new te::da::FunctionEncoder("st_isclosed"));
  mydialect->insert("st_isempty", new te::da::FunctionEncoder("st_isempty"));
  mydialect->insert("st_isring", new te::da::FunctionEncoder("st_isring"));
  mydialect->insert("st_issimple", new te::da::FunctionEncoder("st_issimple"));
  mydialect->insert("st_isvalid", new te::da::FunctionEncoder("st_isvalid"));

  mydialect->insert("st_contains", new te::da::FunctionEncoder("st_contains"));
  mydialect->insert("st_covers", new te::da::FunctionEncoder("st_covers"));
  mydialect->insert("st_coveredby", new te::da::FunctionEncoder("st_coveredby"));
  mydialect->insert("st_crosses", new te::da::FunctionEncoder("st_crosses"));
  mydialect->insert("st_disjoint", new te::da::FunctionEncoder("st_disjoint"));
  mydialect->insert("st_equals", new te::da::FunctionEncoder("st_equals"));
  mydialect->insert("st_overlaps", new te::da::FunctionEncoder("st_overlaps"));
  mydialect->insert("st_relate", new te::da::FunctionEncoder("st_relate"));
  mydialect->insert("st_touches", new te::da::FunctionEncoder("st_touches"));
  mydialect->insert("st_intersects", new te::da::FunctionEncoder("st_intersects"));
  mydialect->insert("st_within", new te::da::FunctionEncoder("st_within"));

  mydialect->insert("st_ndims", new te::da::FunctionEncoder("st_ndims"));
  mydialect->insert("st_nrings", new te::da::FunctionEncoder("st_nrings"));
  mydialect->insert("st_numgeometries", new te::da::FunctionEncoder("st_numgeometries"));
  mydialect->insert("st_numinteriorrings", new te::da::FunctionEncoder("st_numinteriorrings"));
  mydialect->insert("st_numpoints", new te::da::FunctionEncoder("st_numpoints"));
  mydialect->insert("st_pointn", new te::da::FunctionEncoder("st_pointn"));
  mydialect->insert("st_srid", new te::da::FunctionEncoder("st_srid"));
  mydialect->insert("st_startpoint", new te::da::FunctionEncoder("st_startpoint"));
  mydialect->insert("st_x", new te::da::FunctionEncoder("st_x"));
  mydialect->insert("st_y", new te::da::FunctionEncoder("st_y"));
  mydialect->insert("st_z", new te::da::FunctionEncoder("st_z"));
  mydialect->insert("st_m", new te::da::FunctionEncoder("st_m"));
  mydialect->insert("st_transform", new te::da::FunctionEncoder("st_transform"));
  mydialect->insert("st_area", new te::da::FunctionEncoder("st_area"));
  mydialect->insert("st_centroid", new te::da::FunctionEncoder("st_centroid"));
  mydialect->insert("st_distance", new te::da::FunctionEncoder("st_distance"));
  mydialect->insert("st_dfullywithin", new te::da::FunctionEncoder("st_dfullywithin"));
  mydialect->insert("st_dwithin", new te::da::FunctionEncoder("st_dwithin"));
  mydialect->insert("st_length", new te::da::FunctionEncoder("st_length"));
  mydialect->insert("st_perimeter", new te::da::FunctionEncoder("st_perimeter"));
  mydialect->insert("st_buffer", new te::da::FunctionEncoder("st_buffer"));
  mydialect->insert("st_convexhull", new te::da::FunctionEncoder("st_convexhull"));
  mydialect->insert("st_difference", new te::da::FunctionEncoder("st_difference"));
  mydialect->insert("st_intersection", new te::da::FunctionEncoder("st_intersection"));
  mydialect->insert("st_symdifference", new te::da::FunctionEncoder("st_symdifference"));
  mydialect->insert("st_union", new te::da::FunctionEncoder("st_union"));

  return mydialect;
}