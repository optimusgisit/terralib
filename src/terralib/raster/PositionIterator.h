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
  \file terralib/rp/PositionIterator.h

  \brief It implements several ways to retrieve positions inside a band with spatial restriction,
         e.g. through a line, inside a bounding box or polygon, etc.
*/

#ifndef __TERRALIB_RASTER_INTERNAL_POSITIONITERATOR_H
#define __TERRALIB_RASTER_INTERNAL_POSITIONITERATOR_H

// Terralib
#include "../common/STLUtils.h"
#include "../common/MathUtils.h"
#include "../core/logger/Logger.h"
#include "../geometry.h"
#include "Band.h"
#include "BandProperty.h"
#include "BlockUtils.h"
#include "Grid.h"
#include "Exception.h"

// STL
#include <iostream>
#include <cmath>

namespace te
{
  namespace rst
  {
// Forward declaration.
    class Band;
    
    inline bool StdSortPointPointerComparison( te::gm::Point *p1, te::gm::Point *p2 )
    {
      return *p1 < *p2;
    }    

    /*!
      \class AbstractPositionIterator

      \brief This class is the base for implementing ways to navigate over the band with spatial restriction,
             e.g. through a line, inside a bounding box or polygon, etc.

      \ingroup rst
    */
    template<class T> class AbstractPositionIterator
    {
      public:

        /*! \brief Constructor. */
        AbstractPositionIterator();

        /*!
          \brief Constructor.

          \param b The band to iterate.
        */
        AbstractPositionIterator(const te::rst::Raster* r);

        /*!
          \brief Copy constructor.

          \param rhs The right-hand-side copy used to copy from.
        */
        AbstractPositionIterator(const AbstractPositionIterator& rhs);

        /*! \brief Destructor. */
        ~AbstractPositionIterator();

        /*! \brief Returns a vector of the values in current position (column, row) from iterator. */
        virtual const std::vector<T> operator*() const = 0;

        /*!
          \brief Returns the real value in current position (column, row, band) from iterator.

          \param i The band index.

          \return The pixel real value in current position.
          
          \note For complex values use operator()
        */
        virtual T operator[](const unsigned int i) const = 0;
        
        /*!
          \brief Returns the complex value in current position (column, row, band) from iterator.

          \param i The band index.

          \return The pixel comples value in current position.
          
          \note For real values use operator[]
        */
        virtual std::complex< T > operator()(const unsigned int i) const = 0;        

        /*! \brief Returns the current row in iterator. */
        virtual unsigned int getRow() const = 0;

        /*! \brief Returns the current column in iterator. */
        virtual unsigned int getColumn() const = 0;

        /*! \brief Advances to the next position. */
        virtual void operator++() = 0;

        /*! \brief Returns to the previous position. */
        virtual void operator--() = 0;

        /*!
          \brief Assignment operator.

          \param rhs The right-hand-side copy used to copy from.

          \return A reference to this object.
        */
        AbstractPositionIterator& operator=(const AbstractPositionIterator& rhs);

        /*!
          \brief Difference operator.

          \param rhs The right-hand side to compare.

          \return Returns true if the iterators are at different positions, or false otherwise.
        */
        virtual bool operator!=(const AbstractPositionIterator& rhs) const;

        /*!
          \brief Sets the iterator position to the end of the current band.

          \param b The band to retrieve the end information.
        */
        virtual void setEnd() = 0;

      protected:

        const te::rst::Raster* m_raster;              //!< The band from where to get the values.

    };

    /*!
      \class PolygonIterator

      \brief This class implements the strategy to iterate with spatial restriction,
             the iteration occurs inside a polygon.

      \ingroup rst
    */
    template<class T> class PolygonIterator: public AbstractPositionIterator<T>
    {
      public:

        PolygonIterator();

        /*!
          \brief Constructor.

          \param b The band to iterate.
          \param p The polygon from where the iteration will navigate.
          \note Both raster and polygon must have the same SRID.
        */
        PolygonIterator(const te::rst::Raster* r, const te::gm::Polygon* p);

        /*!
          \brief Copy constructor.

          \param rhs The right-hand-side copy used to copy from.
        */
        PolygonIterator(const PolygonIterator& rhs);

        ~PolygonIterator();

        void setNextLine(bool updatecurrline = true);

        const std::vector<T> operator*() const;

        T operator[](const unsigned int i) const;
        
        std::complex< T > operator()(const unsigned int i) const;

        unsigned int getRow() const;

        unsigned int getColumn() const;

        void operator++();

        void operator--();

        PolygonIterator& operator=(const PolygonIterator& rhs);

        void setEnd();

        /*! \brief Returns an iterator referring to the first value of the band.*/
        static PolygonIterator begin(const te::rst::Raster* r, const te::gm::Polygon* p);

        /*! \brief Returns an iterator referring to after the end of the iterator. */
        static PolygonIterator end(const te::rst::Raster* r, const te::gm::Polygon* p);

        bool operator!=(const PolygonIterator<T>& rhs) const;

      protected:

        const te::gm::Polygon* m_polygon;                  //!< The spatial restriction to be applied in the iterator.
        te::gm::Line* m_currline;                          //!< The current line in the iterator.
        int m_column;                                      //!< The current column of the iterator.
        int m_row;                                         //!< The current row of the iterator.
        int m_startingcolumn;                              //!< The starting column (in current line) to initialize the iteration.
        int m_endingcolumn;                                //!< The column (in current line) to finalize the iteration.
        int m_startingrow;                                 //!< The starting row of the iteration.
        int m_endingrow;                                   //!< The ending row of the iteration.
        int m_maxcolumns;                                  //!< The number of columns in band.
        int m_maxrows;                                     //!< The number of rows in band.
        int m_actualintersection;                          //!< The actual line of the iterator.
        int m_nintersections;                              //!< The number number of intersected lines in current line of the iterator.
        std::auto_ptr<te::rst::TileIndexer> m_tileIndexer; //!< Tile indexer used to optimize the geometric operations
        std::vector<std::pair<int, int> > m_columns; //!< Coordinates of the columns to be transversed
        
        // Variables used by the operator[] method.
        mutable double m_operatorBrackets_value;
        mutable std::complex< double > m_operatorParenthesis_value;
        

        /*! \brief Clear all internal allocated objects and reset back to the initial state. */        
        void clear();

    };

