#ifndef __TERRALIB_EDIT_QT_INTERNAL_SUBTRACTAREATOOL_H
#define __TERRALIB_EDIT_QT_INTERNAL_SUBTRACTAREATOOL_H

// TerraLib
#include "../../../geometry/Envelope.h"
#include "../../../maptools/AbstractLayer.h"
#include "../../Feature.h"
#include "../Config.h"

// Qt
#include <QPointF>

#include "CreateLineTool.h"

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      class Canvas;
      class MapDisplay;
    }
  }

  namespace edit
  {
    // Forward declaration
    //class Feature;

    class TEEDITQTEXPORT SubtractAreaTool : public CreateLineTool
    {
      Q_OBJECT

    public:
      SubtractAreaTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, QObject* parent = 0);

      ~SubtractAreaTool();

      //@}

      /** @name AbstractTool Methods
      *  Methods related with tool behavior.
      */
      //@{

      bool mousePressEvent(QMouseEvent* e);

      bool mouseMoveEvent(QMouseEvent* e);

      bool mouseReleaseEvent(QMouseEvent* e);

      bool mouseDoubleClickEvent(QMouseEvent* e);

      //@}

    private:
      //void updateCursor();
      void drawPolygon();
      void draw();
      void reset();
      void storeEditedFeature();

      void pickFeature(const te::map::AbstractLayerPtr& layer, const QPointF& pos);

      te::gm::Envelope buildEnvelope(const QPointF& pos);

      te::gm::Geometry* buildPolygon();

      te::gm::Geometry* Difference(te::gm::Geometry* g1, te::gm::Geometry* g2);

      private slots:

      void onExtentChanged();

    protected:
      te::map::AbstractLayerPtr m_layer;
      te::edit::Feature* m_feature;

    };

  }   // end namespace edit
}     // end namespace te

#endif  // __TERRALIB_EDIT_QT_INTERNAL_AGGREGATEAREATOOL_H
