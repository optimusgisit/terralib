#include "LayerExplorer.h"
#include <terralib/qt/widgets/layer/LayerExplorer.h>
#include <terralib/qt/widgets/layer/LayerExplorerModel.h>
#include <terralib/qt/widgets/layer/AbstractTreeItem.h>
#include <terralib/qt/af/events/LayerAdded.h>
#include <terralib/qt/af/events/LayerSelected.h>
#include <terralib/qt/af/ApplicationController.h>
#include <terralib/qt/af/BaseApplicationController.h>
//#include <terralib/maptools/Layer.h> // * Under revision *

namespace te
{
  namespace qt
  {
    namespace af
    {
      LayerExplorer::LayerExplorer(te::qt::widgets::LayerExplorer* explorer) :
      QObject(explorer),
      m_explorer(explorer)
      {
        QItemSelectionModel* md = m_explorer->selectionModel();
        connect(md, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), SLOT(layerChanged(const QItemSelection&, const QItemSelection&)));
      }

      LayerExplorer::~LayerExplorer()
      {
      }

      void LayerExplorer::onApplicationTriggered(te::qt::af::Event* evt)
      {
        switch(evt->getId())
        {
          case te::qt::af::evt::LAYER_ADDED:
            {
              LayerAdded* e = static_cast<LayerAdded*>(evt);
              te::qt::widgets::LayerExplorerModel* model = static_cast<te::qt::widgets::LayerExplorerModel*>(m_explorer->model());

              int idx = model->rowCount(QModelIndex());

              model->insertItem(QModelIndex(), idx, e->m_layer);
              model->resetModel();
            }
          break;

          default:
          break;
        }
      }

      void LayerExplorer::layerChanged(const QItemSelection& selected, const QItemSelection&)
      {
        QModelIndexList lst = selected.indexes();

        if(lst.isEmpty())
          return;

        te::qt::widgets::AbstractTreeItem* item = static_cast<te::qt::widgets::AbstractTreeItem*>((*lst.begin()).internalPointer());

        if(item != 0 && item->isLayerItem())
        {
          te::map::AbstractLayerPtr abs_lay = item->getRefLayer();
          te::qt::af::LayerSelected ls_ev(abs_lay.get());
          ApplicationController::getInstance().broadcast(&ls_ev);
        }
      }
    }
  }
}