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
  \file terralib/qt/widgets/se/StyleExplorer.h

  \brief A widget used to explore a style.
*/

#ifndef __TERRALIB_QT_WIDGETS_SE_INTERNAL_STYLEEXPLORER_H
#define __TERRALIB_QT_WIDGETS_SE_INTERNAL_STYLEEXPLORER_H

// TerraLib
#include "../Config.h"

// Qt
#include <QtCore/QString>
#include <QTreeWidget>

// STL
#include <map>

namespace te
{
// Forward declarations
  namespace se
  {
    class Rule;
    class Style;
    class Symbolizer;
  }

  namespace qt
  {
    namespace widgets
    {
      /*!
        \class StyleExplorer

        \brief A widget used to explore a style.

        \ingroup widgets
      */
      class TEQTWIDGETSEXPORT StyleExplorer : public QTreeWidget
      {
        Q_OBJECT

        private:

          /*!
            \enum NodeType

            \brief Auxiliary internal enumeration to control the StyleExplorer tree nodes.
          */
          enum NodeType
          {
            STYLE      = 0, /*!< Style root node. */
            RULE       = 1, /*!< Rule node.       */
            SYMBOLIZER = 2  /*!< Symbolizer node. */
          };

        public:

          /** @name Initializer Methods
           *  Methods related to instantiation and destruction.
           */
          //@{

          /*! \brief Constructs a style explorer widget which is a child of parent. */
          StyleExplorer(QWidget* parent = 0);

          /*! \brief Destructor. */
          ~StyleExplorer();

          //@}

        public:

          /*!
            \brief Sets a style element to this widget.

            \param style A valid style element.

            \note The widget will NOT take the ownership of the given style.
            \note The widget will be update based on given style parameters.
          */
          void setStyle(te::se::Style* style);

          /*! \brief This method updates the Style Explorer. */
          void updateStyleTree();

          /*!
            \brief Gets the current rule.

            \return The current rule on Style Explorer.

            \note If there is not a current rule a NULL point will be returned.
          */
          te::se::Rule* getCurrentRule();

          /*!
            \brief Gets the current symbolizer.

            \return The current symbolizer on Style Explorer.

            \note If there is not a current symbolizer a NULL point will be returned.
          */
          te::se::Symbolizer* getCurrentSymbolizer();

          void goUpSymbolizer();

          void goDownSymbolizer();

          void setLegendIconSize(int size);

          void importStyle(te::se::Style* style, bool isVisual);

        public slots:

          void onSymbolizerChanged(te::se::Symbolizer* symb);

        private:

          /*! \brief Auxiliary internal method to retrieve a rule from a QTreeWidgetItem. */
          te::se::Rule* getRule(QTreeWidgetItem* item);

          /*! \brief Auxiliary internal method to retrieve a symbolizer from a QTreeWidgetItem. */
          te::se::Symbolizer* getSymbolizer(QTreeWidgetItem* item);

          /*!
            \brief Auxiliary internal method to retrieve the selected item on Style Explorer.

            \return The selected QTreeWidgetItem on Style Explorer.

            \note If there is not a selected QTreeWidgetItem a NULL point will be returned.
          */
          QTreeWidgetItem* getSelectedItem() const;

          void swapSymbolizers(te::se::Rule* r, int indexFirst, int indexSecond);

        private slots:

          void onItemClicked(QTreeWidgetItem* item, int column);

        signals:

          /*! This signal is emitted when a rule is clicked. */
          void ruleClicked(te::se::Rule* rule);

          /*! This signal is emitted when a symbolizer is clicked. */
          void symbolizerClicked(te::se::Symbolizer* symb);

          void styleImported(te::se::Style* style, bool isVisual);

        private:

          te::se::Style* m_style;                        //!< Style element that will be explored by this widget.
          std::map<QString, QString> m_symbolizerNames; //!< A map of symbolizers names to user interface names.
      };

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_SE_INTERNAL_STYLEEXPLORER_H