    /*!
      \class LineIterator

      \brief This class implements the strategy to iterate with spatial restriction,
             the iteration occurs inside a line.

      \ingroup rst
    */
    template<class T> class LineIterator: public AbstractPositionIterator<T>
    {
      public:

        LineIterator();

        /*!
          \brief Constructor.

          \param b The band to iterate.
          \param l The line from where the iteration will navigate.
        */
        LineIterator(const te::rst::Raster* r, const te::gm::Line* l);

        /*!
          \brief Copy constructor.

          \param rhs The right-hand-side copy used to copy from.
        */
        LineIterator(const LineIterator& rhs);

        ~LineIterator();

        const std::vector<T> operator*() const;

        T operator[](const unsigned int i) const;
        
        std::complex< T > operator()(const unsigned int i) const;

        unsigned int getRow() const;

        unsigned int getColumn() const;

        void operator++();

        void operator--();

        LineIterator& operator=(const LineIterator& rhs);

        void setEnd();

        /*! \brief Returns an iterator referring to the first value of the band.*/
        static LineIterator begin(const te::rst::Raster* r, const te::gm::Line* l);

        /*! \brief Returns an iterator referring to after the end of the iterator. */
        static LineIterator end(const te::rst::Raster* r, const te::gm::Line* l);

        bool operator!=(const LineIterator<T>& rhs) const;

      protected:

        const te::gm::Line* m_line;                   //!< The spatial restriction to be applied in the iterator.
        int m_currentpixelindex;                      //!< The index of the current pixel location.
        std::vector<te::gm::Point*> m_pixelsinline;   //!< A vector of pixel locations that intersects the line.
        mutable double m_operatorBrackets_value;                   //!< Used by the operator[] method.
        mutable std::complex< double > m_operatorParenthesis_value;     //!< Used by the operator() method.
        

    };

    /*!
      \class PointSetIterator

      \brief This class implements the strategy to iterate with spatial restriction,
             the iteration occurs inside a vector of points.

      \ingroup rst
    */
    template<class T> class PointSetIterator: public AbstractPositionIterator<T>
    {
      public:

        PointSetIterator();

        /*!
          \brief Constructor.

          \param b The band to iterate.
          \param p The vector of points where the iteration will navigate.
        */
        PointSetIterator(const te::rst::Raster* r, const std::vector<te::gm::Point*> p);

        /*!
          \brief Copy constructor.

          \param rhs The right-hand-side copy used to copy from.
        */
        PointSetIterator(const PointSetIterator& rhs);

        ~PointSetIterator();

        const std::vector<T> operator*() const;

        T operator[](const unsigned int i) const;
        
        std::complex< T > operator()(const unsigned int i) const;

        unsigned int getRow() const;

        unsigned int getColumn() const;

        void operator++();

        void operator--();

        PointSetIterator& operator=(const PointSetIterator& rhs);

        void setEnd();

        /*! \brief Returns an iterator referring to the first value of the band.*/
        static PointSetIterator begin(const te::rst::Raster* r, const std::vector<te::gm::Point*> p);

        /*! \brief Returns an iterator referring to after the end of the iterator. */
        static PointSetIterator end(const te::rst::Raster* r, const std::vector<te::gm::Point*> p);

        bool operator!=(const PointSetIterator<T>& rhs) const;

      protected:

        std::vector<te::gm::Point*> m_pixelsinpointset;    //!< The spatial restriction to be applied in the iterator.
        int m_currentpixelindex;                           //!< The index of the current pixel location.
        mutable double m_operatorBrackets_value;                   //!< Used by the operator[] method.
        mutable std::complex< double > m_operatorParenthesis_value;     //!< Used by the operator() method.
        

    };

// implementation of abstract position iterator
    template<class T> te::rst::AbstractPositionIterator<T>::AbstractPositionIterator()
      : m_raster(0)
    {
    }

    template<class T> te::rst::AbstractPositionIterator<T>::AbstractPositionIterator(const te::rst::Raster* r)
      : m_raster(r)
    {
    }

