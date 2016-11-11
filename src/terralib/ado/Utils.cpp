/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file terralib/ado/Utils.cpp
   
  \brief Utility functions for ADO.
*/

// TerraLib
#include "../common/Exception.h"
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/uri/Utils.h"
#include "../datatype.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../dataaccess/dataset/ForeignKey.h"
#include "../dataaccess/dataset/CheckConstraint.h"
#include "../dataaccess/dataset/Constraint.h"
#include "../dataaccess/dataset/PrimaryKey.h"
#include "../dataaccess/dataset/UniqueKey.h"
#include "../dataaccess/utils/Utils.h"
#include "../geometry/GeometryProperty.h"
#include "../geometry/Envelope.h"
#include "../geometry/Enums.h"
#include "../memory/DataSet.h"
#include "../memory/DataSetItem.h"
#include "DataSet.h"
#include "Config.h"
#include "Utils.h"
#include "Globals.h"

// Boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

inline void TESTHR( HRESULT hr )
{
  if( FAILED(hr) ) _com_issue_error( hr );
}

void te::ado::Blob2Variant(const char* blob, int size, _variant_t & var)
{
  char *pByte;

  SAFEARRAY FAR* psa;
  SAFEARRAYBOUND rgsabound[1];
  rgsabound[0].lLbound = 0;
  rgsabound[0].cElements = size;

  psa = SafeArrayCreate(VT_I1, 1, rgsabound);

  if(SafeArrayAccessData(psa,(void **)&pByte) == NOERROR)
    memcpy(pByte, blob, size);
  SafeArrayUnaccessData(psa);

  var.vt = VT_ARRAY | VT_UI1;
  var.parray = psa;
}

std::string te::ado::MakeConnectionStr(const te::core::URI& connInfo)
{
  std::string connSTR;
  std::map<std::string, std::string> kvp = te::core::Expand(connInfo.query());

  std::string provider = kvp["PROVIDER"];
  std::string dbName = connInfo.host() + connInfo.path();
  std::string user = connInfo.user();
  std::string password = connInfo.password();

  if(!provider.empty())
    connSTR += "Provider=" + provider;

  if(!dbName.empty())
    connSTR += ";Data Source=" + dbName;

  if(!user.empty())
    connSTR += ";User Id=" + user;

  if(!password.empty())
    connSTR += ";Jet OLEDB:Database Password=" + password + ";";

  return connSTR;
}

void te::ado::Variant2Blob(const _variant_t var, int size, char* & blob)
{
  try
  {
    char *cdata = 0;
    if(var.vt == (VT_ARRAY | VT_UI1))
    {
      SafeArrayAccessData(var.parray,(void **)&cdata);
      blob = new char[size];
      memcpy(blob, cdata, size);
      SafeArrayUnaccessData(var.parray);
    }
  }
  catch(_com_error& e)
  {
    throw te::common::Exception((LPCSTR)e.ErrorMessage());
  }
}

ADOX::DataTypeEnum te::ado::Convert2Ado(int terralib)
{
  switch(terralib)
  {
  case te::dt::CHAR_TYPE:
  case te::dt::UCHAR_TYPE:
    return ADOX::adWChar;
    break;

  case te::dt::INT16_TYPE:
    return ADOX::adInteger;
    break;

  case te::dt::INT32_TYPE:
    return ADOX::adInteger;
    break;

  case te::dt::INT64_TYPE:
    return ADOX::adInteger;
    break;

  case te::dt::DATETIME_TYPE:
    return ADOX::adDate;

  case te::dt::FLOAT_TYPE:
  case te::dt::DOUBLE_TYPE:
  case te::dt::NUMERIC_TYPE:
    return ADOX::adDouble;
    break;

  case te::dt::STRING_TYPE:
    return ADOX::adLongVarWChar;
    break;

  case te::dt::BOOLEAN_TYPE:
    return ADOX::adBoolean;
    break;

  case te::dt::BYTE_ARRAY_TYPE:
    return ADOX::adLongVarBinary;
    break;

  case te::dt::GEOMETRY_TYPE:
  case te::dt::ARRAY_TYPE:
    return ADOX::adLongVarBinary;
    break;

  default:
    throw te::common::Exception(TE_TR("The informed type could not be mapped to ADO type system!"));
    break;
  }
}

std::string te::ado::GetAdoStringType(const int& terralib)
{
  ADOX::DataTypeEnum aType = te::ado::Convert2Ado(terralib);

  switch(aType) 
  {
    case ADOX::adInteger:
      return "Integer";
    break;

    case ADOX::adDate:
      return "Date/Time";
    break;

    case ADOX::adDouble:
      return "Double";
    break;

    case ADOX::adBoolean:
      return "Yes/No";
    break;

    case ADOX::adLongVarBinary:
      return "Memo";
    break;

    case ADOX::adLongVarWChar:
      return "Text";
    break;
  }

  return "";
}

void te::ado::Convert2Ado(const te::gm::Geometry* geo, _variant_t & var)
{
  long size = (long)geo->getWkbSize();

  char* wkb = new char[size];

  geo->getWkb(wkb, te::common::NDR);

  unsigned int newWkbSize = size+4;

  char* newWkb = new char[newWkbSize];

  memcpy(newWkb, wkb, size);

  unsigned int srid = geo->getSRID();

  memcpy(newWkb+size, &srid, 4);

  te::ado::Blob2Variant(newWkb, newWkbSize, var);
}

