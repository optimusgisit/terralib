/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled
   applications.

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
  \file GeometricOpQuery.h

  \brief Geometric Operation Vector Processing functions.
*/

// Terralib

#include "../BuildConfig.h"
#include "../common/progress/TaskProgress.h"
#include "../core/logger/Logger.h"
#include "../core/translator/Translator.h"

#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/dataset/DataSetAdapter.h"

#include "../dataaccess/query/DataSetName.h"
#include "../dataaccess/query/Expression.h"
#include "../dataaccess/query/Field.h"
#include "../dataaccess/query/Fields.h"
#include "../dataaccess/query/From.h"
#include "../dataaccess/query/FromItem.h"
#include "../dataaccess/query/GroupBy.h"
#include "../dataaccess/query/GroupByItem.h"
#include "../dataaccess/query/LiteralInt32.h"
#include "../dataaccess/query/PropertyName.h"
#include "../dataaccess/query/Select.h"
#include "../dataaccess/query/ST_Area.h"
#include "../dataaccess/query/ST_Centroid.h"
#include "../dataaccess/query/ST_ConvexHull.h"
#include "../dataaccess/query/ST_Envelope.h"
#include "../dataaccess/query/ST_Length.h"
#include "../dataaccess/query/ST_Perimeter.h"
#include "../dataaccess/query/ST_SetSRID.h"
#include "../dataaccess/query/ST_Union.h"
#include "../dataaccess/utils/Utils.h"

#include "../geometry/Geometry.h"
#include "../geometry/GeometryCollection.h"
#include "../geometry/GeometryProperty.h"

#include "../memory/DataSet.h"
#include "../memory/DataSetItem.h"

#include "GeometricOpQuery.h"
#include "Utils.h"

te::vp::GeometricOpQuery::GeometricOpQuery()
{
}

te::vp::GeometricOpQuery::~GeometricOpQuery()
{
}

