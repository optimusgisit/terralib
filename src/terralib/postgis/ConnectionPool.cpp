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
  \file terralib/postgis/ConnectionPool.cpp

  \brief A class that implements a connection pool for PostGIS.  
*/

// TerraLib
#include "../common/Enums.h"
#include "../common/STLUtils.h"
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/uri/Utils.h"
#include "../dataaccess/datasource/ConnectionPoolManager.h"
#include "Connection.h"
#include "ConnectionPool.h"
#include "DataSource.h"
#include "Exception.h"
#include "Utils.h"

// STL
#include <list>

// BOOST
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

// libpq
#include <libpq-fe.h>

class te::pgis::ConnectionPool::ConnectionPoolImpl
{
  public:

    ConnectionPoolImpl(te::pgis::DataSource* ds)
      : m_ds(ds),
        m_initialPoolSize(PGIS_DEFAULT_INITIAL_POOL_SIZE),
        m_minPoolSize(PGIS_DEFAULT_MIN_POOL_SIZE),
        m_maxPoolSize(PGIS_DEFAULT_MAX_POOL_SIZE),
        m_poolSize(0),
        m_maxIdleTime(PGIS_DEFAULT_MAX_IDLE_TIME),
        m_initialized(false)
    {
    }

    ~ConnectionPoolImpl()
    {
      te::common::FreeContents(m_connections);
    }

    std::string m_conninfo;                            //!< The connection info.
    std::string m_cencoding;                           //!< Client encoding.
    te::pgis::DataSource* m_ds;                        //!< The data source using the pool.
    std::size_t m_initialPoolSize;                     //!< It indicates the initial number of connections opened by the pool.
    std::size_t m_minPoolSize;                         //!< It indicates the minimum number of connections in the pool.
    std::size_t m_maxPoolSize;                         //!< It indicates the maximum number of connections in the pool.
    std::size_t m_poolSize;                            //!< It indicates the maximum number of connections in the pool.
    unsigned int m_maxIdleTime;                        //!< The number of seconds waiting for releasing connections.
    std::list<te::pgis::Connection*> m_connections;    //!< The list of available connections ready to be used.
    boost::mutex m_mtx;                                //!< A mutex for preventing reading/writing in the connection list.
    bool m_initialized;                                //!< A flag that indicates if the pool was initialized or not.
};

void te::pgis::ConnectionPool::initialize()
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

  if(isInitialized())
    throw Exception(TE_TR("The connection pool is already initialized!"));

// check for pool parameters...
  const te::core::URI& connInfo = m_pImpl->m_ds->getConnectionInfo();
  std::map<std::string, std::string> kvp = te::core::Expand(connInfo.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

  it = kvp.find("PG_INITIAL_POOL_SIZE");
  m_pImpl->m_initialPoolSize = (it != itend && !it->second.empty()) ? atoi(it->second.c_str()) : PGIS_DEFAULT_INITIAL_POOL_SIZE;
  
  it = kvp.find("PG_MIN_POOL_SIZE");
  m_pImpl->m_minPoolSize = (it != itend && !it->second.empty()) ? atoi(it->second.c_str()) : PGIS_DEFAULT_MIN_POOL_SIZE;
  
  it = kvp.find("PG_MAX_POOL_SIZE");
  m_pImpl->m_maxPoolSize = (it != itend && !it->second.empty()) ? atoi(it->second.c_str()) : PGIS_DEFAULT_MAX_POOL_SIZE;

// assure valid values for pool parameters
  if(m_pImpl->m_minPoolSize > m_pImpl->m_maxPoolSize)
    m_pImpl->m_minPoolSize = m_pImpl->m_maxPoolSize;

  if(m_pImpl->m_initialPoolSize > m_pImpl->m_maxPoolSize)
    m_pImpl->m_initialPoolSize = m_pImpl->m_maxPoolSize;
  else if(m_pImpl->m_initialPoolSize < m_pImpl->m_minPoolSize)
    m_pImpl->m_initialPoolSize = m_pImpl->m_minPoolSize;

  it = kvp.find("PG_MAX_IDLE_TIME");
  m_pImpl->m_maxIdleTime = (it != itend && !it->second.empty()) ? atoi(it->second.c_str()) : PGIS_DEFAULT_MAX_IDLE_TIME;

  m_pImpl->m_cencoding = te::core::CharEncoding::getEncodingName(m_pImpl->m_ds->getEncoding());

// make connection info
  m_pImpl->m_conninfo = MakeConnectionStr(connInfo);

// try to open the connections
  for(std::size_t i = 0; i < m_pImpl->m_initialPoolSize; ++i)
  {
    Connection* conn = new Connection(this, m_pImpl->m_conninfo, m_pImpl->m_cencoding, false);
    m_pImpl->m_connections.push_back(conn);
    ++(m_pImpl->m_poolSize);
  }

// verify the internal date time storage
  if(!m_pImpl->m_connections.empty())
  {
    std::string off = "off";
      
    std::string answer = PQparameterStatus(m_pImpl->m_connections.front()->m_pgconn, "integer_datetimes");

    if(answer == off)
      m_pImpl->m_ds->setTimeAsInteger(false);
    else
      m_pImpl->m_ds->setTimeAsInteger(true);
  }

  m_pImpl->m_initialized = true;
}

void te::pgis::ConnectionPool::finalize()
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

// if there is a connection in use it will raise an exception!
  std::list<te::pgis::Connection*>::iterator it = m_pImpl->m_connections.begin();
  std::list<te::pgis::Connection*>::iterator itend = m_pImpl->m_connections.end();

  while(it != itend)
  {
    if((*it)->m_inuse)
      throw Exception(TE_TR("There are opened connections. Please, close all connections before finalizing the connection pool."));

    ++it;
  }

