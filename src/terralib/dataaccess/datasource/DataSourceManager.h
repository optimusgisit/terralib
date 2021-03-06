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
  \file terralib/dataaccess/datasource/DataSourceManager.h

  \brief This is a singleton for managing all data source instances available in the system.
*/

#ifndef __TERRALIB_DATAACCESS_INTERNAL_DATASOURCEMANAGER_H
#define __TERRALIB_DATAACCESS_INTERNAL_DATASOURCEMANAGER_H

// TerraLib
#include "../../common/Comparators.h"
#include "../../common/Singleton.h"
#include "../../common/ThreadingPolicies.h"
#include "DataSource.h"

// STL
#include <map>
#include <string>

namespace te
{
  namespace core { class URI; }
  namespace da
  {
    /*!
      \class DataSourceManager

      \brief This is a singleton for managing all data source instances available in the system.

      Prefer using the methods from this singleton instead of using the data source factory
      because it keeps track of the data sources available in the system.
      
      Another reason to use it in your application is that the application can choose any
      strategy to label data sources, for examples, using a
      descriptive title provided by the user or using an unique-universal-identifier (uid) generated by an algorithm. 

      \ingroup dataaccess
      
      \sa DataSource, DataSourceFactory, te::common::Singleton, ConnectionPoolManager

      \warning Developers: take care when adding new methods to this class as it uses synchronization primitives!
    */
    class TEDATAACCESSEXPORT DataSourceManager : public te::common::ObjectLevelLockable<DataSourceManager,
                                                                                        ::boost::recursive_mutex,
                                                                                        ::boost::lock_guard< ::boost::recursive_mutex>,
                                                                                        ::boost::lock_guard< ::boost::recursive_mutex> >,
                                                 public te::common::Singleton<DataSourceManager>
    {
      friend class te::common::Singleton<DataSourceManager>;

      public:

        typedef std::map<std::string, DataSourcePtr>::const_iterator const_iterator;
        typedef std::map<std::string, DataSourcePtr>::iterator iterator;

        /*!
        \brief It creates a new data source, stores a reference to it in the manager and then returns a pointer to it.

        \param id       The identification to be assigned to the data source.
        \param dsType   The data source type name (example: PostGIS, Oracle, WFS).
        \param connInfo The URI, as a valid string, used to set up the underlying access channel to the repository.

        \return A pointer to the new data source.

        \exception Exception It throws an exception if a data source with the same identification already exist.
        
        \note Thread-safe!
        */
        DataSourcePtr make(const std::string& id, const std::string& dsType, const std::string& connInfo);

        /*!
        \brief It creates a new data source, stores a reference to it in the manager and then returns a pointer to it.

        \param id       The identification to be assigned to the data source.
        \param dsType   The data source type name (example: PostGIS, Oracle, WFS).
        \param connInfo The URI used to set up the underlying access channel to the repository.

        \return A pointer to the new data source.

        \exception Exception It throws an exception if a data source with the same identification already exist.

        \note Thread-safe!
        */
        DataSourcePtr make(const std::string& id, const std::string& dsType, const te::core::URI& connInfo);

        /*!
          \brief It opens the data source, makes it ready for use, stores a reference to it in the manager and returns a pointer to it.

          \param id       The identification to be assigned to the data source.
          \param dsType   The data source type name (example: PostGIS, Oracle, WFS).
          \param connInfo The URI, as a valid string, used to set up the underlying access channel to the repository.

          \return A pointer to the new opened data source.

          \exception Exception It throws an exception if the data source can not be opened or if a data source with the same identification already exist.

          \note This method doesn't load the data source catalog.

          \note Thread-safe!
        */
        DataSourcePtr open(const std::string& id, const std::string& dsType, const std::string& connInfo);

        /*!
        \brief It opens the data source, makes it ready for use, stores a reference to it in the manager and returns a pointer to it.

        \param id       The identification to be assigned to the data source.
        \param dsType   The data source type name (example: PostGIS, Oracle, WFS).
        \param connInfo The URI used to set up the underlying access channel to the repository.

        \return A pointer to the new opened data source.

        \exception Exception It throws an exception if the data source can not be opened or if a data source with the same identification already exist.

        \note This method doesn't load the data source catalog.

        \note Thread-safe!
        */
        DataSourcePtr open(const std::string& id, const std::string& dsType, const te::core::URI& connInfo);

        /*!
          \brief It searches for an opened data source with the given id or it opens a new one if it doesn't exists

          \param id       The data source identification.
          \param dsType   The data source type name (example: PostGIS, Oracle, WFS).
          \param connInfo The set of parameters, as a valid URI string, used to set up the underlying access channel to the repository.

          \return A pointer to the new opened data source.

          \exception Exception It throws an exception if the data source can not be opened.

          \note This method doesn't load the data source catalog.

          \note Thread-safe!
        */
        DataSourcePtr get(const std::string& id, const std::string& dsType, const std::string& connInfo);

        /*!
        \brief It searches for an opened data source with the given id or it opens a new one if it doesn't exists

        \param id       The data source identification.
        \param dsType   The data source type name (example: PostGIS, Oracle, WFS).
        \param connInfo The set of parameters used to set up the underlying access channel to the repository.

        \return A pointer to the new opened data source.

        \exception Exception It throws an exception if the data source can not be opened.

        \note This method doesn't load the data source catalog.

        \note Thread-safe!
        */
        DataSourcePtr get(const std::string& id, const std::string& dsType, const te::core::URI& connInfo);

        /*!
          \brief It returns the number of data sources that the manager are keeping track of.
 
          \return The number of tracked data sources.

          \note Thread-safe: but take care when relying on this value in a multi-threaded environment!
        */
        std::size_t size() const;

        /*!
          \brief It returns the data source identified by the given id.

          \param id The data source identification.

          \return The data source with the given id, or NULL if none is found.

          \note Thread-safe!
        */
        DataSourcePtr find(const std::string& id) const;

        /*!
          \brief It stores the data source in the manager.

          The data source must have an identification in order to be inserted.

          \param ds The data source to be stored in the manager.

          \note The manager will take the ownership of the data source.

          \exception Exception It throws an exception if a data source with the same identification already exist or if the data source id is empty.

          \note Thread-safe!
        */
        void insert(const DataSourcePtr& ds);

        /*!
          \brief It changes the ownership of the data source to the caller.

          The memory used by the given data source will NOT BE released.
          In other words, you will take the ownership of the data source pointer.

          \param ds The data source to be detached.

          \note Thread-safe!
        */
        void detach(const DataSourcePtr& ds);

        /*!
          \brief It changes the ownership of the data source with the given identifier to the caller.

          \param id The data source identifier.

          \return The data source identified by id. The caller takes the data source ownership.

          \note Thread-safe!
        */
        DataSourcePtr detach(const std::string& id);

        /*!
          \brief All data sources of the specified type are detached from the manager.

          \note Thread-safe!
        */
        void detachAll(const std::string& dsType);

        /*!
          \brief All data sources are detached from the manager.

          \note Thread-safe!
        */
        void detachAll();

        /*!
          \brief It returns an iterator to the beginning of the conteiner.

          \return An iterator to the beginning of the conteiner.
        */
        const_iterator begin() const;

        /*!
          \brief It returns an iterator to the beginning of the conteiner.

          \return An iterator to the beginning of the conteiner.
        */
        iterator begin();

        /*!
          \brief It returns an iterator to the end of the conteiner.

          \return An iterator to the beginning of the conteiner.
        */
        const_iterator end() const;

        /*!
          \brief It returns an iterator to the end of the conteiner.

          \return An iterator to the beginning of the conteiner.
        */
        iterator end();

      protected:

        /*! \brief It initializes the singleton instance of the data source manager. */
        DataSourceManager();

        /*! \brief Singleton destructor. */
        ~DataSourceManager();

      private:
        
        std::map<std::string, DataSourcePtr> m_dss;   //!< The data sources kept in the manager.
    };

    inline std::size_t DataSourceManager::size() const
    {
      return m_dss.size();
    }

    inline DataSourceManager::const_iterator DataSourceManager::begin() const
    {
      return m_dss.begin();
    }

    inline DataSourceManager::iterator DataSourceManager::begin()
    {
      return m_dss.begin();
    }

    inline DataSourceManager::const_iterator DataSourceManager::end() const
    {
      return m_dss.end();
    }

    inline DataSourceManager::iterator DataSourceManager::end()
    {
      return m_dss.end();
    }

  } // end namespace da
}   // end namespace te


#endif  // __TERRALIB_DATAACCESS_INTERNAL_DATASOURCEMANAGER_H