    template<class T> te::rst::AbstractPositionIterator<T>::AbstractPositionIterator(const AbstractPositionIterator& rhs)
      : m_raster(rhs.m_raster)
    {
    }

    template<class T> te::rst::AbstractPositionIterator<T>::~AbstractPositionIterator()
    {
    }

    template<class T> te::rst::AbstractPositionIterator<T>& te::rst::AbstractPositionIterator<T>::operator=(const AbstractPositionIterator& rhs)
    {
      if (this != &rhs)
        m_raster = rhs.m_raster;

      return *this;
    }

    template<class T> bool te::rst::AbstractPositionIterator<T>::operator!=(const te::rst::AbstractPositionIterator<T>& rhs) const
    {
      return (m_raster != rhs.m_raster);
    }

// implementation of iteration strategy bounded by a polygon
    template<class T> te::rst::PolygonIterator<T>::PolygonIterator()
      : AbstractPositionIterator<T>(),
        m_polygon(0),
        m_currline(0),
        m_column(-1),
        m_row(-1),
        m_startingcolumn(0),
        m_endingcolumn(0),
        m_startingrow(0),
        m_endingrow(0),
        m_maxcolumns(0),
        m_maxrows(0),
        m_actualintersection(-1),
        m_nintersections(0)
    {
    }

    template<class T> te::rst::PolygonIterator<T>::PolygonIterator(const te::rst::Raster* r, const te::gm::Polygon* p)
      : AbstractPositionIterator<T>(r),
        m_polygon(p),
        m_currline(0),
        m_column(-1),
        m_row(-1),
        m_startingcolumn(0),
        m_endingcolumn(0),
        m_startingrow(0),
        m_endingrow(0),
        m_maxcolumns(r->getNumberOfColumns()),
        m_maxrows(r->getNumberOfRows()),
        m_actualintersection(-1),
        m_nintersections(0)
    {
      if( r->getSRID() != p->getSRID() )
      {
        throw te::rst::Exception( TE_TR("Invalid polygon SRID") );
      }

      const te::rst::Grid& rasterGrid = *r->getGrid();
      const te::gm::Envelope& rasterEnvelope = *rasterGrid.getExtent();
      
      te::gm::Coord2D ll = m_polygon->getMBR()->getLowerLeft();
      te::gm::Coord2D ur = m_polygon->getMBR()->getUpperRight();

      // Is the polygon extent is outside the current raster area
      
      if(
          ( ll.x > rasterEnvelope.m_urx )
          ||
          ( ur.x < rasterEnvelope.m_llx )
          ||
          ( ur.y < rasterEnvelope.m_lly )
          ||
          ( ll.y > rasterEnvelope.m_ury )
        )
      {
        setEnd();
      }
      else
      {
        // defining starting/ending rows

        te::gm::Coord2D llIndexed;
        rasterGrid.geoToGrid(ll.x, ll.y, llIndexed.x, llIndexed.y);
        te::gm::Coord2D urIndexed;
        rasterGrid.geoToGrid(ur.x, ur.y, urIndexed.x, urIndexed.y);

        m_startingrow = (int)std::ceil( urIndexed.y );
        m_startingrow = std::max( 0, m_startingrow );
        m_startingrow = std::min( m_startingrow, ((int)rasterGrid.getNumberOfRows()) - 1 );

        m_endingrow = (int)std::floor( llIndexed.y );
        m_endingrow = std::max( 0, m_endingrow );
        m_endingrow = std::min( m_endingrow, ((int)rasterGrid.getNumberOfRows()) - 1 );

        if( m_endingrow < m_startingrow )
        {
          setEnd();
        }
        else
        {
          // initialize the TileIndexer

          m_tileIndexer = std::auto_ptr<te::rst::TileIndexer>(new te::rst::TileIndexer(*m_polygon, rasterGrid.getResolutionY()));

          // defining initial state

          m_currline = new te::gm::Line(te::gm::Point(0, 0, m_polygon->getSRID()),
            te::gm::Point(0, 0, m_polygon->getSRID()), te::gm::LineStringType,
            m_polygon->getSRID());

          m_row = m_startingrow;

          setNextLine(true);

          m_column = m_startingcolumn;
        }
      }
    }

    template<class T> te::rst::PolygonIterator<T>::PolygonIterator(const PolygonIterator<T>& rhs)
      : AbstractPositionIterator<T>(rhs),
        m_polygon(0),
        m_currline(0),
        m_column(-1),
        m_row(-1),
        m_startingcolumn(0),
        m_endingcolumn(0),
        m_startingrow(0),
        m_endingrow(0),
        m_maxcolumns(0),
        m_maxrows(0),
        m_actualintersection(-1),
        m_nintersections(0)
    {
      operator=( rhs );
    }

    template<class T> te::rst::PolygonIterator<T>::~PolygonIterator()
    {
      clear();
    }

