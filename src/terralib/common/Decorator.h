/*  Copyright (C) 2001-2010 National Institute For Space Research (INPE) - Brazil.

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
  \file Decorator.h
  \brief Defines an abstraction for a Decorator.
  \details Decorators are classes that can change object behavioral in run-time. For more informations about decorator pattern, 
  see <A HREF="http://en.wikipedia.org/wiki/Decorator_pattern">Decorator pattern on Wikipedia.</A>
  \version 5.0
  \author Frederico Augusto Bed&ecirc; &lt;frederico.bede@funcate.org.br&gt;
  \date 2001-2012
  \copyright GNU Lesser General Public License.
 */
#ifndef __TERRALIB_COMMON_INTERNAL_DECORATOR_H
#define __TERRALIB_COMMON_INTERNAL_DECORATOR_H

namespace te 
{
  namespace common 
  {

    /*!
      \class Decorator
      \copydoc Decorator.h 
    */
    template<class T>
    class Decorator : public T 
    {
    public:
      /*! 
        \brief Constructor.    
        \details The \a deleteDecorated argument tells if the decorator HAS or NOT the ownership of the decorated pointer.
        A \a true value gives the ownership to the decorator and the client do not need manage memory of the decorated pointer.
        Otherwise, the client needs manage the memory of the decorated pointer.
        \param decorated The object to be decorated.
        \param deleteDecorated Tells to decorator to also delete decorated pointer. 
      */
      Decorator(T * decorated, const bool& deleteDecorated=false);

      /*! 
        \brief Virtual destructor.
      */
      virtual ~Decorator();

      /*! 
        \brief Copy constructor.    
        \param source Object to be copied.
      */
      Decorator(const Decorator<T> & source);

      /*! 
        \brief Copy operator.    
        \param source Object to be copied.
      */
      Decorator<T> & operator=(const Decorator<T> & source);

      /*! 
        \brief Returns the pointer of decorated object.    
      */
      T* getDecorated();

    protected:
      T * m_decorated;      //!< The object decorated.
      bool m_delDecorated;  //!< If true, also delete decorated pointer.
    };

    //! Methods implementation
    template<class T>
    Decorator<T>::Decorator(T * decorated, const bool& deleteDecorated) :
    T(),
    m_delDecorated(deleteDecorated),
    m_decorated(decorated)
    {
    }

    template<class T>
    Decorator<T>::~Decorator() 
    {
      if(m_delDecorated)
        delete m_decorated;
    }

    template<class T>
    Decorator<T>::Decorator(const Decorator<T> & source) : 
    m_decorated(source.m_decorated),
    m_delDecorated(source.m_delDecorated)
    {
    }

    template<class T>
    Decorator<T> & Decorator<T>::operator=(const Decorator<T> & source) 
    {
      m_decorated = source.m_decorated;
      m_delDecorated = source.m_delDecorated;

      return *this;
    }

    template<class T>
    T* Decorator<T>::getDecorated() 
    {
      return m_decorated;
    }
  } 
}

#endif //__TERRALIB_COMMON_INTERNAL_DECORATOR_H