int te::ado::Convert2Terralib(ADOX::DataTypeEnum adoType)
{
  switch(adoType)
  {
    case ADOX::adBoolean:
      return te::dt::BOOLEAN_TYPE;
    break;

    case ADOX::adEmpty:
      return te::dt::VOID_TYPE;
    break;

    case ADOX::adBinary:
    case ADOX::adVarBinary:
    case ADOX::adLongVarBinary:
      return te::dt::BYTE_ARRAY_TYPE;
    break;

    case ADOX::adVarWChar:
    case ADOX::adWChar:
    case ADOX::adVarChar:
    case ADOX::adLongVarChar:
    case ADOX::adLongVarWChar:
    case ADOX::adBSTR:
    case ADOX::adChar:
      return te::dt::STRING_TYPE;
    break;

    case ADOX::adBigInt:
      return te::dt::INT16_TYPE;
    break;

    case ADOX::adSingle:
      return te::dt::FLOAT_TYPE;
    break;

    case ADOX::adDouble:
    case ADOX::adDecimal:
      return te::dt::DOUBLE_TYPE;
    break;

    case ADOX::adInteger:
      return te::dt::INT32_TYPE;
    break;

    case ADOX::adTinyInt:
    case ADOX::adSmallInt:
      return te::dt::INT16_TYPE;
    break;

    case ADOX::adUnsignedBigInt:
      return te::dt::UINT64_TYPE;
    break;

    case ADOX::adUnsignedInt:
      return te::dt::UINT32_TYPE;
    break;

    case ADOX::adUnsignedSmallInt:
    case ADOX::adUnsignedTinyInt:
      return te::dt::UINT16_TYPE;
    break;

    case ADOX::adDate:
    case ADOX::adDBDate:
    case ADOX::adDBTime:
    case ADOX::adDBTimeStamp:
      return te::dt::DATETIME_TYPE;
    break;

    //case ADOX::adGUID:
    //case ADOX::adError:
    //case ADOX::adSingle:
    //case ADOX::adDecimal:
    //case ADOX::adNumeric:
    //case ADOX::adChapter:
    //case ADOX::adVarNumeric:
    //case ADOX::adCurrency:
    //case ADOX::adFileTime:
    //case ADOX::adPropVariant:
    //case ADOX::adUserDefined:

    default:
      return te::dt::UNKNOWN_TYPE;
    break;
  }
}

int te::ado::Convert2Terralib(::DataTypeEnum adoType)
{
  switch(adoType)
  {
    case ::adBoolean:
      return te::dt::BOOLEAN_TYPE;
    break;

    case ::adEmpty:
      return te::dt::VOID_TYPE;
    break;

    case ::adBinary:
    case ::adVarBinary:
    case ::adLongVarBinary:
      return te::dt::BYTE_ARRAY_TYPE;
    break;

    case ::adVarWChar:
    case ::adWChar:
    case ::adVarChar:
    case ::adLongVarChar:
    case ::adLongVarWChar:
    case ::adBSTR:
    case ::adChar:
      return te::dt::STRING_TYPE;
    break;

    case ::adBigInt:
      return te::dt::INT16_TYPE;
    break;

    case ::adSingle:
      return te::dt::FLOAT_TYPE;
    break;

    case ::adDouble:
    case ::adDecimal:
      return te::dt::DOUBLE_TYPE;
    break;

    case ::adInteger:
      return te::dt::INT32_TYPE;
    break;

    case ::adTinyInt:
    case ::adSmallInt:
      return te::dt::INT16_TYPE;
    break;

    case ::adUnsignedBigInt:
      return te::dt::UINT64_TYPE;
    break;

    case ::adUnsignedInt:
      return te::dt::UINT32_TYPE;
    break;

    case ::adUnsignedSmallInt:
    case ::adUnsignedTinyInt:
      return te::dt::UINT16_TYPE;
    break;

    case ::adDate:
    case ::adDBDate:
    case ::adDBTime:
    case ::adDBTimeStamp:
      return te::dt::DATETIME_TYPE;
    break;

    //case ::adGUID:
    //case ::adError:
    //case ::adSingle:
    //case ::adDecimal:
    //case ::adNumeric:
    //case ::adChapter:
    //case ::adVarNumeric:
    //case ::adCurrency:
    //case ::adFileTime:
    //case ::adPropVariant:
    //case ::adUserDefined:

    default:
      return te::dt::UNKNOWN_TYPE;
    break;
  }
}