    template<class T> void te::rst::PolygonIterator<T>::setNextLine(bool updatecurrline)
    {
      if (m_actualintersection == -1 || m_actualintersection >= m_nintersections)
      {
        // Globals

        const double& rasterEnvelopeLLX =
          AbstractPositionIterator< T >::m_raster->getGrid()->getExtent()->m_llx;
        const double& rasterEnvelopeURX =
          AbstractPositionIterator< T >::m_raster->getGrid()->getExtent()->m_urx;

        // Updates the line corresponding to the current line of the iterator

        if (updatecurrline)
        {
          double nexty = this->m_raster->getGrid()->gridToGeo(0, m_row).y;

          m_currline->setX(0, std::min(
            rasterEnvelopeLLX,
            m_polygon->getMBR()->getLowerLeft().x) );
          m_currline->setY(0, nexty);

          m_currline->setX(1, std::max(
            rasterEnvelopeURX,
            m_polygon->getMBR()->getUpperRight().x ) );
          m_currline->setY(1, nexty);
        }

        // Initialize Tile Indexer and structures used to retrieve the points where the current line 
        // intersects the polygon

        m_columns.clear();

        
        // Vector to store the points where the current line intersects the current tile of the polygon
        std::vector<te::gm::Point*> intersectionPoints;

        // Retrieve the tile corresponding to the current line and store the points where the line
        // intersects the tile
        
        te::rst::TileIndexer::TileSegIndex* currentTilePtr = 0;
        
        if (m_tileIndexer->getTile(m_currline->getY(0), &currentTilePtr)) 
        {
          assert(currentTilePtr);
          
          te::gm::LinearRing const* ringTile;
          std::auto_ptr<te::gm::Line> tileSeg;          
          std::auto_ptr< te::gm::Geometry > interResultPtr;
          std::vector< te::gm::Geometry * > singleGeomsPtrs;
          std::size_t singleGeomsPtrsIdx = 0;
          
          try
          {
            // in some cases the intersection presents an unhandled exception, in this case we do not paint the current line
            // Transverse the segments of the tile retrieving the intersection between them and the current line

            te::gm::LineString* lineStrPtr = 0;

            for (std::size_t i = 0; i < currentTilePtr->size(); i++) 
            {
              assert((*currentTilePtr)[i].first < m_polygon->getNumRings());

              // Retrieve the current ring of the current tile
              assert(dynamic_cast<te::gm::LinearRing const*>((*m_polygon)[(*currentTilePtr)[i].first]));
              ringTile = (te::gm::LinearRing const*)(*m_polygon)[(*currentTilePtr)[i].first];

              assert((*currentTilePtr)[i].second < m_polygon->getNPoints());

              // Retrieve the current segment of the current tile
              tileSeg.reset(new te::gm::Line(te::gm::Point(ringTile->getX((*currentTilePtr)[i].second),
                                                           ringTile->getY((*currentTilePtr)[i].second), 
                                                           ringTile->getSRID()),
                                             te::gm::Point(ringTile->getX((*currentTilePtr)[i].second + 1),
                                                           ringTile->getY((*currentTilePtr)[i].second + 1),
                                                           ringTile->getSRID()),
                                             te::gm::LineStringType, ringTile->getSRID()));

              // Computes the intersection point between the current segment and the current line
              
              interResultPtr.reset( tileSeg->intersection( m_currline ) );
              
              if( interResultPtr.get() != 0 ) 
              {
                singleGeomsPtrs.clear();
                te::gm::Multi2Single( interResultPtr.get(), singleGeomsPtrs );
                
                for( singleGeomsPtrsIdx = 0 ; singleGeomsPtrsIdx < singleGeomsPtrs.size() ; 
                  ++singleGeomsPtrsIdx )
                {
                  if( singleGeomsPtrs[ singleGeomsPtrsIdx ]->getGeomTypeId() == 
                    te::gm::PointType )
                  {
                    intersectionPoints.push_back( (te::gm::Point*)(
                      singleGeomsPtrs[ singleGeomsPtrsIdx ]->clone() ) );
                  }
                  else if( singleGeomsPtrs[ singleGeomsPtrsIdx ]->getGeomTypeId() == 
                    te::gm::LineStringType )
                  {
                    lineStrPtr = ((te::gm::LineString*) singleGeomsPtrs[
                      singleGeomsPtrsIdx ]);
                    intersectionPoints.push_back( lineStrPtr->getStartPoint() );
                    intersectionPoints.push_back( lineStrPtr->getEndPoint() );
                  }
                }
              }
            }
          }
          catch(const std::exception& e)
          {
            TE_LOG_WARN( "Geometry intersection error:" + e.what() );
            
            // deleting the intersections points
            
            for( std::size_t intersectionPointsIdx = 0; intersectionPointsIdx < 
              intersectionPoints.size() ; ++intersectionPointsIdx )  
            {
              delete intersectionPoints[intersectionPointsIdx];
            }
            
            intersectionPoints.clear();
          }          
          catch(...)
          {
            TE_LOG_WARN( "Geometry intersection error" );
            
            // deleting the intersections points
            
            for( std::size_t intersectionPointsIdx = 0; intersectionPointsIdx < 
              intersectionPoints.size() ; ++intersectionPointsIdx )  
            {
              delete intersectionPoints[intersectionPointsIdx];
            }
            
            intersectionPoints.clear();
          }
        }

        // Removing duplicated points

        {
          std::size_t positionBegin = 0;
          std::size_t positionEnd = 0;
          std::vector<te::gm::Point*> intersectionPointsAux = intersectionPoints;

          intersectionPoints.clear();

          while( positionBegin < intersectionPointsAux.size() )
          {
            if( intersectionPointsAux[positionBegin] )
            {
              positionEnd = positionBegin + 1;

              while( positionEnd < intersectionPointsAux.size() )
              {
                if(
                    ( intersectionPointsAux[positionEnd] != 0 )
                    &&
                    intersectionPointsAux[positionBegin]->equals(
                      intersectionPointsAux[positionEnd], true)
                  )
                {
                  delete intersectionPointsAux[positionEnd];
                  intersectionPointsAux[positionEnd] = 0;
                }

                positionEnd++;
              }

              intersectionPoints.push_back( intersectionPointsAux[positionBegin] );
            }

            ++positionBegin;
          }
        }
        
        // Sort the intersection points through its Y coordinates (column)
        std::sort(intersectionPoints.begin(), intersectionPoints.end(),
                  te::rst::StdSortPointPointerComparison);        
  
        // Using the intersection points, build a vector of coordinates (columns) with the start and
        // end column of the current line for each stretch
        
        std::size_t positionBegin = 0;
        std::size_t positionEnd = 0;
        int startingCol = 0;
        int endingCol = 0;
        double startingX = 0;
        double startingY = 0;
        double endingX = 0;
        double endingY = 0;          
        
        while( ( positionBegin + 1 ) < intersectionPoints.size() )  
        {
          positionEnd = positionBegin + 1;
          
          startingX = intersectionPoints[positionBegin]->getX();
          startingX = std::max( startingX, rasterEnvelopeLLX );

          startingY = intersectionPoints[positionBegin]->getY();

          endingX = intersectionPoints[positionEnd]->getX();
          endingX = std::min( endingX, rasterEnvelopeURX );

          endingY = intersectionPoints[positionEnd]->getY();

          // Build the vector of coordinates with the folowing structure: 
          // vector<pair<startcolumn, endcolumn>>;
          // where each pair represents a stretch to be transversed
          
          if(
              ( startingX != endingX )
              &&
              (
                m_tileIndexer->withinOrTouches(te::gm::Point(
                (startingX + (endingX - startingX) / 2.0 ), (startingY +
                (endingY - startingY) / 2.0 ), m_polygon->getSRID()))
              )
            )
          {
            // The middle-point (between the start end end point)
            // inside the current polygon
            
            startingCol = (int)std::ceil( AbstractPositionIterator<T>::m_raster->getGrid()->geoToGrid(startingX, startingY).x );
            startingCol = std::max( 0, startingCol );
            startingCol = std::min( m_maxcolumns - 1, startingCol );
            
            endingCol = (int)std::floor( AbstractPositionIterator<T>::m_raster->getGrid()->geoToGrid(endingX, endingY).x );
            endingCol = std::max( 0, endingCol );
            endingCol = std::min( m_maxcolumns - 1, endingCol );

            if( endingCol >= startingCol )
            {
              m_columns.push_back(std::pair<int, int>(startingCol, endingCol));
            }
          } 

          positionBegin = positionEnd;
        }
        
        // deleting the intersections points
        
        positionBegin = 0;
        
        while ( positionBegin < intersectionPoints.size() )  
        {
          delete intersectionPoints[positionBegin];
          ++positionBegin;
        }

        if (m_columns.empty())
        {
          m_row++;
          if (m_row > m_endingrow)
          {
            setEnd();

            return;
          }

          setNextLine();

          return;
        }

        m_actualintersection = 0;

        m_nintersections = static_cast<int>(m_columns.size());
      }

      m_startingcolumn = this->m_columns[m_actualintersection].first;
      m_endingcolumn = this->m_columns[m_actualintersection].second;

      int tmp;
      if (m_startingcolumn > m_endingcolumn)
      {
        tmp = m_startingcolumn;
        m_startingcolumn = m_endingcolumn;
        m_endingcolumn = tmp;
      }

// // avoiding bad access
//       m_startingcolumn = m_startingcolumn < 0? 0: m_startingcolumn;
//       m_startingcolumn = m_startingcolumn >= m_maxcolumns? m_maxcolumns - 1: m_startingcolumn;
// 
//       m_endingcolumn = m_endingcolumn < 0? 0: m_endingcolumn;
//       m_endingcolumn = m_endingcolumn >= m_maxcolumns? m_maxcolumns - 1: m_endingcolumn;
    }

