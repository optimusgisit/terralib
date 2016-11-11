/*
 * File: DataAccess.i
 *
 * Data Access especific to Lua
 */

%{
te::da::DataSource* GetDs(const std::string& dsId, const bool opened = true)
{
  te::da::DataSourcePtr ds = te::da::GetDataSource(dsId, opened);
  
  return ds.get();
}
%}

te::da::DataSource* GetDs(const std::string& dsId, bool opened);

%extend te::da::DataSourceManager {
  void insert(te::da::DataSource* source)
  {
    te::da::DataSourcePtr src(source);
    $self->insert(src);
  }
  
  void open(const te::da::DataSourceInfo& info)
  {
    $self->open(info.getId(), info.getType(), info.getConnInfo());
  }
  
  te::da::DataSource* find(const std::string& id)
  {
	return ($self->find(id)).get();
  }  
};

%extend te::da::DataSourceInfoManager {
  bool add(te::da::DataSourceInfo source)
  {
	te::da::DataSourceInfoPtr ds;  
    ds.reset(new te::da::DataSourceInfo);
	*ds = source;
	bool res = $self->add(ds);
	
	ds.reset();	

	return res;
  }
  
  te::da::DataSourceInfo getDsInfo(const std::string& id)
  {
	te::da::DataSourceInfoPtr ds;  
    ds = $self->get(id);
	
	if (ds.get() == 0)
		return te::da::DataSourceInfo();	
	
    return *ds.get();
  }  
  
  te::da::DataSourceInfo getDsInfoByConnInfo(const std::string& connInfoAsString)
  {
	te::da::DataSourceInfoPtr ds;  
    ds = $self->getByConnInfo(connInfoAsString);
	
	if (ds.get() == 0)
		return te::da::DataSourceInfo();
	
    return *ds.get();	  
  }
  
  te::da::DataSourceInfo getDsInfoByConnInfo(const std::string& connInfoAsString)
  {
	te::da::DataSourceInfoPtr ds;  
    ds = $self->getByConnInfo(connInfoAsString);
	
	if (ds.get() == 0)
		return te::da::DataSourceInfo();
	
    return *ds.get();	  
  }  
  
  void show()
  {
    te::da::DataSourceInfoManager::iterator it;
    te::da::DataSourceInfoManager::iterator beg = te::da::DataSourceInfoManager::getInstance().begin();
    te::da::DataSourceInfoManager::iterator end = te::da::DataSourceInfoManager::getInstance().end();

    for(it = beg; it != end; ++it)
	{
      std::cout << "------DataSource Info------" << std::endl;
      std::cout << "id:\t" << it->second.get()->getId() << std::endl;   
      std::cout << "type:\t" << it->second.get()->getType() << std::endl;      
      std::cout << "access driver:\t" << it->second.get()->getAccessDriver() << std::endl;      
      std::cout << "title:\t" << it->second.get()->getTitle() << std::endl;         
      std::cout << "description:\t" << it->second.get()->getDescription() << std::endl;      
      std::cout << "conn info:\t" << it->second.get()->getConnInfoAsString() << std::endl;      
      std::cout << "\n\n" << std::endl;      
	}
  }
};