te::dt::Property* te::ado::Convert2Terralib(ADOX::_ColumnPtr column)
{
  te::dt::Property* prop = 0;

  _bstr_t cName = column->GetName();

  ADOX::DataTypeEnum cType = column->GetType();

  long cSize = column->GetDefinedSize();
      
  switch(cType)
  {
    case ::adBoolean:
      prop = new te::dt::SimpleProperty(std::string(cName), Convert2Terralib(cType));
    break;

    case ::adVarWChar:
    case ::adWChar:
    case ::adVarChar:
    case ::adLongVarChar:
    case ::adLongVarWChar:
    case ::adBSTR:
    case ::adChar:
      prop = new te::dt::StringProperty(std::string(cName), (te::dt::StringType)Convert2Terralib(cType), cSize);
    break;

    case ADOX::adTinyInt:
    case ADOX::adSmallInt:
      prop = new te::dt::SimpleProperty(std::string(cName), Convert2Terralib(cType));
    break;

    case ADOX::adInteger:
      prop = new te::dt::SimpleProperty(std::string(cName), Convert2Terralib(cType));
    break;

    case ADOX::adBigInt:
      prop = new te::dt::SimpleProperty(std::string(cName), Convert2Terralib(cType));
    break;

    case ADOX::adDouble:
    case ADOX::adDecimal:
      prop = new te::dt::SimpleProperty(std::string(cName), Convert2Terralib(cType));
    break;

    case ::adUnsignedBigInt:
      prop = new te::dt::SimpleProperty(std::string(cName), Convert2Terralib(cType));
    break;

    case ::adUnsignedInt:
      prop = new te::dt::SimpleProperty(std::string(cName), Convert2Terralib(cType));
    break;

    case ::adUnsignedSmallInt:
    case ::adUnsignedTinyInt:
      prop = new te::dt::SimpleProperty(std::string(cName),Convert2Terralib(cType));
    break;

    case ADOX::adLongVarBinary:
      prop = new te::dt::ArrayProperty(std::string(cName), new te::dt::SimpleProperty(std::string(cName), Convert2Terralib(cType)));
    break;

    case ADOX::adDate:
    case ADOX::adDBDate:
      prop = new te::dt::DateTimeProperty(std::string(cName), te::dt::DATE);
    break;

    case ADOX::adDBTime:
      prop = new te::dt::DateTimeProperty(std::string(cName), te::dt::TIME_DURATION);
    break;

    case ADOX::adDBTimeStamp:
      prop = new te::dt::DateTimeProperty(std::string(cName), te::dt::TIME_INSTANT);
    break;
          
    default:
      throw te::common::Exception(TE_TR("The informed column could not be mapped to TerraLib Data Set Type!"));
  }

  return prop;
}

std::vector<te::dt::Property*> te::ado::Convert2Terralib(ADOX::ColumnsPtr columns)
{
  std::vector<te::dt::Property*> properties;

  for(long i = 0; i < columns->GetCount(); i++)
    properties.push_back(Convert2Terralib(columns->GetItem(i)));

  return properties;
}

te::da::Constraint* te::ado::Convert2Terralib(ADOX::_KeyPtr key)
{

  if(key->GetType() == ADOX::adKeyPrimary)
  {
    te::da::PrimaryKey* con = new te::da::PrimaryKey(std::string(key->GetName()));
    con->setProperties(Convert2Terralib(key->GetColumns()));

    return con;
  }
  else if(key->GetType() == ADOX::adKeyForeign)
  {
    te::da::ForeignKey* con = new te::da::ForeignKey(std::string(key->GetName()));
    con->setProperties(Convert2Terralib(key->GetColumns()));

    return con;
  }
  else if(key->GetType() == ADOX::adKeyUnique)
  {
    te::da::UniqueKey* con = new te::da::UniqueKey(std::string(key->GetName()));
    con->setProperties(Convert2Terralib(key->GetColumns()));

    return con;
  }
  else
    throw te::common::Exception(TE_TR("Unknown type!"));

}

const std::string& te::ado::GetGeometryName(te::gm::GeomType t)
{
  switch(t)
  {
  case te::gm::GeometryType:
    return Globals::sm_geometryTypeName;
    break;

  case te::gm::PointType:
    return Globals::sm_pointTypeName;
    break;

  case te::gm::LineStringType:
    return Globals::sm_lineStringTypeName;
    break;

  case te::gm::PolygonType:
    return Globals::sm_polygonTypeName;
    break;

  case te::gm::MultiPointType:
    return Globals::sm_multiPointTypeName;
    break;

  case te::gm::MultiLineStringType:
    return Globals::sm_multiLineStringTypeName;
    break;

  case te::gm::MultiPolygonType:
    return Globals::sm_multiPolygonTypeName;
    break;

  case te::gm::CurvePolygonType:
    return Globals::sm_curvePolygonTypeName;
    break;

  case te::gm::PolyhedralSurfaceType:
    return Globals::sm_polyhedralSurfaceTypeName;
    break;

  case te::gm::GeometryZType:
    return Globals::sm_geometryZTypeName;
    break;

  case te::gm::PointZType:
    return Globals::sm_pointZTypeName;
    break;

  case te::gm::LineStringZType:
    return Globals::sm_lineStringZTypeName;
    break;

  case te::gm::PolygonZType:
    return Globals::sm_polygonZTypeName;
    break;

  case te::gm::MultiPointZType:
    return Globals::sm_multiPointZTypeName;
    break;

  case te::gm::MultiLineStringZType:
    return Globals::sm_multiLineStringZTypeName;
    break;

  case te::gm::MultiPolygonZType:
    return Globals::sm_multiPolygonZTypeName;
    break;

  case te::gm::CurvePolygonZType:
    return Globals::sm_curvePolygonZTypeName;
    break;

  case te::gm::PolyhedralSurfaceZType:
    return Globals::sm_polyhedralSurfaceZTypeName;
    break;

  case te::gm::GeometryMType:
    return Globals::sm_geometryMTypeName;
    break;

  case te::gm::PointMType:
    return Globals::sm_pointMTypeName;
    break;

  case te::gm::LineStringMType:
    return Globals::sm_lineStringMTypeName;
    break;

  case te::gm::PolygonMType:
    return Globals::sm_polygonMTypeName;
    break;

  case te::gm::MultiPointMType:
    return Globals::sm_multiPointMTypeName;
    break;

  case te::gm::MultiLineStringMType:
    return Globals::sm_multiLineStringMTypeName;
    break;

  case te::gm::MultiPolygonMType:
    return Globals::sm_multiPolygonMTypeName;
    break;

  case te::gm::CurvePolygonMType:
    return Globals::sm_curvePolygonMTypeName;
    break;

  case te::gm::PolyhedralSurfaceMType:
    return Globals::sm_polyhedralSurfaceMTypeName;
    break;

  case te::gm::GeometryZMType:
    return Globals::sm_geometryZMTypeName;
    break;

  case te::gm::PointZMType:
    return Globals::sm_pointZMTypeName;
    break;

  case te::gm::LineStringZMType:
    return Globals::sm_lineStringZMTypeName;
    break;

  case te::gm::PolygonZMType:
    return Globals::sm_polygonZMTypeName;
    break;

  case te::gm::MultiPointZMType:
    return Globals::sm_multiPointZMTypeName;
    break;

  case te::gm::MultiLineStringZMType:
    return Globals::sm_multiLineStringZMTypeName;
    break;

  case te::gm::MultiPolygonZMType:
    return Globals::sm_multiPolygonZMTypeName;
    break;

  case te::gm::CurvePolygonZMType:
    return Globals::sm_curvePolygonZMTypeName;
    break;

  case te::gm::PolyhedralSurfaceZMType:
    return Globals::sm_polyhedralSurfaceZMTypeName;
    break;

  default:
    return Globals::sm_geometryTypeName;
  }
}