    template<class T> const std::vector<T> te::rst::PolygonIterator<T>::operator*() const
    {
      std::vector<T> values(this->m_raster->getNumberOfBands());
      double value;

      for (unsigned int b = 0; b < this->m_raster->getNumberOfBands(); b++)
      {
        this->m_raster->getValue(getColumn(), getRow(), value, b);
        values[b] = ((T) value);
      }

      return values;
    }

    template<class T> T te::rst::PolygonIterator<T>::operator[](const unsigned int i) const
    {
      this->m_raster->getValue(m_column, m_row, m_operatorBrackets_value, i);

      return (T) m_operatorBrackets_value;
    }
    
    template<class T> std::complex< T > te::rst::PolygonIterator<T>::operator()(const unsigned int i) const
    {
      this->m_raster->getValue(m_column, m_row, m_operatorParenthesis_value, i);

      return (std::complex< T >) m_operatorParenthesis_value;
    }    

    template<class T> unsigned int te::rst::PolygonIterator<T>::getRow() const
    {
      return m_row;
    }

    template<class T> unsigned int te::rst::PolygonIterator<T>::getColumn() const
    {
      return m_column;
    }

    template<class T> void te::rst::PolygonIterator<T>::operator++()
    {
      m_column++;

      if (m_column > m_endingcolumn)
      {
        m_actualintersection++;

        if (m_actualintersection >= m_nintersections)
          m_row++;

        if (m_row > m_endingrow)
        {
          setEnd();

          return;
        }

        setNextLine();

        m_column = m_startingcolumn;
      }
    }