%extend te::da::DataSource {
  static std::string Exists(te::da::DataSource* ds)
  {
    try
	{
	  if (ds->exists(ds->getType(), ds->getConnectionInfo().uri()))
		  return "";
	}
	catch (const std::exception& e)
	{
	  return e.what();
	}
	
	return "";
  }
  
  void createDataSet(DataSetType* dst)
  {
	std::map<std::string, std::string> options;
	$self->createDataSet(dst, options);
  }
  
  void add(const std::string& dstName, DataSet* dse)
  {
	std::map<std::string, std::string> options;
	$self->add(dstName, dse, options);
  }
	
  void updateNullValues(const std::string& dseName, const std::string& propName, const std::string& data)
  {
    std::auto_ptr<te::da::DataSet>	dse = $self->getDataSet(dseName);
	std::auto_ptr<te::da::DataSetType> dst = $self->getDataSetType(dseName);
	
	int propPos = dst->getPropertyPosition(propName);
	te::da::PrimaryKey* pk = dst->getPrimaryKey();
    size_t numPks = pk->getProperties().size();
    std::vector<std::size_t> ids;

    for (size_t i = 0; i < numPks; ++i)
    {
      std::string pkName = pk->getProperties()[i]->getName();
      ids.push_back(dst->getPropertyPosition(pkName));
    }	
	
	te::mem::DataSet* newDse = new te::mem::DataSet(dst.get());
	std::vector< std::set<int> > fields;
	
	int lin = 0;
	int numProps = dse->getNumProperties();
	
	while (dse->moveNext())
	{
      te::mem::DataSetItem* item = new te::mem::DataSetItem(newDse);
	  std::set<int> cols;
	  
	  for (int i = 0; i < numProps; i++)
	  {
	    if (dse->isNull(i) && (i == propPos))
	    {
          switch(dse->getPropertyDataType(i))
          {
            case te::dt::INT16_TYPE:
               item->setValue(i, new te::dt::Int16(atoi(data.c_str())));
            break;

            case te::dt::INT32_TYPE:
               item->setValue(i, new te::dt::Int32(atoi(data.c_str())));
            break;

            case te::dt::INT64_TYPE:
               item->setValue(i, new te::dt::Int64(atoi(data.c_str())));
            break;

            case te::dt::FLOAT_TYPE:
               item->setValue(i, new te::dt::Float(atof(data.c_str())));
            break;

            case te::dt::DOUBLE_TYPE:
               item->setValue(i, new te::dt::Double(atof(data.c_str())));
            break;

            case te::dt::NUMERIC_TYPE:
               item->setValue(i, new te::dt::Numeric(data));
            break;

            case te::dt::STRING_TYPE:
               item->setValue(i, new te::dt::String(data));
            break;

            default:
              break;
		  }
		  
		  cols.insert(propPos);
	    }
	    else
	    {
		  item->setValue(i, dse->getValue(i).release());
	    }
	  }
	
	  if (cols.size() > 0)
	  {
		newDse->add(item);
		fields.push_back(cols);
	  }
	
	  lin++;
	}
	
	if (fields.size() > 0) 
	{
		$self->update(dseName, newDse, fields, ids);
	}
  }
};

%extend te::da::PrimaryKey {
  int getNumProperties() 
  {
    return $self->getProperties().size();
  }
  
  std::string getPropertyName(int i)
  {
    return $self->getProperties()[i]->getName();
  }
};

%extend te::da::DataSet {
  te::dt::AbstractData* getValue(int i)
  {
	return $self->getValue(i).release();
  }
  
  te::gm::Geometry* getGeom(int i)
  {
    return $self->getGeometry(i).release();
  }
};

%extend te::da::DataSetType {
  void add(const std::string& name, bool isPk, int datatype, bool isRequired)
  { 
    te::dt::SimpleProperty* prop;
	
	switch(datatype)
    {
	  case te::dt::NUMERIC_TYPE:
		prop = new te::dt::NumericProperty(name, 10, 3, true);
		break;
	  case te::dt::STRING_TYPE:
	    prop = new te::dt::StringProperty(name, te::dt::VAR_STRING, 255, isRequired);
	    break;
	  case te::dt::DATETIME_TYPE:
	    prop = new te::dt::DateTimeProperty(name, te::dt::DATE, isRequired);
		break;
	  default:
	    prop = new te::dt::SimpleProperty(name, datatype, isRequired);
        break;
	}
	
	$self->add(prop);
	
    if (isPk)
    {
	  te::da::PrimaryKey* pk = new te::da::PrimaryKey($self->getName() + "_pk_" + name, $self);
	  pk->add(prop);
	}
  }
  
  void add(const std::string& name, int srid, te::gm::GeomType subtype, bool isRequired)
  {
	te::gm::GeometryProperty* prop = new te::gm::GeometryProperty(name, srid, subtype, isRequired);
	$self->add(prop);
  }
};