const te::gm::GeomType te::ado::GetGeometryType(std::string t)
{
  
  if(t == Globals::sm_geometryTypeName)
    return te::gm::GeometryType;

  else if(t == Globals::sm_pointTypeName)
    return te::gm::PointType;


  else if(t == Globals::sm_lineStringTypeName)
    return te::gm::LineStringType;


  else if(t == Globals::sm_polygonTypeName)
    return te::gm::PolygonType;


  else if(t == Globals::sm_multiPointTypeName)
    return te::gm::MultiPointType;


  else if(t == Globals::sm_multiLineStringTypeName)
    return te::gm::MultiLineStringType;


  else if(t == Globals::sm_multiPolygonTypeName)
    return te::gm::MultiPolygonType;


  else if(t == Globals::sm_curvePolygonTypeName)
    return te::gm::CurvePolygonType;


  else if(t == Globals::sm_polyhedralSurfaceTypeName)
    return te::gm::PolyhedralSurfaceType;


  else if(t == Globals::sm_geometryZTypeName)
    return te::gm::GeometryZType;


  else if(t == Globals::sm_pointZTypeName)
    return te::gm::PointZType;


  else if(t == Globals::sm_lineStringZTypeName)
    return te::gm::LineStringZType;


  else if(t == Globals::sm_polygonZTypeName)
    return te::gm::PolygonZType;


  else if(t == Globals::sm_multiPointZTypeName)
    return te::gm::MultiPointZType;


  else if(t == Globals::sm_multiLineStringZTypeName)
    return te::gm::MultiLineStringZType;


  else if(t == Globals::sm_multiPolygonZTypeName)
    return te::gm::MultiPolygonZType;


  else if(t == Globals::sm_curvePolygonZTypeName)

    return te::gm::CurvePolygonZType;

  else if(t == Globals::sm_polyhedralSurfaceZTypeName)
    return te::gm::PolyhedralSurfaceZType;


  else if(t == Globals::sm_geometryMTypeName)
    return te::gm::GeometryMType;


  else if(t == Globals::sm_pointMTypeName)
    return te::gm::PointMType;


  else if(t == Globals::sm_lineStringMTypeName)
    return te::gm::LineStringMType;


  else if(t == Globals::sm_polygonMTypeName)
    return te::gm::PolygonMType;


  else if(t == Globals::sm_multiPointMTypeName)
    return te::gm::MultiPointMType;


  else if(t == Globals::sm_multiLineStringMTypeName)
    return te::gm::MultiLineStringMType;


  else if(t == Globals::sm_multiPolygonMTypeName)
    return te::gm::MultiPolygonMType;


  else if(t == Globals::sm_curvePolygonMTypeName)
    return te::gm::CurvePolygonMType;


  else if(t == Globals::sm_polyhedralSurfaceMTypeName)
    return te::gm::PolyhedralSurfaceMType;


  else if(t == Globals::sm_geometryZMTypeName)
    return te::gm::GeometryZMType;


  else if(t == Globals::sm_pointZMTypeName)
    return te::gm::PointZMType;


  else if(t == Globals::sm_lineStringZMTypeName)
    return te::gm::LineStringZMType;


  else if(t == Globals::sm_polygonZMTypeName)
    return te::gm::PolygonZMType;


  else if(t == Globals::sm_multiPointZMTypeName)
    return te::gm::MultiPointZMType;


  else if(t == Globals::sm_multiLineStringZMTypeName)
    return te::gm::MultiLineStringZMType;


  else if(t == Globals::sm_multiPolygonZMTypeName)
    return te::gm::MultiPolygonZMType;


  else if(t == Globals::sm_curvePolygonZMTypeName)
    return te::gm::CurvePolygonZMType;


  else if(t == Globals::sm_polyhedralSurfaceZMTypeName)
    return te::gm::PolyhedralSurfaceZMType;

  else
    return te::gm::UnknownGeometryType;
  
}

