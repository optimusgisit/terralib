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
  \file terralib/core/utils/HexUtils.h

  \brief This file contains several utilities functions for dealing with HEX strings.

  \ingroup core
*/

#ifndef __TERRALIB_COMMON_INTERNAL_HEXUTILS_H
#define __TERRALIB_COMMON_INTERNAL_HEXUTILS_H

// STL 
#include <cstring>
#include <map>
#include <vector>

namespace te
{
  namespace core
  {
    /*!
      \brief It returns the decimal value in a char from a given hex value (for example: 'A' => 10, '8' => 8).
      
      \param hexValue The hex value we want to extract the decimal value.

      \return The decimal value in a char from a given hex value (for example: 'A' => 10, '8' => 8).

      \note This version is not case sensitive, i. e., 'A' and 'a' will return 10.      
    */
    inline unsigned char GetDecimalFromHexNotCS(unsigned char hexValue)
    {
      switch(hexValue)
      {
        case '0':
          return 0;

        case '1':
          return 1;

        case '2':
          return 2;

        case '3':
          return 3;

        case '4':
          return 4;

        case '5':
          return 5;

        case '6':
          return 6;

        case '7':
          return 7;

        case '8':
          return 8;

        case '9':
          return 9;

        case 'A':
        case 'a':
          return 10;

        case 'B':
        case 'b':
          return 11;

        case 'C':
        case 'c':
          return 12;

        case 'D':
        case 'd':
          return 13;

        case 'E':
        case 'e':
          return 14;

        //case 'F':
        //case 'f':
        default:
          return 15;
      }
    }

    /*!
      \brief It returns the decimal value in a char from a given hex value (for example: 'A' => 10, '8' => 8).
        
      \param hexValue The hex value we want to extract the decimal value.

      \return The decimal value in a char from a given hex value (for example: 'A' => 10, '8' => 8).

      \note This version is upper case sensitive, i. e., 'A' will return 10 and 'a' will return a wrong value.      
    */
    inline unsigned char GetDecimalFromHexUCase(char hexValue)
    {
      switch(hexValue)
      {
        case '0':
          return 0;

        case '1':
          return 1;

        case '2':
          return 2;

        case '3':
          return 3;

        case '4':
          return 4;

        case '5':
          return 5;

        case '6':
          return 6;

        case '7':
          return 7;

        case '8':
          return 8;

        case '9':
          return 9;

        case 'A':
          return 10;

        case 'B':
          return 11;

        case 'C':
          return 12;

        case 'D':
          return 13;

        case 'E':
          return 14;

        //case 'F':
        default:
          return 15;
      }
    }

    /*!
      \fn unsigned char GetDecimalFromHexLCase(char hexValue)

      \brief It returns the decimal value in a char from a given hex value (for example: 'a' => 10, '8' => 8).
        
      \param hexValue The hex value we want to extract the decimal value.

      \return The decimal value in a char from a given hex value (for example: 'a' => 10, '8' => 8).

      \note This version is lower case sensitive, i. e., 'a' will return 10 and 'A' will return a wrong value.
    */
    inline unsigned char GetDecimalFromHexLCase(char hexValue)
    {
      switch(hexValue)
      {
        case '0':
          return 0;

        case '1':
          return 1;

        case '2':
          return 2;

        case '3':
          return 3;

        case '4':
          return 4;

        case '5':
          return 5;

        case '6':
          return 6;

        case '7':
          return 7;

        case '8':
          return 8;

        case '9':
          return 9;

        case 'a':
          return 10;

        case 'b':
          return 11;

        case 'c':
          return 12;

        case 'd':
          return 13;

        case 'e':
          return 14;

        //case 'f':
        default:
          return 15;
      }
    }

    /*!
      \brief It converts the character to a hex representation.

      \param c The byte to be transformed to hex representation.
      \param r The resulting hex encoding will be write in the r pointer.

      \note It is used by Binary2Hex function.
    */
    inline void Char2Hex(unsigned char c, char* r)
    {
      static char ct[]={"0123456789ABCDEF" };
      int h;  // high byte
      int l;  // low byte

      h = (c >> 4);
      l = (c & 0x0F);

      r[0] = ct[h];
      r[1] = ct[l];
    }