    template<class T> void te::rst::PolygonIterator<T>::operator--()
    {
      if (m_row == -1) 
          throw te::rst::Exception(TE_TR("This operation is not supported!"));
      m_column--;

      if (m_column < m_startingcolumn)
      {
        m_actualintersection--;

        if (m_actualintersection < 0)
          m_row--;

        if (m_row < m_startingrow)
        {
          setEnd();

          return;
        }

        setNextLine();

        m_column = m_endingcolumn;
      }
    }

    template<class T> 
    te::rst::PolygonIterator<T>& te::rst::PolygonIterator<T>::operator=(
      const te::rst::PolygonIterator<T>& rhs)
    {
      clear();
      
      if (this != &rhs)
      {
        te::rst::AbstractPositionIterator<T>::operator=(rhs);

        m_polygon = rhs.m_polygon;

        if( rhs.m_currline )
        {
          m_currline = (te::gm::Line*)rhs.m_currline->clone();
        }
        
        m_column = rhs.m_column;
        m_row = rhs.m_row;
        m_startingcolumn = rhs.m_startingcolumn;
        m_endingcolumn = rhs.m_endingcolumn;
        m_startingrow = rhs.m_startingrow;
        m_endingrow = rhs.m_endingrow;
        m_maxcolumns = rhs.m_maxcolumns;
        m_maxrows = rhs.m_maxrows;
        m_actualintersection = rhs.m_actualintersection;
        m_nintersections = rhs.m_nintersections;
      }

      return *this;
    }

    template<class T> void te::rst::PolygonIterator<T>::setEnd()
    {
      m_column = -1;
      m_row = -1;
    }

    template<class T> te::rst::PolygonIterator<T> te::rst::PolygonIterator<T>::begin(const te::rst::Raster* r, const te::gm::Polygon* p)
    {
      return te::rst::PolygonIterator<T>(r, p);
    }

    template<class T> te::rst::PolygonIterator<T> te::rst::PolygonIterator<T>::end(const te::rst::Raster* r, const te::gm::Polygon* p)
    {
      te::rst::PolygonIterator<T> it(r, p);

      it.setEnd();

      return it;
    }

    template<class T> bool te::rst::PolygonIterator<T>::operator!=(const te::rst::PolygonIterator<T>& rhs) const
    {
      return ( (this->m_row != rhs.m_row) && (this->m_column != rhs.m_column));
    }
    
    template<class T> void te::rst::PolygonIterator<T>::clear()
    {
      m_polygon = 0;

      if( m_currline )
      {
        delete m_currline;
        m_currline = 0;
      }
      
      m_column = -1;
      m_row = -1;
      m_startingcolumn = 0;
      m_endingcolumn = 0;
      m_startingrow = 0;
      m_endingrow = 0;
      m_maxcolumns = 0;
      m_maxrows = 0;
      m_actualintersection = -1;
      m_nintersections = 0;
    }    

// implementation of iteration strategy bounded by a line
    template<class T> te::rst::LineIterator<T>::LineIterator()
      : AbstractPositionIterator<T>(),
        m_line(0),
        m_currentpixelindex(0),
        m_pixelsinline(0)
    {
    }

