/*  Copyright (C) 2011-2012 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/qt/widgets/utils/ColorPickerToolButton.h

  \brief Custom widget used to pick a color.
*/

#ifndef __TERRALIB_QT_WIDGETS_UTILS_INTERNAL_COLORPICKERTOOLBUTTON_H
#define __TERRALIB_QT_WIDGETS_UTILS_INTERNAL_COLORPICKERTOOLBUTTON_H

// TerrLib
#include "../Config.h"

// Qt
#include <QtGui/QApplication>
#include <QtGui/QColor>
#include <QtGui/QColorDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtCore/QHash>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/QRectF>
#include <QtGui/QToolButton>

#define COLORSIZE 20
#define COLS 8
#define SPACE 4
#define MARGINY 19
#define MARGINX 4

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      /*!
        \class ColorKeyInfo

        \brief Associate a rect to a color.
      */
      class ColorKeyInfo
      {
        public:

          ColorKeyInfo(const QColor& color, const QRectF& rect)
          {
            m_color = color;
            m_rect = rect;
          }

          QColor m_color;
          QRectF m_rect;
      };

      /*!
        \class ColorPickerPopup

        \brief Create a frame with a default color palette.
      */
      class ColorPickerPopup : public QFrame
      {
        Q_OBJECT

        public:

          ColorPickerPopup(QWidget* parent = 0) : QFrame(parent)
          {
            setWindowFlags(Qt::Popup);
            setFrameShape(QFrame::StyledPanel);
            setFrameShadow(QFrame::Plain);
            setStyleSheet("QFrame{background-color: rgb(255, 255, 255);border-color: rgb(0, 0, 0);}");

            // Create default collors
            int i = 0; 
            for(int g = 0; g < 4; ++g)
              for(int r = 0;  r < 4; ++r)
                for(int b = 0; b < 3; ++b)
                  m_stdrgb[i++] = qRgb(r * 255 / 3, g * 255 / 3, b * 255 / 2);

            int width = COLS;
            int height = 48 / COLS;

            width = width * COLORSIZE + SPACE + MARGINX + 4;
            height = height * COLORSIZE + SPACE + MARGINY + 32;
            resize(width, height);

            setMouseTracking(true);
          }

          void paintButton(QPainter* painter, QRectF rc, QString text, bool hover)
          {
            if(!hover)
            {
              painter->setPen(Qt::black);
              painter->fillRect(rc, Qt::lightGray);
            }
            else
            {
              painter->setPen(Qt::black);
              painter->fillRect(rc, Qt::gray);
            }

            painter->drawRect(rc);
            painter->drawText(rc, text, Qt::AlignHCenter | Qt::AlignVCenter);
          }

          void paintEvent(QPaintEvent* /*e*/)
          {
            QPainter painter(this);

            painter.drawRect(0, 0, width() - 1, height() - 1);
            painter.drawText(QRect(10, 1, this->width(), 19), tr("Basic Colors"), Qt::AlignLeft | Qt::AlignVCenter);

            int col, row;

            m_colorInfos.clear();

            for(int i = 0; i < 48; ++i)
            {
              QRectF rc;
              col = i % COLS;
              row = (int)(i / COLS);

              rc.setLeft(col * COLORSIZE + SPACE + MARGINX);
              rc.setTop(row * COLORSIZE + SPACE + MARGINY);

              rc.setWidth(COLORSIZE - SPACE);
              rc.setHeight(COLORSIZE - SPACE);

              QColor color(m_stdrgb[i]);
              painter.fillRect(rc, color);
              m_colorInfos.append(ColorKeyInfo(color, rc));
            }

            QPointF mousePos = mapFromGlobal(QCursor::pos());

            m_buttonKeyRect = QRect(SPACE, (row + 1) * COLORSIZE + SPACE + MARGINY + 5, width() - 2 * SPACE, 20);
            
            paintButton(&painter, m_buttonKeyRect, tr("Custom Colors..."), m_buttonKeyRect.contains(mousePos));

            for(int i = 0; i < m_colorInfos.count(); ++i)
            {
              if(m_colorInfos[i].m_rect.contains(mousePos))
              {
                QPainter painter(this);

                m_hoverColor = m_colorInfos[i].m_color;
                QRectF rc = m_colorInfos[i].m_rect;

                rc.setTop(rc.top() - 2);
                rc.setLeft(rc.left() - 2);
                rc.setBottom(rc.bottom() + 2);
                rc.setRight(rc.right() + 2);

                painter.setPen(QPen(Qt::black, 2));
                painter.drawRect(rc);
              }
            }
          }

          void mouseMoveEvent(QMouseEvent* /*e*/)
          {
            repaint();
          }

          void mousePressEvent(QMouseEvent* e)
          {
            if(!rect().contains(e->pos()))
              close();

            for(int i = 0; i < m_colorInfos.count(); ++i)
            {
              if(m_colorInfos[i].m_rect.contains(e->pos()))
              {
                m_hoverColor = m_colorInfos.at(i).m_color;
                emit selected(m_hoverColor);
                close();
              }
            }

            if(m_buttonKeyRect.contains(e->posF()))
            {
              // Show original color dialog
              QColorDialog* dialog = new QColorDialog(this);
              dialog->setOptions(QColorDialog::ShowAlphaChannel);

              if(dialog->exec())
              {
                m_hoverColor = dialog->selectedColor();
                delete dialog;
                emit selected(m_hoverColor);
                close();
              }
            }
          }

        signals:

          void selected(const QColor& color);

        public:

          QColor m_hoverColor;
          QRgb m_stdrgb[48];
          QList<ColorKeyInfo> m_colorInfos;
          QRectF m_buttonKeyRect;
      };

      /*!
        \class ColorPickerToolButton

        \brief Custom tool button used to pick a color.
      */
      class TEQTWIDGETSEXPORT ColorPickerToolButton : public QToolButton
      {
        Q_OBJECT

        public:

          ColorPickerToolButton(QWidget* parent = 0);

          void mouseReleaseEvent(QMouseEvent* e);

          void setColor(const QColor& color);

          QColor color() const { return m_selectedColor; }

          void resizeEvent(QResizeEvent* e);

          void updateIcon();

        public slots:

          void onPopupSelected(const QColor& color);

        signals:

          void colorChanged();

        private:

          ColorPickerPopup* m_popup;
          QColor m_selectedColor;
      };

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif // __TERRALIB_QT_WIDGETS_UTILS_INTERNAL_COLORPICKERTOOLBUTTON_H
