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
  \file terralib/color/RGBAColor.h

  \brief A helper class for 24-bit RGBA (Red-Green-Blue-Alpha channel) color.
*/

#ifndef __TERRALIB_COLOR_INTERNAL_RGBACOLOR_H
#define __TERRALIB_COLOR_INTERNAL_RGBACOLOR_H

// TerraLib
#include "../core/utils/HexUtils.h"
#include "Config.h"

// STL 
#include <cassert>
#include <string>
#include <cstdio>

namespace te
{
  namespace color
  {
    /*!
      \class RGBAColor

      \brief A helper class for 32-bit RGBA (Red-Green-Blue-Alpha channel) color.

      Internally the color is represented by an integer of 32-bits as the following:
      <ul>
      <li>bits 0-7: blue</li>
      <li>bits 8-15: green</li>
      <li>bits 16-23: red</li>
      <li>bits 24-31: alpha. See the macro TE_OPAQUE and TE_TRANSPARENT for the values of totally transparent and totally opaque.</li>
      </ul>

      \ingroup color
    */
    class RGBAColor
    {
      public:
    
        /** @name Initializer Methods
         *  Methods related to instantiation and destruction.
         */
        //@{

        /*! \brief Default constructor of a new color. */
        RGBAColor();

        /*!
          \brief It initializes a new color with the given value.

          \param rgba The color encoded as an integer.
         */
        RGBAColor(int rgba);

        /*!
          \brief It initializes a new color.

          \param r Red component (range from 0 to 255).
          \param g Green component (range from 0 to 255).
          \param b Blue component (range from 0 to 255).
          \param a Alpha component. A range from 0 to 255 (see the macro TE_OPAQUE and TE_TRANSPARENT for the values of totally transparent and totally opaque).
         */
        RGBAColor(int r, int g, int b, int a);

        /*!
          \brief It initializes a new color from a given hex RGB-encoded string.

          \param hexColor The color value is RGB-encoded using
                          two hexadecimal digits per primary-color
                          component, in the order Red, Green, Blue,
                          prefixed with a hash (#) sign.
                          The hexadecimal digits between
                          A and F may be in uppercase
                          or lowercase.
         */
        RGBAColor(const std::string& hexColor);

        /*! \brief Destructor */
        ~RGBAColor();

        //@}

        /** @name Accessor methods
         *  Methods used to get or set properties.
         */
        //@{

        /*!
          \brief Assigns a copy of a color to this color, and returns a reference to it.

          \param color The rgba color to be copied.

          \return Returns a reference to it.
        */
        te::color::RGBAColor& operator=(const RGBAColor& color);

        /*!
          \brief Assigns a copy of a color to this color.
          
          \param rgba the 32-bit RGBA (Red-Green-Blue-Alpha channel) color to be copied.

          \return Returns a reference to it.
        */
        te::color::RGBAColor& operator=(const int& rgba);

        /*!
          \brief It compares the colors are iqual.

          \param color The rgba color to be compared.
            
          \return Returns true if this color has the same RGB and alpha values.
       */
       bool operator==(const RGBAColor& color) const;

        /*!
          \brief It compares the colors are different.

          \param color The rgba color to be compared.
            
          \return Returns true if this color has not the same RGB and alpha values.
       */
       bool operator!=(const RGBAColor& color) const;

        /*!
          \brief It returns the red component color value (a value from 0 to 255).

          \return The red component color value (a value from 0 to 255).
         */
        int getRed() const;

        /*!
          \brief It returns the green component color value (a value from 0 to 255).

          \return The green component color value (a value from 0 to 255).
         */
        int getGreen() const;

        /*!
          \brief It returns the blue component color value (a value from 0 to 255).

          \return The blue component color value (a value from 0 to 255).
         */
        int getBlue() const;

        /*!
          \brief It returns the alpha component color value (a value from 0 to 255).

          \return The alpha component color value. It is a value from 0 to 255 (see the macro TE_OPAQUE and TE_TRANSPARENT for the values of totally transparent and totally opaque).
         */
        int getAlpha() const;

        /*!
          \brief It gets the color value.

          \param r Red component.
          \param g Green component.
          \param b Blue component.
          \param a Alpha component.
         */
        void getRgba(int* r, int* g, int* b, int* a=0) const;

        /*!
          \brief It returns the color value (0xAARRGGBB).

          \return It returns a number of 4 bytes. The values are on the following order: alpha, red, green and blue. The alpha is the most significant byte.
         */
        int getRgba() const;

        /*!
          \brief It sets the color using a two hexadecimal RGB-encoded color.

          \param hexColor The color value is RGB-encoded using
                          two hexadecimal digits per primary-color
                          component, in the order Red, Green, Blue,
                          prefixed with a hash (#) sign.
                          The hexadecimal digits between
                          A and F may be in either uppercase
                          or lowercase.
         */
        void setColor(const std::string& hexColor);