int te::ado::GetSRID(_ConnectionPtr adoConn, std::string tableName, std::string geomPropName)
{
  _RecordsetPtr recset;
  TESTHR(recset.CreateInstance(__uuidof(Recordset)));

  std::string sel = "SELECT * FROM geometry_columns where f_table_name = '";
  sel += tableName + "' and f_geometry_column = '" + geomPropName + "'";

  _variant_t variantSel;
  variantSel.SetString(sel.c_str());

  try
  {
    TESTHR(recset->Open(variantSel, _variant_t((IDispatch *)adoConn), 
      adOpenStatic, adLockReadOnly, adCmdText));
  }
  catch(_com_error& e)
  {
    throw te::common::Exception((LPCSTR)e.ErrorMessage());
  }

  return (int32_t)recset->GetFields()->GetItem("srid")->GetValue();
}

te::gm::GeomType te::ado::GetType(_ConnectionPtr adoConn, std::string tableName, std::string geomPropName)
{
  _RecordsetPtr recset;
  TESTHR(recset.CreateInstance(__uuidof(Recordset))); 

  std::string sel = "SELECT * FROM geometry_columns where f_table_name = '";
  sel += tableName + "' and f_geometry_column = '" + geomPropName + "'";

  _variant_t variantSel;
  variantSel.SetString(sel.c_str());

  try
  {
    TESTHR(recset->Open(variantSel, _variant_t((IDispatch *)adoConn), 
      adOpenStatic, adLockReadOnly, adCmdText));
  }
  catch(_com_error& e)
  {
    throw te::common::Exception((LPCSTR)e.ErrorMessage());
  }

  std::string type = (LPCSTR)(_bstr_t)recset->GetFields()->GetItem("type")->GetValue();

  return GetGeometryType(type);
}

bool te::ado::IsGeomProperty(_ConnectionPtr adoConn, std::string tableName, std::string columnName)
{
  ADOX::_CatalogPtr pCatalog;

  TESTHR(pCatalog.CreateInstance(__uuidof(ADOX::Catalog)));

  try
  {
    pCatalog->PutActiveConnection(variant_t((IDispatch *)adoConn));

    for(long i = 0; i < pCatalog->GetTables()->Count; i++)
    {
      if(std::string(pCatalog->GetTables()->GetItem(i)->GetName()) == "geometry_columns")
      {
        _RecordsetPtr recset;
        TESTHR(recset.CreateInstance(__uuidof(Recordset)));
  
        TESTHR(recset->Open(_bstr_t("geometry_columns"),
        _variant_t((IDispatch*)adoConn,true), adOpenKeyset, adLockOptimistic, adCmdTable));

        while(!recset->EndOfFile)
        {
          if(std::string((_bstr_t)recset->Fields->GetItem("f_table_name")->Value) == tableName
            && std::string((_bstr_t)recset->Fields->GetItem("f_geometry_column")->Value) == columnName)
            return true;

          recset->MoveNext();
        }
      }
    }
  }
  catch(_com_error& e)
  {
    throw te::common::Exception((LPCSTR)e.ErrorMessage());
  }

  return false;
}

bool te::ado::IsZProperty(te::gm::GeomType type)
{
  if( (type >= 1000 && type < 2000) || (type >= 3000 && type < 4000) )
    return true;
  return false;
}