// release all connections
  te::common::FreeContents(m_pImpl->m_connections);

  m_pImpl->m_connections.clear();
  m_pImpl->m_poolSize = 0;

  m_pImpl->m_initialized = false;
}

void te::pgis::ConnectionPool::idle()
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

// must we check the pool?
// no: we don't care about monitoring
// no: we weren't initialized
// no: if the pool has a fixed size we don't need to care about it!
// no: we have just the minimum pool connections
  if((m_pImpl->m_maxIdleTime == 0) ||
    !isInitialized() ||
    (m_pImpl->m_maxPoolSize == m_pImpl->m_minPoolSize) ||
    (m_pImpl->m_poolSize == m_pImpl->m_minPoolSize))
    return;

// what time is it?
  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

// release all connections not in use whose time has expired
  std::list<te::pgis::Connection*>::iterator it = m_pImpl->m_connections.begin();
  std::list<te::pgis::Connection*>::iterator itend = m_pImpl->m_connections.end();

  while(it != itend)
  {
    if((*it)->m_inuse == false)
    {
      boost::posix_time::time_duration d = now - (*it)->m_lastuse;

      if(d.seconds() > static_cast<long>(m_pImpl->m_maxIdleTime))
      {
        std::list<te::pgis::Connection*>::iterator itaux = it;
        ++it; // advance current iterator... we will release the connection pointed by the auxiliar iterator
        delete *itaux;
        m_pImpl->m_connections.erase(itaux);
        --(m_pImpl->m_poolSize);

        if(m_pImpl->m_poolSize == m_pImpl->m_minPoolSize) // have we reached the minimum pool size?
          break;

        continue; // we have already go on... (++it)! 
      }
    }

    ++it;
  }
}

bool te::pgis::ConnectionPool::isValid() const
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

  std::list<te::pgis::Connection*>::iterator it = m_pImpl->m_connections.begin();
  std::list<te::pgis::Connection*>::iterator itend = m_pImpl->m_connections.end();

  while(it != itend)
  {
    ConnStatusType status = PQstatus((*it)->m_pgconn);

    if(status != CONNECTION_OK)
      return false;

    ++it;
  }

  return true;
}

bool te::pgis::ConnectionPool::isInitialized() const
{
  return m_pImpl->m_initialized;
}

te::pgis::DataSource* te::pgis::ConnectionPool::getDataSource() const
{
  return m_pImpl->m_ds;
}

std::size_t te::pgis::ConnectionPool::getPoolSize() const
{
  return m_pImpl->m_poolSize;
}

std::size_t te::pgis::ConnectionPool::getInitialPoolSize() const
{
  return m_pImpl->m_initialPoolSize;
}

void te::pgis::ConnectionPool::setInitialPoolSize(std::size_t size)
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

  m_pImpl->m_initialPoolSize = size;
}

std::size_t te::pgis::ConnectionPool::getMinPoolSize() const
{
  return m_pImpl->m_minPoolSize;
}

void te::pgis::ConnectionPool::setMinPoolSize(std::size_t size)
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

  m_pImpl->m_minPoolSize = size;
}

std::size_t te::pgis::ConnectionPool::getMaxPoolSize() const
{
  return m_pImpl->m_maxPoolSize;
}

void te::pgis::ConnectionPool::setMaxPoolSize(std::size_t size)
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

  m_pImpl->m_maxPoolSize = size;
}

te::pgis::Connection* te::pgis::ConnectionPool::getConnection()
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

// search for available connections: we are optmistic - just try to open a new connection if none exist!
  std::list<te::pgis::Connection*>::iterator it = m_pImpl->m_connections.begin();
  std::list<te::pgis::Connection*>::iterator itend = m_pImpl->m_connections.end();

  while(it != itend)
  {
    if((*it)->m_inuse == false)
    {
      (*it)->m_inuse = true;
      return (*it);
    }

    ++it;
  }

// if we couldn't find an opened connection maybe we may create one and return it
  if(m_pImpl->m_poolSize < m_pImpl->m_maxPoolSize)
  {
    Connection* newConn = new Connection(this, m_pImpl->m_conninfo.c_str(), m_pImpl->m_cencoding.c_str(), true);
    m_pImpl->m_connections.push_back(newConn);
    ++(m_pImpl->m_poolSize);

    return newConn;
  }

  throw Exception(TE_TR("The connection pool has reached its maximum size!"), te::common::NO_CONNECTION_AVAILABLE);
}

void te::pgis::ConnectionPool::release(Connection* conn)
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

  conn->m_inuse = false;
  conn->m_lastuse = boost::posix_time::second_clock::local_time();
}

unsigned int te::pgis::ConnectionPool::getMaxIdleTime() const
{
  return m_pImpl->m_maxIdleTime;
}

void te::pgis::ConnectionPool::setMaxIdleTime(unsigned int t)
{
// acquire lock
  boost::lock_guard<boost::mutex> lock(m_pImpl->m_mtx);

  m_pImpl->m_maxIdleTime = t;
}

te::pgis::ConnectionPool::ConnectionPool(DataSource* ds)
  : m_pImpl(0)
{
  m_pImpl = new ConnectionPoolImpl(ds);

  te::da::ConnectionPoolManager::getInstance().add(this);
}

te::pgis::ConnectionPool::~ConnectionPool()
{
  te::da::ConnectionPoolManager::getInstance().remove(this);

  delete m_pImpl;
}