bool te::vp::GeometricOpQuery::run() throw(te::common::Exception)
{
  // get the geometric operation and/or tabular operation.
  std::vector<int> opGeom;
  std::vector<int> opTab;
  std::vector<te::da::DataSetType*> dsTypeVec;

  for(std::size_t i = 0; i < m_operations.size(); ++i)
  {
    switch(m_operations[i])
    {
      case te::vp::CENTROID:
        opGeom.push_back(te::vp::CENTROID);
        break;
      case te::vp::CONVEX_HULL:
        opGeom.push_back(te::vp::CONVEX_HULL);
        break;
      case te::vp::MBR:
        opGeom.push_back(te::vp::MBR);
        break;
      case te::vp::AREA:
        opTab.push_back(te::vp::AREA);
        break;
      case te::vp::LINE:
        opTab.push_back(te::vp::LINE);
        break;
      case te::vp::PERIMETER:
        opTab.push_back(te::vp::PERIMETER);
        break;
      default:
      {
#ifdef TERRALIB_LOGGER_ENABLED
        TE_CORE_LOG_DEBUG("vp",
                          "Geometric Operation - The operation is not valid.");
#endif  // TERRALIB_LOGGER_ENABLED
      }
    }
  }

  bool hasMultiGeomColumns = false;
  bool result = false;

  switch(m_objStrategy)
  {
    case te::vp::ALL_OBJ:
    {
      if(hasMultiGeomColumns)  // Condição se o DataSource suporta mais de uma
                               // soluna geometrica...
      {
        dsTypeVec.push_back(
            te::vp::GeometricOp::GetDataSetType(te::vp::ALL_OBJ, true));
      }
      else
      {
        if(opGeom.size() > 0)
        {
          for(std::size_t i = 0; i < opGeom.size(); ++i)
            dsTypeVec.push_back(te::vp::GeometricOp::GetDataSetType(
                te::vp::ALL_OBJ, false, opGeom[i]));
        }
        else
        {
          dsTypeVec.push_back(
              te::vp::GeometricOp::GetDataSetType(te::vp::ALL_OBJ, false));
        }
      }

      for(std::size_t dsTypePos = 0; dsTypePos < dsTypeVec.size(); ++dsTypePos)
      {
        m_outDsetNameVec.push_back(dsTypeVec[dsTypePos]->getName());

        std::unique_ptr<te::da::DataSetType> outDataSetType(
            dsTypeVec[dsTypePos]);
        std::unique_ptr<te::mem::DataSet> outDataSet(
            SetAllObjects(dsTypeVec[dsTypePos], opTab, opGeom));

        te::vp::Save(m_outDsrc.get(), outDataSet.get(), outDataSetType.get());
        result = true;

        if(!result)
          return result;
      }
    }
    break;
    case te::vp::AGGREG_OBJ:
    {
      if(hasMultiGeomColumns)  // Condição se o DataSource suporta mais de uma
                               // coluna geometrica...
      {
        dsTypeVec.push_back(
            te::vp::GeometricOp::GetDataSetType(te::vp::AGGREG_OBJ, true));
      }
      else
      {
        if(opGeom.size() > 0)
        {
          for(std::size_t i = 0; i < opGeom.size(); ++i)
            dsTypeVec.push_back(te::vp::GeometricOp::GetDataSetType(
                te::vp::AGGREG_OBJ, false, opGeom[i]));
        }
        else
        {
          dsTypeVec.push_back(
              te::vp::GeometricOp::GetDataSetType(te::vp::AGGREG_OBJ, false));
        }
      }

      for(std::size_t dsTypePos = 0; dsTypePos < dsTypeVec.size(); ++dsTypePos)
      {
        m_outDsetNameVec.push_back(dsTypeVec[dsTypePos]->getName());

        std::unique_ptr<te::da::DataSetType> outDataSetType(
            dsTypeVec[dsTypePos]);
        std::unique_ptr<te::mem::DataSet> outDataSet(
            SetAggregObj(dsTypeVec[dsTypePos], opTab, opGeom));
        try
        {
          te::vp::Save(m_outDsrc.get(), outDataSet.get(), outDataSetType.get());
          result = true;
        }
        catch(...)
        {
          result = false;
        }
        if(!result)
          return result;
      }
    }
    break;
    case te::vp::AGGREG_BY_ATTRIBUTE:
    {
      if(hasMultiGeomColumns)  // Condição se o DataSource suporta mais de uma
                               // coluna geometrica...
      {
        dsTypeVec.push_back(te::vp::GeometricOp::GetDataSetType(
            te::vp::AGGREG_BY_ATTRIBUTE, true));
      }
      else
      {
        if(opGeom.size() > 0)
        {
          for(std::size_t i = 0; i < opGeom.size(); ++i)
            dsTypeVec.push_back(te::vp::GeometricOp::GetDataSetType(
                te::vp::AGGREG_BY_ATTRIBUTE, false, opGeom[i]));
        }
        else
        {
          dsTypeVec.push_back(te::vp::GeometricOp::GetDataSetType(
              te::vp::AGGREG_BY_ATTRIBUTE, false));
        }
      }

      for(std::size_t dsTypePos = 0; dsTypePos < dsTypeVec.size(); ++dsTypePos)
      {
        m_outDsetNameVec.push_back(dsTypeVec[dsTypePos]->getName());

        std::unique_ptr<te::da::DataSetType> outDataSetType(
            dsTypeVec[dsTypePos]);
        std::unique_ptr<te::mem::DataSet> outDataSet(
            SetAggregByAttribute(dsTypeVec[dsTypePos], opTab, opGeom));
        try
        {
          te::vp::Save(m_outDsrc.get(), outDataSet.get(), outDataSetType.get());
          result = true;
        }
        catch(...)
        {
          result = false;
        }
        if(!result)
          return result;
      }
    }
    break;
    default:
    {
#ifdef TERRALIB_LOGGER_ENABLED
      TE_CORE_LOG_DEBUG("vp", "Geometric Operation - Strategy Not found!");
#endif  // TERRALIB_LOGGER_ENABLED
    }
      return false;
  }

  return result;
}