std::string te::ado::GetSystemDateTimeFormat(std::string& indAM, std::string& indPM, std::string& sepD, std::string& sepT)
{
  std::string key = "Control Panel\\International";
  std::string dateFormat = "sShortDate";	//formato da data d/M/yyyy
  std::string timeFormat = "sTimeFormat";	//formato da hora HH:mm:ss
  std::string hourFormat = "iTime";		//12 horas (0) ou 24 horas(1)
  std::string indicatorAM = "s1159";		//AM
  std::string indicatorPM = "s2359";		//PM
  std::string dateSeparator = "sDate";		//separador de data
  std::string timeSeparator = "sTime";		//separador de hora

  std::string rdateFormat;
  std::string rtimeFormat;
  std::string rhourFormat;
  std::string rdateSeparator;
  std::string rtimeSeparator; 

  HKEY    hk;
  DWORD	DataSize = 1024;
  DWORD   Type = REG_SZ;
  char    buf[1024];
    
  if (RegOpenKeyExA(HKEY_CURRENT_USER, key.c_str(), 0, KEY_READ, &hk) == ERROR_SUCCESS)
  {
    memset (buf, 0, 1024);
    DataSize = 1024;
    //date format
    if (RegQueryValueExA(hk, dateFormat.c_str(), NULL, &Type, (LPBYTE)buf, &DataSize) == ERROR_SUCCESS)
      rdateFormat = buf;

    memset (buf, 0, 1024);
    DataSize = 1024;
    //date separator
    if (RegQueryValueExA(hk, dateSeparator.c_str(), NULL, &Type, (LPBYTE)buf, &DataSize) == ERROR_SUCCESS)
      rdateSeparator = buf;

    memset (buf, 0, 1024);
    DataSize = 1024;
    //time format
    if (RegQueryValueExA(hk, timeFormat.c_str(), NULL, &Type, (LPBYTE)buf, &DataSize) == ERROR_SUCCESS)
      rtimeFormat = buf;

    memset (buf, 0, 1024);
    DataSize = 1024;
    //hour format
    if (RegQueryValueExA(hk, hourFormat.c_str(), NULL, &Type, (LPBYTE)buf, &DataSize) == ERROR_SUCCESS)
      rhourFormat = buf;

    memset (buf, 0, 1024);
    DataSize = 1024;
    //indicator AM
    if (RegQueryValueExA(hk, indicatorAM.c_str(), NULL, &Type, (LPBYTE)buf, &DataSize) == ERROR_SUCCESS)
      indAM = buf;

    memset (buf, 0, 1024);
    DataSize = 1024;
    //indicator PM 
    if (RegQueryValueExA(hk, indicatorPM.c_str(), NULL, &Type, (LPBYTE)buf, &DataSize) == ERROR_SUCCESS)
      indPM = buf;

    memset (buf, 0, 1024);
    DataSize = 1024;
    //time separator
    if (RegQueryValueExA(hk, timeSeparator.c_str(), NULL, &Type, (LPBYTE)buf, &DataSize) == ERROR_SUCCESS)
      rtimeSeparator = buf;
  }
  else
    return "";

  sepD = rdateSeparator;
  sepT = rtimeSeparator;

  //DATE
  //first
  int pos = (int)rdateFormat.find(rdateSeparator);
  std::string firstD = rdateFormat.substr(0,pos);
  std::string temp = rdateFormat.substr(pos+1);
  //second and third
  pos = (int)temp.find(rdateSeparator);
  std::string secondD = temp.substr(0,pos);
  std::string thirdD = temp.substr(pos+1);

  //passar para o formato
  if(firstD.find("a")==0)
    replace(firstD.begin(), firstD.end(), 97, 89);
  else if(secondD.find("a")==0)
    replace(secondD.begin(), secondD.end(),97, 89);
  else if(thirdD.find("a")==0)
    replace(thirdD.begin(), thirdD.end(), 97, 89);

  //TIME
  //first
  pos = (int)rtimeFormat.find(rtimeSeparator);
  std::string firstT = rtimeFormat.substr(0,pos);
  temp = rtimeFormat.substr(pos+1);
  //second and third
  pos = (int)temp.find(rtimeSeparator);
  std::string secondT = temp.substr(0,pos);
  int posEmpth = (int)temp.find(" ");
  std::string thirdT;

  if(posEmpth==-1)
  thirdT = temp.substr(pos+1);
  else
  thirdT = temp.substr(pos+1, (posEmpth-(pos+1)));

  int hFormat = atoi(rhourFormat.c_str());

  //passar para o formato
  
  firstT = te::common::Convert2UCase(firstT);
  secondT = te::common::Convert2UCase(secondT);
  thirdT = te::common::Convert2UCase(thirdT);

  if((firstT.find("M")==0))
    replace(firstT.begin(), firstT.end(), 77, 109);
  else if(secondT.find("M")==0)
    replace(secondT.begin(), secondT.end(), 77, 109);
  else if(thirdT.find("M")==0)
    replace(thirdT.begin(), thirdT.end(), 77, 109);

  std::string timef;
  if(hFormat==0)
    timef = "sTT";
  else
    timef = "";

  std::string result = te::common::Convert2UCase(firstD) +"s"+ 
  te::common::Convert2UCase(secondD) +"s"+ 
  te::common::Convert2UCase(thirdD) +"s"+ 
  firstT +"s"+ 
  secondT +"s"+ 
  thirdT + timef;

  RegCloseKey (hk);
  return result;
}

int te::ado::GetMonth(const std::string& month)
{
  std::string tempM = te::common::Convert2UCase(month);
  if(tempM=="JAN")
    return 1;
  else if(tempM=="FEB")
    return 2;
  else if(tempM=="MAR")
    return 3;
  else if(tempM=="APR")
    return 4;
  else if(tempM=="MAY")
    return 5; 
  else if(tempM=="JUN")
    return 6;
  else if(tempM=="JUL")
    return 7;
  else if(tempM=="AUG")
    return 8;
  else if(tempM=="SEP")
    return 9;
  else if(tempM=="OCT")
    return 12;
  else if(tempM=="NOV")
    return 11;
  else if(tempM=="DEC")
    return 12;

  return -1;
}