    template<class T> te::rst::LineIterator<T>::LineIterator(const te::rst::Raster* r, const te::gm::Line* l)
      : AbstractPositionIterator<T>(r),
        m_line(l),
        m_currentpixelindex(0),
        m_pixelsinline(0)
    {
      if( r->getSRID() != l->getSRID() )
      {
        throw te::rst::Exception( TE_TR("Invalid line SRID") );
      }
      
      int srid = this->m_raster->getSRID();

// make intersection between line and band's envelope
      te::gm::Geometry* bandEnvelope = te::gm::GetGeomFromEnvelope(this->m_raster->getExtent(), srid);
      te::gm::Geometry* inter = bandEnvelope->intersection(m_line);

      if (inter->isEmpty())
      {
        setEnd();

        return;
      }

// create line that intersects only band's envelope
      te::gm::Line* inrasterline = (te::gm::Line*) inter;

// find starting and ending points
      double startingcolumn;
      double startingrow;
      te::gm::Point* startpoint = inrasterline->getStartPoint();
      this->m_raster->getGrid()->geoToGrid(startpoint->getX(), startpoint->getY(),
                                           startingcolumn, startingrow);

      double endingcolumn;
      double endingrow;
      te::gm::Point* endpoint = inrasterline->getEndPoint();
      this->m_raster->getGrid()->geoToGrid(endpoint->getX(), endpoint->getY(),
                                           endingcolumn, endingrow);

// creating one envelope per pixel, and intersects with line
      const double resXdiv2 = this->m_raster->getResolutionX() / 2;
      const double resYdiv2 = this->m_raster->getResolutionY() / 2;
      double x1, x2, y1, y2, geoX, geoY;
      for(int r = (int)startingrow; r <= (int)endingrow; r++)
        for(int c = (int)startingcolumn; c <= (int)endingcolumn; c++)
        {
// define envelope of pixel
          this->m_raster->getGrid()->gridToGeo(c, r, geoX, geoY);
          x1 = geoX - resXdiv2; y1 = geoY - resYdiv2;
          x2 = geoX + resXdiv2; y2 = geoY + resYdiv2;

          te::gm::Envelope* pixelbox = new te::gm::Envelope(x1, y1, x2, y2);
          te::gm::Geometry* pixelboxgeometry = GetGeomFromEnvelope(pixelbox, srid);

          if (te::gm::SatisfySpatialRelation(inrasterline, pixelboxgeometry, te::gm::INTERSECTS))
            m_pixelsinline.push_back(new te::gm::Point(c, r, srid));
        }

        if (m_pixelsinline.empty())
          setEnd();
    }

    template<class T> te::rst::LineIterator<T>::LineIterator(const LineIterator<T>& rhs)
      : AbstractPositionIterator<T>(rhs),
        m_currentpixelindex(rhs.m_currentpixelindex),
        m_pixelsinline(rhs.m_pixelsinline)
    {
    }

    template<class T> te::rst::LineIterator<T>::~LineIterator()
    {
      m_pixelsinline.clear();
    }

    template<class T> const std::vector<T> te::rst::LineIterator<T>::operator*() const
    {
      std::vector<T> values(this->m_raster->getNumberOfBands());
      double value;

      for (unsigned int b = 0; b < this->m_raster->getNumberOfBands(); b++)
      {
        this->m_raster->getValue(getColumn(), getRow(), value, b);
        values[b] = ((T) value);
      }

      return values;
    }

    template<class T> T te::rst::LineIterator<T>::operator[](const unsigned int i) const
    {
      this->m_raster->getValue(getColumn(), getRow(), m_operatorBrackets_value, i);

      return (T) m_operatorBrackets_value;
    }
    
    template<class T> std::complex< T > te::rst::LineIterator<T>::operator()(const unsigned int i) const
    {
      this->m_raster->getValue(getColumn(), getRow(), m_operatorParenthesis_value, i);

      return (std::complex< T >) m_operatorParenthesis_value;
    }
        
    template<class T> unsigned int te::rst::LineIterator<T>::getRow() const
    {
      return (unsigned int)(m_pixelsinline[m_currentpixelindex]->getY());
    }

    template<class T> unsigned int te::rst::LineIterator<T>::getColumn() const
    {
      return (unsigned int)(m_pixelsinline[m_currentpixelindex]->getX());
    }

    template<class T> void te::rst::LineIterator<T>::operator++()
    {
      m_currentpixelindex++;

      if (m_currentpixelindex >= (int)(m_pixelsinline.size()))
        setEnd();
    }

    template<class T> void te::rst::LineIterator<T>::operator--()
    {
      m_currentpixelindex--;

      if (m_currentpixelindex < 0)
        setEnd();
    }

    template<class T> te::rst::LineIterator<T>& te::rst::LineIterator<T>::operator=(const te::rst::LineIterator<T>& rhs)
    {
      if (this != &rhs)
      {
        te::rst::AbstractPositionIterator<T>::operator=(rhs);

        m_line = rhs.m_line;
        m_currentpixelindex = rhs.m_currentpixelindex;
        m_pixelsinline = rhs.m_pixelsinline;
      }

      return *this;
    }

    template<class T> void te::rst::LineIterator<T>::setEnd()
    {
      this->m_currentpixelindex = -1;
    }

    template<class T> te::rst::LineIterator<T> te::rst::LineIterator<T>::begin(const te::rst::Raster* r, const te::gm::Line* l)
    {
      return te::rst::LineIterator<T>(r, l);
    }

    template<class T> te::rst::LineIterator<T> te::rst::LineIterator<T>::end(const te::rst::Raster* r, const te::gm::Line* l)
    {
      te::rst::LineIterator<T> it(r, l);

      it.setEnd();

      return it;
    }

    template<class T> bool te::rst::LineIterator<T>::operator!=(const te::rst::LineIterator<T>& rhs) const
    {
      return ( (this->m_currentpixelindex != rhs.m_currentpixelindex) );
    }

// implementation of iteration strategy bounded by a vector of points
    template<class T> te::rst::PointSetIterator<T>::PointSetIterator()
      : AbstractPositionIterator<T>(),
        m_pixelsinpointset(0),
        m_currentpixelindex(0)
    {
    }