te::mem::DataSet* te::vp::GeometricOpQuery::SetAllObjects(
    te::da::DataSetType* dsType, std::vector<int> tabVec,
    std::vector<int> geoVec)
{
  std::unique_ptr<te::mem::DataSet> outDSet(new te::mem::DataSet(dsType));
  std::unique_ptr<te::da::DataSet> inDset = m_inDsrc->getDataSet(m_inDsetName);
  std::size_t geom_pos = te::da::GetFirstSpatialPropertyPos(inDset.get());

  te::da::Fields* fields = new te::da::Fields;

  if(m_selectedProps.size() > 0)
  {
    for(std::size_t prop_pos = 0; prop_pos < m_selectedProps.size(); ++prop_pos)
    {
      te::da::Field* f_prop = new te::da::Field(m_selectedProps[prop_pos]);
      fields->push_back(f_prop);
    }
  }

  std::string name = inDset->getPropertyName(geom_pos);

  if(tabVec.size() > 0)
  {
    for(std::size_t tabPos = 0; tabPos < tabVec.size(); ++tabPos)
    {
      te::da::Expression* ex;
      te::da::Field* f;

      switch(tabVec[tabPos])
      {
        case te::vp::AREA:
        {
          ex = new te::da::ST_Area(te::da::PropertyName(name));
          f = new te::da::Field(*ex, "area");
          fields->push_back(f);
        }
        break;
        case te::vp::LINE:
        {
          ex = new te::da::ST_Length(te::da::PropertyName(name));
          f = new te::da::Field(*ex, "line_length");
          fields->push_back(f);
        }
        break;
        case te::vp::PERIMETER:
        {
          ex = new te::da::ST_Perimeter(te::da::PropertyName(name));
          f = new te::da::Field(*ex, "perimeter");
          fields->push_back(f);
        }
        break;
        default:
        {
#ifdef TERRALIB_LOGGER_ENABLED
          TE_CORE_LOG_DEBUG(
              "vp", "Geometric Operation - Invalid field to add in query.");
#endif  // TERRALIB_LOGGER_ENABLED
        }
      }
    }
  }

  std::unique_ptr<te::da::DataSetType> dsTypeSource(
      m_inDsrc->getDataSetType(m_inDsetName));

  te::gm::GeometryProperty* geomPropertySource =
      te::da::GetFirstGeomProperty(dsTypeSource.get());
  te::gm::GeometryProperty* geomPropertyLayer =
      te::da::GetFirstGeomProperty(dsType);

  if(geoVec.size() > 0)
  {
    for(std::size_t geoPos = 0; geoPos < geoVec.size(); ++geoPos)
    {
      te::da::Expression* ex_operation;
      te::da::Expression* ex_geom;
      te::da::Field* f_geom;

      switch(geoVec[geoPos])
      {
        case te::vp::CONVEX_HULL:
        {
          std::size_t pos = dsType->getPropertyPosition("convex_hull");
          if(pos < outDSet->getNumProperties() && pos > 0)
          {
            if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
            {
              te::da::LiteralInt32* srid =
                  new te::da::LiteralInt32(geomPropertyLayer->getSRID());
              ex_geom =
                  new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
            }
            else
            {
              ex_geom = new te::da::PropertyName(name);
            }

            ex_operation = new te::da::ST_ConvexHull(*ex_geom);
            f_geom = new te::da::Field(*ex_operation, "convex_hull");
            fields->push_back(f_geom);
          }
        }
        break;
        case te::vp::CENTROID:
        {
          std::size_t pos = dsType->getPropertyPosition("centroid");
          if(pos < outDSet->getNumProperties() && pos > 0)
          {
            if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
            {
              te::da::LiteralInt32* srid =
                  new te::da::LiteralInt32(geomPropertyLayer->getSRID());
              ex_geom =
                  new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
            }
            else
            {
              ex_geom = new te::da::PropertyName(name);
            }

            ex_operation = new te::da::ST_Centroid(*ex_geom);
            f_geom = new te::da::Field(*ex_operation, "centroid");
            fields->push_back(f_geom);
          }
        }
        break;
        case te::vp::MBR:
        {
          std::size_t pos = dsType->getPropertyPosition("mbr");
          if(pos < outDSet->getNumProperties() && pos > 0)
          {
            if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
            {
              te::da::LiteralInt32* srid =
                  new te::da::LiteralInt32(geomPropertyLayer->getSRID());
              ex_geom =
                  new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
            }
            else
            {
              ex_geom = new te::da::PropertyName(name);
            }

            ex_operation = new te::da::ST_Envelope(*ex_geom);
            f_geom = new te::da::Field(*ex_operation, "mbr");
            fields->push_back(f_geom);
          }
        }
        break;
        default:
        {
#ifdef TERRALIB_LOGGER_ENABLED
          TE_CORE_LOG_DEBUG(
              "vp", "Geometric Operation - Invalid field to add in query.");
#endif  // TERRALIB_LOGGER_ENABLED
        }
      }
    }
  }
  else
  {
    te::da::Expression* ex_geom;

    if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
    {
      te::da::LiteralInt32* srid =
          new te::da::LiteralInt32(geomPropertyLayer->getSRID());
      ex_geom = new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
    }
    else
    {
      ex_geom = new te::da::PropertyName(name);
    }

    te::da::Field* f_prop = new te::da::Field(*ex_geom);
    fields->push_back(f_prop);
  }

  te::da::FromItem* fromItem =
      new te::da::DataSetName(m_converter->getResult()->getName());
  te::da::From* from = new te::da::From;
  from->push_back(fromItem);

  te::da::Select select(fields, from);

  std::unique_ptr<te::da::DataSet> dsQuery = m_inDsrc->query(select);

  if(dsQuery->isEmpty())
    return 0;

  SetOutputDSet(dsQuery.get(), outDSet.get());

  return outDSet.release();
}