    /*!
      \brief It converts the character from a hex representation to a byte.

      \param h At least a pair of bytes to be transformed from a hex representation.

      \note It is used by Hex2Binary function.

      \result A byte representation from a two bytes hex representation.
    */
    inline char Hex2Char(const char* hex)
    {
      char c = hex[0];
      char h, l;

      if(c >= '0' && c <= '9')
        h = c - 48;
      else
        h = c + 10 - 65;

      c = hex[1];

      if((c >= '0') && (c <= '9'))
        l = c - 48;
      else
        l = c + 10 - 65;

      return (h << 4) + l;
    }

    /*!
      \brief It converts the character from a hex representation to a byte.

      \param h At least a pair of bytes to be transformed from a hex representation.

      \note It is used by Hex2Binary function.

      \result A byte representation from a two bytes hex representation.

      \warning In our tests, this version was slower than the other.
    */
    inline unsigned char Hex2Char2(const char* hex)
    {
      unsigned char h = 0; // high byte
      unsigned char l = 0; // low byte

      switch(hex[0])
      {
        case '0' :
            h = 0;
            break;

        case '1' :
            h = 1;
            break;

        case '2' :
            h = 2;
            break;

        case '3' :
            h = 3;
            break;

        case '4' :
            h = 4;
            break;

        case '5' :
            h = 5;
            break;

        case '6' :
            h = 6;
            break;

        case '7' :
            h = 7;
            break;

        case '8' :
            h = 8;
            break;

        case '9' :
            h = 9;
            break;

        case 'A' :
        case 'a' :
            h = 10;
            break;

        case 'B' :
        case 'b' :
            h = 11;
            break;

        case 'C' :
        case 'c' :
            h = 12;
            break;

        case 'D' :
        case 'd' :
            h = 13;
            break;

        case 'E' :
        case 'e' :
            h = 14;
            break;

        case 'F' :
        case 'f' :
            h = 15;
            break;
      }

      switch(hex[1])
      {
        case '0' :
            l = 0;
            break;

        case '1' :
            l = 1;
            break;

        case '2' :
            l = 2;
            break;

        case '3' :
            l = 3;
            break;

        case '4' :
            l = 4;
            break;

        case '5' :
            l = 5;
            break;

        case '6' :
            l = 6;
            break;

        case '7' :
            l = 7;
            break;

        case '8' :
            l = 8;
            break;

        case '9' :
            l = 9;
            break;

        case 'A' :
        case 'a' :
            l = 10;
            break;

        case 'B' :
        case 'b' :
            l = 11;
            break;

        case 'C' :
        case 'c' :
            l = 12;
            break;

        case 'D' :
        case 'd' :
            l = 13;
            break;

        case 'E' :
        case 'e' :
            l = 14;
            break;

        case 'F' :
        case 'f' :
            l = 15;
            break;
      }

      return ((h << 4) + l);
    }    

    /*!
      \brief It converts each pair of hex characters from a NULL-terminated string of hex characters into a binary format.

      This function can be used, for example, to decode the returned geometry from PostGIS.

      \param hex A NULL-terminated string of hexadecimal characters.

      \return An array of binary characters in binary format.

      \note The size of the returned array is the half of the input array.

      \note The caller of this function will take the ownership of the returned
            pointer, so it must delete it. Use delete [] returned_pointer.
    */
    inline char* Hex2Binary(const char* hex)
    {
      std::size_t hSize = strlen(hex);

      std::size_t size = hSize / 2;

      char* buf = new char[size];

      char h, l;

      for(std::size_t i = 0; i < size; ++i)
      {
        char c = hex[i * 2];

        if(c >= '0' && c <= '9')
          h = c - 48;
        else
          h = c + 10 - 65;

        c = hex[i * 2 + 1];

        if((c >= '0') && (c <= '9'))
          l = c - 48;
        else
          l = c + 10 - 65;

        buf[i] = (h << 4) + l;
      }

      return buf;
    }

