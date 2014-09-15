/*  Copyright (C) 2008-2011 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/edit/RepositoryManager.h

  \brief This is a singleton for managing edit repositories.
*/

#ifndef __TERRALIB_EDIT_INTERNAL_REPOSITORYMANAGER_H
#define __TERRALIB_EDIT_INTERNAL_REPOSITORYMANAGER_H

// TerraLib
#include "../common/Singleton.h"
#include "Config.h"

// STL
#include <map>
#include <vector>

namespace te
{
// Forward declarations
  namespace da
  {
    class ObjectId;
  }

  namespace gm
  {
    class Envelope;
    class Geometry;
  }

  namespace edit
  {
// Forward declaration
    class IdGeometry;
    class Repository;

    /*!
      \class RepositoryManager

      \brief This is a singleton for managing edit repositories.
    */
    class TEEDITEXPORT RepositoryManager : public te::common::Singleton<RepositoryManager>
    {
      friend class te::common::Singleton<RepositoryManager>;

      public:

        void addNewGeometry(const std::string& source, te::gm::Geometry* geom);

        void addEditedGeometry(const std::string& source, te::da::ObjectId* id, te::gm::Geometry* geom);

        const std::map<std::string, Repository*>& getRepositories() const;

        Repository* getRepository(const std::string& source) const;

        std::vector<IdGeometry*> getGeometries(const std::string& source, const te::gm::Envelope& e, int srid) const;

        IdGeometry* getGeometry(const std::string& source, const te::gm::Envelope& e, int srid) const;

        void clearAll();

        void clearEditedGeometries();

      protected:

        /*! \brief It initializes the singleton instance of the geometry manager. */
        RepositoryManager();

        /*! \brief Singleton destructor. */
        ~RepositoryManager();

      private:

        std::map<std::string, Repository*> m_repositories;
    };

  } // end namespace edit
}   // end namespace te

#endif  // __TERRALIB_EDIT_INTERNAL_REPOSITORYMANAGER_H