te::mem::DataSet* te::vp::GeometricOpQuery::SetAggregObj(
    te::da::DataSetType* dsType, std::vector<int> tabVec,
    std::vector<int> geoVec)
{
  std::unique_ptr<te::mem::DataSet> outDSet(new te::mem::DataSet(dsType));

  std::unique_ptr<te::da::DataSet> inDset = m_inDsrc->getDataSet(m_inDsetName);
  std::size_t geom_pos = te::da::GetFirstSpatialPropertyPos(inDset.get());
  std::string name = inDset->getPropertyName(geom_pos);

  te::da::Fields* fields = new te::da::Fields;

  if(tabVec.size() > 0)
  {
    for(std::size_t tabPos = 0; tabPos < tabVec.size(); ++tabPos)
    {
      te::da::Expression* ex;
      te::da::Field* f;

      switch(tabVec[tabPos])
      {
        case te::vp::AREA:
        {
          te::da::Expression* e_union =
              new te::da::ST_Union(te::da::PropertyName(name));
          ex = new te::da::ST_Area(*e_union);
          f = new te::da::Field(*ex, "area");
          fields->push_back(f);
        }
        break;
        case te::vp::LINE:
        {
          te::da::Expression* e_union =
              new te::da::ST_Union(te::da::PropertyName(name));
          ex = new te::da::ST_Length(*e_union);
          f = new te::da::Field(*ex, "line_length");
          fields->push_back(f);
        }
        break;
        case te::vp::PERIMETER:
        {
          te::da::Expression* e_union =
              new te::da::ST_Union(te::da::PropertyName(name));
          ex = new te::da::ST_Perimeter(*e_union);
          f = new te::da::Field(*ex, "perimeter");
          fields->push_back(f);
        }
        break;
        default:
        {
#ifdef TERRALIB_LOGGER_ENABLED
          TE_CORE_LOG_DEBUG(
              "vp", "Geometric Operation - Invalid field to add in query.");
#endif  // TERRALIB_LOGGER_ENABLED
        }
      }
    }
  }

  std::unique_ptr<te::da::DataSetType> dsTypeSource(
      m_inDsrc->getDataSetType(m_inDsetName));

  te::gm::GeometryProperty* geomPropertySource =
      te::da::GetFirstGeomProperty(dsTypeSource.get());
  te::gm::GeometryProperty* geomPropertyLayer =
      te::da::GetFirstGeomProperty(dsType);

  if(geoVec.size() > 0)
  {
    for(std::size_t geoPos = 0; geoPos < geoVec.size(); ++geoPos)
    {
      te::da::Expression* ex_operation;
      te::da::Expression* ex_geom;
      te::da::Field* f_geom;

      switch(geoVec[geoPos])
      {
        case te::vp::CONVEX_HULL:
        {
          std::size_t pos = dsType->getPropertyPosition("convex_hull");
          if(pos < outDSet->getNumProperties() && pos > 0)
          {
            if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
            {
              te::da::LiteralInt32* srid =
                  new te::da::LiteralInt32(geomPropertyLayer->getSRID());
              ex_geom =
                  new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
            }
            else
            {
              ex_geom = new te::da::PropertyName(name);
            }

            te::da::Expression* e_union = new te::da::ST_Union(*ex_geom);
            ex_operation = new te::da::ST_ConvexHull(*e_union);
            f_geom = new te::da::Field(*ex_operation, "convex_hull");
            fields->push_back(f_geom);
          }
        }
        break;
        case te::vp::CENTROID:
        {
          std::size_t pos = dsType->getPropertyPosition("centroid");
          if(pos < outDSet->getNumProperties() && pos > 0)
          {
            if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
            {
              te::da::LiteralInt32* srid =
                  new te::da::LiteralInt32(geomPropertyLayer->getSRID());
              ex_geom =
                  new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
            }
            else
            {
              ex_geom = new te::da::PropertyName(name);
            }

            te::da::Expression* e_union = new te::da::ST_Union(*ex_geom);
            ex_operation = new te::da::ST_Centroid(*e_union);
            f_geom = new te::da::Field(*ex_operation, "centroid");
            fields->push_back(f_geom);
          }
        }
        break;
        case te::vp::MBR:
        {
          std::size_t pos = dsType->getPropertyPosition("mbr");
          if(pos < outDSet->getNumProperties() && pos > 0)
          {
            if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
            {
              te::da::LiteralInt32* srid =
                  new te::da::LiteralInt32(geomPropertyLayer->getSRID());
              ex_geom =
                  new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
            }
            else
            {
              ex_geom = new te::da::PropertyName(name);
            }

            te::da::Expression* e_union = new te::da::ST_Union(*ex_geom);
            ex_operation = new te::da::ST_Envelope(*e_union);
            f_geom = new te::da::Field(*ex_operation, "mbr");
            fields->push_back(f_geom);
          }
        }
        break;
        default:
        {
#ifdef TERRALIB_LOGGER_ENABLED
          TE_CORE_LOG_DEBUG(
              "vp", "Geometric Operation - Invalid field to add in query.");
#endif  // TERRALIB_LOGGER_ENABLED
        }
      }
    }
  }
  else
  {
    te::da::Expression* e_union;

    if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
    {
      te::da::LiteralInt32* srid =
          new te::da::LiteralInt32(geomPropertyLayer->getSRID());
      e_union = new te::da::ST_Union(
          new te::da::ST_SetSRID(new te::da::PropertyName(name), srid));
    }
    else
    {
      e_union = new te::da::ST_Union(te::da::PropertyName(name));
    }

    te::da::Field* f_prop = new te::da::Field(*e_union, name);
    fields->push_back(f_prop);
  }

  te::da::FromItem* fromItem =
      new te::da::DataSetName(m_converter->getResult()->getName());
  te::da::From* from = new te::da::From;
  from->push_back(fromItem);

  te::da::Select select(fields, from);

  std::unique_ptr<te::da::DataSet> dsQuery = m_inDsrc->query(select);

  if(dsQuery->isEmpty())
    return 0;

  SetOutputDSet(dsQuery.get(), outDSet.get());

  return outDSet.release();
}