std::auto_ptr<te::dt::DateTime> te::ado::GetDateTime(std::string& value, std::string& mask, 
                                                     std::string& sepD, std::string& sepT)
{
  int sec = 0;
  int min = 0;
  int hour = 0;
  int day = 0;
  int mon = 0;
  int year = 0;

  if(mask == "DDsMMsYYYYsHHsmmsSS")
  {
    std::vector<std::string> tokGeneral;
    te::common::Tokenize(value, tokGeneral, " ");

   // Only Date or only Time
    if(tokGeneral.size() == 1)
    {
      std::vector<std::string> tokDate;
      te::common::Tokenize(tokGeneral[0], tokDate, sepD);
      std::vector<std::string> tokTime;
      te::common::Tokenize(tokGeneral[0], tokTime, sepT);

      // Only Date
      if(tokDate.size() > 1)
      {
        day = boost::lexical_cast<int>(tokDate[0]);
        mon = boost::lexical_cast<int>(tokDate[1]);
        year = boost::lexical_cast<int>(tokDate[2]);
      }
      // Only Time
      else if(tokTime.size() > 1)
      {
        hour = boost::lexical_cast<int>(tokTime[0]);
        min = boost::lexical_cast<int>(tokTime[1]);
        sec = boost::lexical_cast<int>(tokTime[2]);
      }
    }
    // Date and Time
    else if(tokGeneral.size() == 2)
    {
      std::vector<std::string> tokDate;
      te::common::Tokenize(tokGeneral[0], tokDate, sepD);
      std::vector<std::string> tokTime;
      te::common::Tokenize(tokGeneral[1], tokTime, sepT);

      day = boost::lexical_cast<int>(tokDate[0]);
      mon = boost::lexical_cast<int>(tokDate[1]);
      year = boost::lexical_cast<int>(tokDate[2]);
      hour = boost::lexical_cast<int>(tokTime[0]);
      min = boost::lexical_cast<int>(tokTime[1]);
      sec = boost::lexical_cast<int>(tokTime[2]);
    }
  }
  else if(mask == "MsDsYYYYsHsmmsSSsTT")
  {
    std::vector<std::string> tokGeneral;
    te::common::Tokenize(value, tokGeneral, " ");

    // Only Date
    if(tokGeneral.size() == 1)
    {
      std::vector<std::string> tokDate;
      te::common::Tokenize(tokGeneral[0], tokDate, sepD);
      mon = boost::lexical_cast<int>(tokDate[0]);
      day = boost::lexical_cast<int>(tokDate[1]);
      year = boost::lexical_cast<int>(tokDate[2]);
    }
    // Only Time
    else if(tokGeneral.size() == 2)
    {
      std::vector<std::string> tokTime;
      te::common::Tokenize(tokGeneral[0], tokTime, sepT);
      hour = boost::lexical_cast<int>(tokTime[0]);
      min = boost::lexical_cast<int>(tokTime[1]);
      sec = boost::lexical_cast<int>(tokTime[2]);
    }
    // Date and Time
    else if(tokGeneral.size() == 3)
    {
      std::vector<std::string> tokDate;
      std::vector<std::string> tokTime;
      te::common::Tokenize(tokGeneral[0], tokDate, sepD);
      te::common::Tokenize(tokGeneral[1], tokTime, sepT);

      mon = boost::lexical_cast<int>(tokDate[0]);
      day = boost::lexical_cast<int>(tokDate[1]);
      year = boost::lexical_cast<int>(tokDate[2]);
      hour = boost::lexical_cast<int>(tokTime[0]);
      min = boost::lexical_cast<int>(tokTime[1]);
      sec = boost::lexical_cast<int>(tokTime[2]);
    }
  }
  else if(mask == "YYYYsMMsDDsHHsmmsSS")
  {
    std::vector<std::string> tokGeneral;
    te::common::Tokenize(value, tokGeneral, " ");

   // Only Date or only Time
    if(tokGeneral.size() == 1)
    {
      std::vector<std::string> tokDate;
      te::common::Tokenize(tokGeneral[0], tokDate, sepD);
      std::vector<std::string> tokTime;
      te::common::Tokenize(tokGeneral[0], tokTime, sepT);

      // Only Date
      if(tokDate.size() > 1)
      {
        year = boost::lexical_cast<int>(tokDate[0]);
        mon = boost::lexical_cast<int>(tokDate[1]);
        day = boost::lexical_cast<int>(tokDate[2]);
      }
      // Only Time
      else if(tokTime.size() > 1)
      {
        hour = boost::lexical_cast<int>(tokTime[0]);
        min = boost::lexical_cast<int>(tokTime[1]);
        sec = boost::lexical_cast<int>(tokTime[2]);
      }
    }
    // Date and Time
    else if(tokGeneral.size() == 2)
    {
      std::vector<std::string> tokDate;
      te::common::Tokenize(tokGeneral[0], tokDate, sepD);
      std::vector<std::string> tokTime;
      te::common::Tokenize(tokGeneral[1], tokTime, sepT);

      year = boost::lexical_cast<int>(tokDate[0]);
      mon = boost::lexical_cast<int>(tokDate[1]);
      day = boost::lexical_cast<int>(tokDate[2]);
      hour = boost::lexical_cast<int>(tokTime[0]);
      min = boost::lexical_cast<int>(tokTime[1]);
      sec = boost::lexical_cast<int>(tokTime[2]);
    }
  }
  else
    throw te::common::Exception((boost::format(TE_TR("DateTime format not provided or invalid: %1%!")) % mask).str());

  te::dt::DateTime* result = 0;

  if((day > 0 || mon > 0 || year > 0) && (sec > 0 || min > 0 || hour > 0))
  {
    te::dt::Date d((unsigned short)year, (unsigned short)mon, (unsigned short)day);
    te::dt::TimeDuration td(hour, min, sec);
    result = new te::dt::TimeInstant(d, td);
  }
  else if(day > 0 || mon > 0 || year > 0)
  {
    result = new te::dt::Date((unsigned short)year, (unsigned short)mon, (unsigned short)day);
  }
  else if(sec > 0 || min > 0 || hour > 0)
  {
    result = new te::dt::TimeDuration(hour, min, sec);
  }
  else
    return std::auto_ptr<te::dt::DateTime>(0);

  return std::auto_ptr<te::dt::DateTime>(result);
}

