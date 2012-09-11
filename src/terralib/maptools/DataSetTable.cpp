#include "DataSetTable.h"

//TerraLib include files
#include <terralib/dataaccess.h>

te::map::DataSetTable::DataSetTable(te::da::DataSet* dset) :
te::map::AbstractTable(),
m_dataSet(dset)
{
	resetTable();
}

te::map::DataSetTable::~DataSetTable()
{
}

void te::map::DataSetTable::resetTable()
{
  te::map::AbstractTable::resetTable();

  for(size_t i=0; i<m_dataSet->getType()->size(); i++)
  {
    m_presentOrder.push_back(i);
    m_colVisiblity.push_back(true);
  }

  m_currentRow = 0;

  m_dataSet->moveFirst();
}

size_t te::map::DataSetTable::numRows()
{
	return m_dataSet->size();
}

std::string te::map::DataSetTable::dataAsString(size_t row, size_t column)
{
  if(row < 0 || row >= numRows())
    throw te::common::Exception("row index out of boundaries.");

  if(column < 0 || column >= numColumns())
    throw te::common::Exception("row index out of boundaries.");

  size_t logCol = getLogicalColumn(column);

  size_t c = m_presentOrder[logCol];

  if(row != m_currentRow)
  {
    if(row == m_nextRow)
    {
      if(!m_dataSet->moveNext())
        throw te::common::Exception("Fail to move next on data set.");
    }
    else if(!m_dataSet->move(row))
      throw te::common::Exception("Fail to move dataSet.");

    m_currentRow = row;
    m_nextRow = m_currentRow+1;
  }

  return m_dataSet->getAsString((int) c);
}

std::string te::map::DataSetTable::getColumnName(size_t column) const
{
  te::da::DataSetType* type = m_dataSet->getType();
  size_t c = getLogicalColumn(column);

  std::string pName = type->getProperty(m_presentOrder[c])->getName();

  return pName;
}

std::set<size_t> te::map::DataSetTable::findGeoColsPositions() const
{
  std::set<size_t> cols;

  te::da::DataSetType* type = m_dataSet->getType();

  if(type->hasGeom())
  {
    std::vector<te::dt::Property*> props = type->getProperties();

    for(size_t i=0; i<props.size(); i++)
    {
      te::dt::Property* p = props[i];

      if(p->getType() == te::dt::GEOMETRY_TYPE)
        cols.insert(i);
    }
  }

  return cols;
}