te::mem::DataSet* te::vp::GeometricOpQuery::SetAggregByAttribute(
    te::da::DataSetType* dsType, std::vector<int> tabVec,
    std::vector<int> geoVec)
{
  std::unique_ptr<te::mem::DataSet> outDSet(new te::mem::DataSet(dsType));
  std::unique_ptr<te::da::DataSet> inDset = m_inDsrc->getDataSet(m_inDsetName);
  std::size_t geom_pos = te::da::GetFirstSpatialPropertyPos(inDset.get());
  std::string name = inDset->getPropertyName(geom_pos);

  te::da::Fields* fields = new te::da::Fields;

  te::da::Field* f_aggreg = new te::da::Field(m_attribute);
  fields->push_back(f_aggreg);

  if(tabVec.size() > 0)
  {
    for(std::size_t tabPos = 0; tabPos < tabVec.size(); ++tabPos)
    {
      te::da::Expression* ex;
      te::da::Field* f;

      switch(tabVec[tabPos])
      {
        case te::vp::AREA:
        {
          te::da::Expression* e_union =
              new te::da::ST_Union(te::da::PropertyName(name));
          ex = new te::da::ST_Area(*e_union);
          f = new te::da::Field(*ex, "area");
          fields->push_back(f);
        }
        break;
        case te::vp::LINE:
        {
          te::da::Expression* e_union =
              new te::da::ST_Union(te::da::PropertyName(name));
          ex = new te::da::ST_Length(*e_union);
          f = new te::da::Field(*ex, "line_length");
          fields->push_back(f);
        }
        break;
        case te::vp::PERIMETER:
        {
          te::da::Expression* e_union =
              new te::da::ST_Union(te::da::PropertyName(name));
          ex = new te::da::ST_Perimeter(*e_union);
          f = new te::da::Field(*ex, "perimeter");
          fields->push_back(f);
        }
        break;
        default:
        {
#ifdef TERRALIB_LOGGER_ENABLED
          TE_CORE_LOG_DEBUG(
              "vp", "Geometric Operation - Invalid field to add in query.");
#endif  // TERRALIB_LOGGER_ENABLED
        }
      }
    }
  }

  std::unique_ptr<te::da::DataSetType> dsTypeSource(
      m_inDsrc->getDataSetType(m_inDsetName));

  te::gm::GeometryProperty* geomPropertySource =
      te::da::GetFirstGeomProperty(dsTypeSource.get());
  te::gm::GeometryProperty* geomPropertyLayer =
      te::da::GetFirstGeomProperty(dsType);

  if(geoVec.size() > 0)
  {
    for(std::size_t geoPos = 0; geoPos < geoVec.size(); ++geoPos)
    {
      te::da::Expression* ex_operation;
      te::da::Expression* ex_geom;
      te::da::Field* f_geom;

      switch(geoVec[geoPos])
      {
        case te::vp::CONVEX_HULL:
        {
          std::size_t pos = dsType->getPropertyPosition("convex_hull");
          if(pos < outDSet->getNumProperties() && pos > 0)
          {
            if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
            {
              te::da::LiteralInt32* srid =
                  new te::da::LiteralInt32(geomPropertyLayer->getSRID());
              ex_geom =
                  new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
            }
            else
            {
              ex_geom = new te::da::PropertyName(name);
            }

            te::da::Expression* e_union = new te::da::ST_Union(*ex_geom);
            ex_operation = new te::da::ST_ConvexHull(*e_union);
            f_geom = new te::da::Field(*ex_operation, "convex_hull");
            fields->push_back(f_geom);
          }
        }
        break;
        case te::vp::CENTROID:
        {
          std::size_t pos = dsType->getPropertyPosition("centroid");
          if(pos < outDSet->getNumProperties() && pos > 0)
          {
            if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
            {
              te::da::LiteralInt32* srid =
                  new te::da::LiteralInt32(geomPropertyLayer->getSRID());
              ex_geom =
                  new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
            }
            else
            {
              ex_geom = new te::da::PropertyName(name);
            }

            te::da::Expression* e_union = new te::da::ST_Union(*ex_geom);
            ex_operation = new te::da::ST_Centroid(*e_union);
            f_geom = new te::da::Field(*ex_operation, "centroid");
            fields->push_back(f_geom);
          }
        }
        break;
        case te::vp::MBR:
        {
          std::size_t pos = dsType->getPropertyPosition("mbr");
          if(pos < outDSet->getNumProperties() && pos > 0)
          {
            if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
            {
              te::da::LiteralInt32* srid =
                  new te::da::LiteralInt32(geomPropertyLayer->getSRID());
              ex_geom =
                  new te::da::ST_SetSRID(new te::da::PropertyName(name), srid);
            }
            else
            {
              ex_geom = new te::da::PropertyName(name);
            }

            te::da::Expression* e_union = new te::da::ST_Union(*ex_geom);
            ex_operation = new te::da::ST_Envelope(*e_union);
            f_geom = new te::da::Field(*ex_operation, "mbr");
            fields->push_back(f_geom);
          }
        }
        break;
        default:
        {
#ifdef TERRALIB_LOGGER_ENABLED
          TE_CORE_LOG_DEBUG(
              "vp", "Geometric Operation - Invalid field to add in query.");
#endif  // TERRALIB_LOGGER_ENABLED
        }
      }
    }
  }
  else
  {
    te::da::Expression* e_union;

    if(geomPropertySource->getSRID() != geomPropertyLayer->getSRID())
    {
      te::da::LiteralInt32* srid =
          new te::da::LiteralInt32(geomPropertyLayer->getSRID());
      e_union = new te::da::ST_Union(
          new te::da::ST_SetSRID(new te::da::PropertyName(name), srid));
    }
    else
    {
      e_union = new te::da::ST_Union(te::da::PropertyName(name));
    }

    te::da::Field* f_prop = new te::da::Field(*e_union, name);
    fields->push_back(f_prop);
  }

  te::da::FromItem* fromItem =
      new te::da::DataSetName(m_converter->getResult()->getName());
  te::da::From* from = new te::da::From;
  from->push_back(fromItem);

  te::da::Select select(fields, from);

  te::da::GroupBy* groupBy = new te::da::GroupBy();
  te::da::GroupByItem* e_groupBy = new te::da::GroupByItem(m_attribute);
  groupBy->push_back(e_groupBy);

  select.setGroupBy(groupBy);

  std::unique_ptr<te::da::DataSet> dsQuery = m_inDsrc->query(select);

  if(dsQuery->isEmpty())
    return 0;

  SetOutputDSet(dsQuery.get(), outDSet.get());

  return outDSet.release();
}