        /*!
          \brief It gets the color encoded using two hexadecimal digits per primary-color component, in the order Red, Green, Blue, prefixed with a hash (#) sign.

          \return A string that represents the color encoded using two hexadecimal digits per primary-color component. e.g. #FF0000

          \note The hexadecimal digits between A and F will be uppercase.
         */
        std::string getColor() const;

        /*!
          \brief It sets the color.

          \param r Red component (range from 0 to 255).
          \param g Green component (range from 0 to 255).
          \param b Blue component (range from 0 to 255).
          \param a Alpha component. A range from 0 to 255 (see the macro TE_OPAQUE and TE_TRANSPARENT for the values of totally transparent and totally opaque).
         */
        void setColor(int r, int g, int b, int a=255);

        /*!
          \brief It sets the color.

          \param rgba The color represented by a number of 4 bytes, where the values ??are in the following order: alpha, red, green and blue. The alpha is the most significant byte.
         */
        void setColor(int rgba);

        /*!
          \brief It returns the internal representation as a 32-bits RGBA color.

          \return The internal representation as a 32-bit RGBA color.
         */
        //int value() const;

        //@}

      private:

        int m_rgba; //!< Internal color, stored as a 32-bit.
    };

    inline RGBAColor::RGBAColor()
      : m_rgba(0)
    {
    }

    inline RGBAColor::RGBAColor(int rgba)
      : m_rgba(rgba)
    {
    }

    inline RGBAColor::RGBAColor(int r, int g, int b, int a)
      : m_rgba((a << 24) + (r << 16) + (g << 8) + b)
    {
      assert(r >= 0 && r <= 255);
      assert(g >= 0 && g <= 255);
      assert(b >= 0 && b <= 255);
      assert(a >= 0 && a <= 255);
    }

    inline RGBAColor::RGBAColor(const std::string& hexColor)
    {
      setColor(hexColor);
    }

    inline RGBAColor::~RGBAColor()
    {
    }

    inline te::color::RGBAColor& RGBAColor::operator=(const RGBAColor& color)
    {
        m_rgba = color.m_rgba;
        return *this;
    }

    inline te::color::RGBAColor& RGBAColor::operator=(const int& rgba)
    {
        m_rgba = rgba;
        return *this;
    }

    inline bool RGBAColor::operator==(const RGBAColor& color) const
    {
      if(m_rgba == color.m_rgba)
        return true;
      return false;
    }

    inline bool RGBAColor::operator!=(const RGBAColor& color) const
    {
      return !operator==(color);
    }

    inline int RGBAColor::getRed() const
    {
      return (((m_rgba) & 0xFF0000) >> 16);
    }

    inline int RGBAColor::getGreen() const
    {
      return (((m_rgba) & 0x00FF00) >> 8);
    }

    inline int RGBAColor::getBlue() const
    {
      return ((m_rgba) & 0x0000FF);
    }

    inline int RGBAColor::getAlpha() const
    {
      return (((m_rgba) & 0xFF000000) >> 24);
    }
        
    inline void RGBAColor::getRgba(int* r, int* g, int* b, int* a) const
    {
      if(a)
        *a = (m_rgba & 0xFF000000) >> 24;
      *r = (m_rgba & 0xFF0000) >> 16;
      *g = (m_rgba & 0xFF00) >> 8;
      *b = m_rgba & 0xFF;
    }

    inline int RGBAColor::getRgba() const
    {
      return m_rgba;
    }

    inline void RGBAColor::setColor(const std::string& hexColor)
    {
      assert(hexColor.size() == 7);
      assert(hexColor[0] == '#');

      unsigned char highR = te::core::GetDecimalFromHexNotCS(hexColor[1]);
      unsigned char lowR =  te::core::GetDecimalFromHexNotCS(hexColor[2]);
      int r = int((highR << 4) + lowR);

      unsigned char highG = te::core::GetDecimalFromHexNotCS(hexColor[3]);
      unsigned char lowG = te::core::GetDecimalFromHexNotCS(hexColor[4]);
      int g = int((highG << 4) + lowG);

      unsigned char highB = te::core::GetDecimalFromHexNotCS(hexColor[5]);
      unsigned char lowB = te::core::GetDecimalFromHexNotCS(hexColor[6]);
      int b = int((highB << 4) + lowB);

      setColor(r, g, b, TE_OPAQUE);
    }

    inline std::string RGBAColor::getColor() const
    {
      char buffer[8];
      sprintf(buffer, "#%02X%02X%02X", getRed(), getGreen(), getBlue());
      std::string result(buffer);
      return result;
    }

    inline void RGBAColor::setColor(int r, int g, int b, int a)
    {
      m_rgba = (a << 24) + (r << 16) + (g << 8) + b;
    }

    inline void RGBAColor::setColor(int rgba)
    {
      m_rgba = rgba;
    }

  }   // end namespace color
}     // end namespace te

#endif  // __TERRALIB_COLOR_INTERNAL_RGBACOLOR_H