    /*!
      \brief It converts each pair of hex characters from an input string of hex characters into a binary format.

      This version can be used for those who wants to pre-allocate the memory where the parsed
      version will be output.

      \param hex A string of hexadecimal characters.
      \param hSize The number of bytes in the input string. Remember that it must be a multiple of 2.
      \param outBuff A pointer to a pre-allocated buffer where to output the binary version. It must have at least hSize/2 bytes.

      \note The size of the input buffer (outBuff) must be at least the half of the input array.

      \note The outBuff can be the same as the input buffer; in this case the input will be overwritten.      
    */
    inline void Hex2Binary(const char* hex, std::size_t hSize, char* outBuff)
    {
      std::size_t size = hSize / 2;

      char h, l;

      for(std::size_t i = 0; i < size; ++i)
      {
        char c = hex[i * 2];

        if(c >= '0' && c <= '9')
          h = c - 48;
        else
          h = c + 10 - 65;

        c = hex[i * 2 + 1];

        if((c >= '0') && (c <= '9'))
          l = c - 48;
        else
          l = c + 10 - 65;

        outBuff[i] = (h << 4) + l;
      }
    }

    /*!
      \brief It converts each pair of hex characters from a NULL-terminated string of hex characters into a binary format.

      This function can be used, for example, to decode the returned geometry from PostGIS.

      \param hex A NULL-terminated string of hexadecimal characters.

      \return An array of binary characters in binary format.

      \note The size of the returned array is the half of the input array.

      \note The caller of this function will take the ownership of the returned
            pointer, so it must delete it. Use delete [] returned_pointer.

      \warning In our tests, this version was slower than the other equivalent.
    */
    inline char* Hex2Binary2(const char* hex)
    {
      std::size_t hSize = strlen(hex);

      std::size_t size = hSize / 2;

      char* buf = new char[size];

      for(std::size_t i = 0; i < size; ++i)
        buf[i] = Hex2Char2(hex + (2 * i));

      return buf;
    }

    /*!
      \brief It converts each pair of hex characters from an input string of hex characters into a binary format.

      This version can be used for those who wants to pre-allocate the memory where the parsed
      version will be output.

      \param hex A string of hexadecimal characters.
      \param hSize The number of bytes in the input string. Remember that it must be a multiple of 2.
      \param outBuff A pointer to a pre-allocated buffer where to output the binary version. It must have at least hSize/2 bytes.

      \note The size of the input buffer (outBuff) must be at least the half of the input array.
    */
    inline void Hex2Binary2(const char* hex, std::size_t hSize, char* outBuff)
    {
      std::size_t size = hSize / 2;

      for(std::size_t i = 0; i < size; ++i)
        outBuff[i] = Hex2Char2(hex + (2 * i));
    }

    /*!
      \brief Each char from the array of characters in binary format is converted into a pair of hex characters. The final string is NULL terminated.

      \param s       An array of characters in binary format.
      \param size    The size of the string of characters (not including the trailing '\0').

      \note The caller of this function will take the ownership of the returned
            pointer, so it must delete it. Use delete [] returned_pointer.

      \note The returned buffer size is: 2 * size + 1.
    */
    inline char* Binary2Hex(const char* s, std::size_t size)
    {
      char* buff = new char[2 * size + 1];

      std::size_t i = 0;

      for(; i < size; ++i)
        Char2Hex(s[i], buff + (i * 2));

      buff[i * 2] = '\0';

      return buff;
    }

    /*!
      \brief Each char from the array of characters in binary format is converted into a pair of hex characters. The final string is NULL terminated.

      \param s       An array of characters in binary format.
      \param size    The size of the string of characters (not including the trailing '\0').
      \param outBuff A pointer to a pre-allocated buffer where to output the binary version. It must have at least 2*size+1 bytes.
    */
    inline void Binary2Hex(const char* s, std::size_t size, char* outBuff)
    {
      std::size_t i = 0;

      for(; i < size; ++i)
        Char2Hex(s[i], outBuff + (i * 2));

      outBuff[i * 2] = '\0';
    }

  } // end namespace common
}   // end namespace te

#endif  // __TERRALIB_COMMON_INTERNAL_HEXUTILS_H