void te::vp::GeometricOpQuery::SetOutputDSet(te::da::DataSet* inDataSet,
                                             te::mem::DataSet* outDataSet)
{
  std::size_t numProps = inDataSet->getNumProperties();
  int pk = 0;

  inDataSet->moveBeforeFirst();
  while(inDataSet->moveNext())
  {
    bool geomFlag = true;

    te::mem::DataSetItem* dItem = new te::mem::DataSetItem(outDataSet);

    dItem->setInt32(0, pk);

    for(std::size_t i = 0; i < numProps; ++i)
    {
      int type = inDataSet->getPropertyDataType(i);
      switch(type)
      {
        case te::dt::INT16_TYPE:
          dItem->setInt16(i + 1, inDataSet->getInt16(i));
          break;
        case te::dt::INT32_TYPE:
          dItem->setInt32(i + 1, inDataSet->getInt32(i));
          break;
        case te::dt::INT64_TYPE:
          dItem->setInt64(i + 1, inDataSet->getInt64(i));
          break;
        case te::dt::DOUBLE_TYPE:
          dItem->setDouble(i + 1, inDataSet->getDouble(i));
          break;
        case te::dt::STRING_TYPE:
          dItem->setString(i + 1, inDataSet->getString(i));
          break;
        case te::dt::GEOMETRY_TYPE:
        {
          std::string inPropName = inDataSet->getPropertyName(i);
          std::unique_ptr<te::gm::Geometry> geom(
              inDataSet->getGeometry(i).release());

          if(inPropName == "convex_hull")
          {
            if(geom->getGeomTypeId() == te::gm::PolygonType)
              dItem->setGeometry("convex_hull", geom.release());
            else
              geomFlag = false;
          }
          else if(inPropName == "centroid")
          {
            dItem->setGeometry("centroid", geom.release());
          }
          else if(inPropName == "mbr")
          {
            if(geom->getGeomTypeId() == te::gm::PolygonType)
              dItem->setGeometry("mbr", geom.release());
            else
              geomFlag = false;
          }
          else
          {
            te::gm::GeometryCollection* teGeomColl =
                new te::gm::GeometryCollection(
                    0, te::gm::GeometryCollectionType, geom->getSRID());

            switch(geom->getGeomTypeId())
            {
              case te::gm::PointType:
              case te::gm::PointMType:
              case te::gm::PointZType:
              case te::gm::PointZMType:
              case te::gm::LineStringType:
              case te::gm::LineStringMType:
              case te::gm::LineStringZType:
              case te::gm::LineStringZMType:
              case te::gm::PolygonType:
              case te::gm::PolygonMType:
              case te::gm::PolygonZType:
              case te::gm::PolygonZMType:
              {
                if(geom->isValid())
                  teGeomColl->add(geom.release());
              }
              break;
              default:
              {
#ifdef TERRALIB_LOGGER_ENABLED
                TE_CORE_LOG_DEBUG("vp",
                                  "Geometric Operation - Could not insert the "
                                  "geometry in collection.");
#endif  // TERRALIB_LOGGER_ENABLED
              }
            }

            if(teGeomColl->getNumGeometries() != 0)
              dItem->setGeometry(i + 1, teGeomColl);
            else
              dItem->setGeometry(i + 1, geom.release());
          }
        }
        break;
        default:
        {
#ifdef TERRALIB_LOGGER_ENABLED
          TE_CORE_LOG_DEBUG("vp",
                            "Geometric Operation - Property type not found.");
#endif  // TERRALIB_LOGGER_ENABLED
        }
      }
    }
    if(geomFlag)
    {
      ++pk;
      outDataSet->add(dItem);
    }
  }
}