    template<class T> te::rst::PointSetIterator<T>::PointSetIterator(const te::rst::Raster* r, const std::vector<te::gm::Point*> p)
      : AbstractPositionIterator<T>(r),
        m_pixelsinpointset(p),
        m_currentpixelindex(0)
    {
      const int rasterSRID = this->m_raster->getSRID();

      const te::gm::Envelope* rasterbox = r->getExtent();
      te::gm::Geometry* rasterboxgeometry = GetGeomFromEnvelope(rasterbox, rasterSRID);

// remove points that are not inside the band's envelope
      std::vector<te::gm::Point*> inside_points;
      double column;
      double row;
      for (unsigned int i = 0; i < m_pixelsinpointset.size(); i++)
      {
        if( rasterSRID != m_pixelsinpointset[i]->getSRID() )
        {
          throw te::rst::Exception( TE_TR("Invalid point SRID") );
        }        
        
        if (te::gm::SatisfySpatialRelation(m_pixelsinpointset[i], rasterboxgeometry, te::gm::INTERSECTS))
        {
          this->m_raster->getGrid()->geoToGrid(m_pixelsinpointset[i]->getX(), m_pixelsinpointset[i]->getY(), column, row);

          inside_points.push_back(new te::gm::Point(column, row));
        }
      }
      
      m_pixelsinpointset.clear();
      m_pixelsinpointset = inside_points;

      if (m_pixelsinpointset.empty())
        setEnd();
    }

    template<class T> te::rst::PointSetIterator<T>::PointSetIterator(const PointSetIterator<T>& rhs)
      : AbstractPositionIterator<T>(rhs),
        m_pixelsinpointset(rhs.m_pixelsinpointset),
        m_currentpixelindex(rhs.m_currentpixelindex)
    {
    }

    template<class T> te::rst::PointSetIterator<T>::~PointSetIterator()
    {
      m_pixelsinpointset.clear();
    }

    template<class T> const std::vector<T> te::rst::PointSetIterator<T>::operator*() const
    {
      std::vector<T> values(this->m_raster->getNumberOfBands());
      double value;

      for (unsigned int b = 0; b < this->m_raster->getNumberOfBands(); b++)
      {
        this->m_raster->getValue(getColumn(), getRow(), value, b);
        values[b] = ((T) value);
      }

      return values;
    }

    template<class T> T te::rst::PointSetIterator<T>::operator[](const unsigned int i) const
    {
      this->m_raster->getValue(getColumn(), getRow(), m_operatorBrackets_value, i);

      return (T) m_operatorBrackets_value;
    }
    
    template<class T> std::complex< T > te::rst::PointSetIterator<T>::operator()(const unsigned int i) const
    {
      this->m_raster->getValue(getColumn(), getRow(), m_operatorParenthesis_value, i);

      return (std::complex< T >) m_operatorParenthesis_value;
    }    

    template<class T> unsigned int te::rst::PointSetIterator<T>::getRow() const
    {
      return (unsigned int)(m_pixelsinpointset[m_currentpixelindex]->getY());
    }

    template<class T> unsigned int te::rst::PointSetIterator<T>::getColumn() const
    {
      return (unsigned int)(m_pixelsinpointset[m_currentpixelindex]->getX());
    }

    template<class T> void te::rst::PointSetIterator<T>::operator++()
    {
      m_currentpixelindex++;

      if (m_currentpixelindex >= (int) m_pixelsinpointset.size())
        setEnd();
    }

    template<class T> void te::rst::PointSetIterator<T>::operator--()
    {
      m_currentpixelindex--;

      if (m_currentpixelindex < 0)
        setEnd();
    }

    template<class T> te::rst::PointSetIterator<T>& te::rst::PointSetIterator<T>::operator=(const te::rst::PointSetIterator<T>& rhs)
    {
      if (this != &rhs)
      {
        te::rst::AbstractPositionIterator<T>::operator=(rhs);

        m_pixelsinpointset = rhs.m_pixelsinpointset;
        m_currentpixelindex = rhs.m_currentpixelindex;
      }

      return *this;
    }

    template<class T> void te::rst::PointSetIterator<T>::setEnd()
    {
      this->m_currentpixelindex = -1;
    }

    template<class T> te::rst::PointSetIterator<T> te::rst::PointSetIterator<T>::begin(const te::rst::Raster* r, const std::vector<te::gm::Point*> p)
    {
      return te::rst::PointSetIterator<T>(r, p);
    }

    template<class T> te::rst::PointSetIterator<T> te::rst::PointSetIterator<T>::end(const te::rst::Raster* r, const std::vector<te::gm::Point*> p)
    {
      te::rst::PointSetIterator<T> it(r, p);

      it.setEnd();

      return it;
    }

    template<class T> bool te::rst::PointSetIterator<T>::operator!=(const te::rst::PointSetIterator<T>& rhs) const
    {
      return ( (this->m_currentpixelindex != rhs.m_currentpixelindex) );
    }

  } // end namespace rst
}   // end namespace te

#endif  // __TERRALIB_RASTER_INTERNAL_POSITIONITERATOR_H