std::string te::ado::GetFormattedDateTime(te::dt::DateTime* dateTime)
{
  std::string result = "";

  te::dt::Date* dtime = dynamic_cast<te::dt::Date*>(dateTime);

  std::string mAM, mPM, sepD, sepT;

  std::string mask = GetSystemDateTimeFormat(mAM, mPM, sepD, sepT);

  if(dtime)
  {
    if(mask.find("DDsMMsYYYY") != std::string::npos)
    {
      result = boost::lexical_cast<std::string>(dtime->getDay()) + sepD;
      result += boost::lexical_cast<std::string>(GetMonth(boost::lexical_cast<std::string>(dtime->getMonth()))) + sepD;
      result += boost::lexical_cast<std::string>(dtime->getYear());
    }
    else if(mask.find("MsDsYYYY") != std::string::npos)
    {
      result = boost::lexical_cast<std::string>(GetMonth(boost::lexical_cast<std::string>(dtime->getMonth()))) + sepD;
      result += boost::lexical_cast<std::string>(dtime->getDay()) + sepD;
      result += boost::lexical_cast<std::string>(dtime->getYear());
    }
    else if(mask.find("YYYYsMMsDD") != std::string::npos)
    {
      result = boost::lexical_cast<std::string>(dtime->getYear()) + sepD;
      result += boost::lexical_cast<std::string>(GetMonth(boost::lexical_cast<std::string>(dtime->getMonth()))) + sepD;
      result += boost::lexical_cast<std::string>(dtime->getDay());
    }
  }

  te::dt::TimeDuration* tduration = dynamic_cast<te::dt::TimeDuration*>(dateTime);
  
  if(tduration)
  {
    if(mask.find("HHsmmsSS") != std::string::npos)
    {
      result = boost::lexical_cast<std::string>(tduration->getHours()) + sepT;
      result += boost::lexical_cast<std::string>(tduration->getMinutes()) + sepT;
      result += boost::lexical_cast<std::string>(tduration->getSeconds());
    }
    else if(mask.find("HsmmsSSsTT") != std::string::npos)
    {
      result = boost::lexical_cast<std::string>(tduration->getHours()) + sepT;
      result += boost::lexical_cast<std::string>(tduration->getMinutes()) + sepT;
      result += boost::lexical_cast<std::string>(tduration->getSeconds());
    }
  }

  te::dt::TimeInstant* tinst = dynamic_cast<te::dt::TimeInstant*>(dateTime);

  if(tinst)
  {
    te::dt::Date date = tinst->getDate();
    te::dt::TimeDuration time = tinst->getTime();

    if(mask == "DDsMMsYYYYsHHsmmsSS")
    {
      result = boost::lexical_cast<std::string>(date.getDay()) + sepD;
      result += boost::lexical_cast<std::string>(GetMonth(boost::lexical_cast<std::string>(date.getMonth()))) + sepD;
      result += boost::lexical_cast<std::string>(date.getYear()) + " ";
      result += boost::lexical_cast<std::string>(time.getHours()) + sepT;
      result += boost::lexical_cast<std::string>(time.getMinutes()) + sepT;
      result += boost::lexical_cast<std::string>(time.getSeconds());
    }
    else if(mask == "MsDsYYYYsHsmmsSSsTT")
    {
      result = boost::lexical_cast<std::string>(GetMonth(boost::lexical_cast<std::string>(date.getMonth()))) + sepD;
      result += boost::lexical_cast<std::string>(date.getDay()) + sepD;
      result += boost::lexical_cast<std::string>(date.getYear()) + " ";
      result += boost::lexical_cast<std::string>(time.getHours()) + sepT;
      result += boost::lexical_cast<std::string>(time.getMinutes()) + sepT;
      result += boost::lexical_cast<std::string>(time.getSeconds());
    }
    else if(mask == "YYYYsMMsDDsHHsmmsSS")
    {
      result = boost::lexical_cast<std::string>(date.getYear()) + sepD;
      result += boost::lexical_cast<std::string>(GetMonth(boost::lexical_cast<std::string>(date.getMonth()))) + sepD;
      result += boost::lexical_cast<std::string>(date.getDay()) + " ";
      result += boost::lexical_cast<std::string>(time.getHours()) + sepT;
      result += boost::lexical_cast<std::string>(time.getMinutes()) + sepT;
      result += boost::lexical_cast<std::string>(time.getSeconds());
    }
  }

  te::dt::TimeInstantTZ* tinstZ = dynamic_cast<te::dt::TimeInstantTZ*>(dateTime);
  
  if(tinstZ)
  {
    boost::local_time::local_date_time boostTime = tinstZ->getTimeInstantTZ();    

    int hh = boostTime.utc_time().time_of_day().hours();
    int mm = boostTime.utc_time().time_of_day().minutes();
    int ss = boostTime.utc_time().time_of_day().seconds();

    if(mask.find("HHsmmsSS") != std::string::npos)
    {
      result = boost::lexical_cast<std::string>(hh) + sepT;
      result += boost::lexical_cast<std::string>(mm) + sepT;
      result += boost::lexical_cast<std::string>(ss);
    }
    else if(mask.find("HsmmsSSsTT") != std::string::npos)
    {
      result = boost::lexical_cast<std::string>(hh) + sepT;
      result += boost::lexical_cast<std::string>(mm) + sepT;
      result += boost::lexical_cast<std::string>(ss);
    }
  }

  return result;
}

