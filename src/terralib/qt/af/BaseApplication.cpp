#include "BaseApplication.h"

#include "ui_BaseApplicationForm.h"

// TerraLib
#include "ApplicationController.h"
#include "connectors/ChartDisplayDockWidget.h"
#include "connectors/DataSetTableDockWidget.h"
#include "connectors/LayerExplorer.h"
#include "connectors/MapDisplay.h"
#include "connectors/StyleExplorer.h"
#include "events/ApplicationEvents.h"
#include "events/LayerEvents.h"
#include "events/MapEvents.h"
#include "events/ToolEvents.h"
#include "InternalSettingsDialog.h"
#include "Utils.h"

#include "../widgets/layer/explorer/FolderItem.h"
#include "../widgets/layer/explorer/LayerItem.h"

#include "../widgets/layer/info/LayerPropertiesInfoWidget.h"
#include "../widgets/layer/utils/CharEncodingMenuWidget.h"
#include "../widgets/layer/utils/SaveSelectedObjectsDialog.h"
#include "../widgets/tools/Info.h"
#include "../widgets/tools/Measure.h"
#include "../widgets/tools/Pan.h"
#include "../widgets/tools/Selection.h"
#include "../widgets/tools/ZoomArea.h"
#include "../widgets/tools/ZoomClick.h"
#include "../widgets/srs/SRSManagerDialog.h"
#include "../widgets/Utils.h"

#include "../../Defines.h"
#include "../../common/TerraLib.h"
#include "../../common/progress/ProgressManager.h"
#include "../../common/progress/TaskProgress.h"
#include "../../dataaccess/dataset/ObjectIdSet.h"
#include "../../dataaccess/utils/Utils.h"
#include "../../maptools/Utils.h"

// Qt
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>


te::qt::af::DataSetTableDockWidget* GetLayerDock(const te::map::AbstractLayer* layer, const std::vector<te::qt::af::DataSetTableDockWidget*>& docs)
{
  std::vector<te::qt::af::DataSetTableDockWidget*>::const_iterator it;

  for(it = docs.begin(); it != docs.end(); ++it)
  if((*it)->getLayer() == layer)
    return *it;

  return 0;
}


te::qt::af::BaseApplication::BaseApplication(QWidget* parent) :
  QMainWindow(parent)
{
  m_ui = 0;
  m_layerExplorer = 0;
  m_display = 0;
  m_styleExplorer = 0;
  m_encodingMenu = 0;
  m_app = 0;
  m_mapCursorSize = QSize(20, 20);

  te::qt::af::AppCtrlSingleton::getInstance().setMsgBoxParentWidget(this);
}


te::qt::af::BaseApplication::~BaseApplication()
{
  te::qt::af::UpdateUserSettings();

  while(!m_tables.empty())
    delete *m_tables.begin();

  delete m_layerExplorer;

  delete m_display;
  delete m_styleExplorer;

  if(m_app)
    m_app->finalize();

  delete m_ui;
}

void te::qt::af::BaseApplication::init(const QString& cfgFile)
{
  initFramework(cfgFile);

  m_ui = new Ui::BaseApplicationForm;
  m_ui->setupUi(this);

  try
  {
    makeDialog();

    //register menubar and load plugins
    m_app->initializePlugins();
  }
  catch(te::common::Exception& e)
  {
    QMessageBox::warning(this, "Error", e.what());
  }
}

te::qt::widgets::LayerItemView* te::qt::af::BaseApplication::getLayerExplorer()
{
  return m_ui->m_layerExplorer;
}

te::qt::widgets::MapDisplay* te::qt::af::BaseApplication::getMapDisplay()
{
  return m_ui->m_display;
}

te::qt::af::DataSetTableDockWidget* te::qt::af::BaseApplication::getLayerDock(const te::map::AbstractLayer* layer, const std::vector<te::qt::af::DataSetTableDockWidget*>& docs)
{
  std::vector<te::qt::af::DataSetTableDockWidget*>::const_iterator it;

  for (it = docs.begin(); it != docs.end(); ++it)
    if ((*it)->getLayer() == layer)
      return *it;

  return 0;
}

QDockWidget* te::qt::af::BaseApplication::getLayerExplorerDock()
{
  return m_ui->m_layerExplorerDockWidget;
}

void te::qt::af::BaseApplication::onApplicationTriggered(te::qt::af::evt::Event* e)
{
  switch (e->m_id)
  {
  case te::qt::af::evt::COORDINATE_TRACKED:
  {
    te::qt::af::evt::CoordinateTracked* ctE = static_cast<te::qt::af::evt::CoordinateTracked*>(e);
    QString text = "(" + QString::number(ctE->m_x, 'f', 5) + " , " + QString::number(ctE->m_y, 'f', 5) + ")";
    m_coordinateLineEdit->setText(text);
  }
  break;

  case te::qt::af::evt::MAP_SRID_CHANGED:
  {
    te::qt::af::evt::MapSRIDChanged* mEvt = static_cast<te::qt::af::evt::MapSRIDChanged*>(e);

    std::pair<int, std::string> srid = mEvt->m_srid;

    if (srid.first != TE_UNKNOWN_SRS)
    {
      QString sridText(srid.second.c_str());
      sridText += ":" + QString::number(srid.first);
      m_mapSRIDLineEdit->setText(sridText);
    }
    else
    {
      m_mapSRIDLineEdit->setText("Unknown SRS");
      m_coordinateLineEdit->setText("Coordinates");
    }
  }
  break;

  case te::qt::af::evt::LAYER_SELECTED_OBJECTS_CHANGED:
  {
    te::qt::af::evt::LayerSelectedObjectsChanged* lEvt = static_cast<te::qt::af::evt::LayerSelectedObjectsChanged*>(e);
    if (lEvt->m_layer == 0 || lEvt->m_layer->getSelected() == 0)
      return;

    m_selected->setText(tr("Selected rows: ") + QString::number(lEvt->m_layer->getSelected()->size()));
  }
  break;

  default:
    break;
  }
}

void te::qt::af::BaseApplication::onDrawTriggered()
{
//  QApplication::setOverrideCursor(Qt::WaitCursor);

  te::qt::af::evt::DrawButtonClicked drawClicked;
  emit triggered(&drawClicked);

  std::list<te::map::AbstractLayerPtr> ls;

  te::qt::widgets::GetValidLayers(getLayerExplorer()->model(), QModelIndex(), ls);

  m_display->draw(ls);

//  QApplication::restoreOverrideCursor();
}

void te::qt::af::BaseApplication::onZoomInToggled(bool checked)
{
  if (!checked)
    return;

  QCursor zoomAreaCursor(QIcon::fromTheme("zoom-in").pixmap(m_mapCursorSize));

  te::qt::widgets::ZoomArea* zoomArea = new te::qt::widgets::ZoomArea(m_display->getDisplay(), zoomAreaCursor);
  m_display->getDisplay()->setCurrentTool(zoomArea);

  te::qt::af::evt::ZoomInButtonToggled ezoom;
  m_app->trigger(&ezoom);
}

void te::qt::af::BaseApplication::onZoomOutToggled(bool checked)
{
  if (!checked)
    return;

  QCursor zoomOutCursor(QIcon::fromTheme("zoom-out").pixmap(m_mapCursorSize));

  te::qt::widgets::ZoomClick* zoomOut = new te::qt::widgets::ZoomClick(m_display->getDisplay(), zoomOutCursor, 2.0, te::qt::widgets::Zoom::Out);
  m_display->getDisplay()->setCurrentTool(zoomOut);

  te::qt::af::evt::ZoomOutButtonToggled ezoom;
  m_app->trigger(&ezoom);
}

void te::qt::af::BaseApplication::onPreviousExtentTriggered()
{
  m_display->previousExtent();
}

void te::qt::af::BaseApplication::onNextExtentTriggered()
{
  m_display->nextExtent();
}

void te::qt::af::BaseApplication::onPanToggled(bool checked)
{
  if (!checked)
    return;

  te::qt::widgets::Pan* pan = new te::qt::widgets::Pan(m_display->getDisplay(), Qt::OpenHandCursor, Qt::ClosedHandCursor);
  m_display->getDisplay()->setCurrentTool(pan);

  te::qt::af::evt::PanButtonToggled epan;
  m_app->trigger(&epan);
}

void te::qt::af::BaseApplication::onZoomExtentTriggered()
{
  std::list<te::map::AbstractLayerPtr> ls;

  te::qt::widgets::GetValidLayers(getLayerExplorer()->model(), QModelIndex(), ls);

  if(ls.empty())
    return;

  //m_display->fit(m_layerExplorer->getExplorer()->getAllLayers());
  // TODO: Change to get only visible single layers, not selected.
  m_display->fit(ls);

}

void te::qt::af::BaseApplication::onInfoToggled(bool checked)
{
  if (!checked)
    return;

  QPixmap pxmap = QIcon::fromTheme("pointer-info").pixmap(m_mapCursorSize);
  QCursor infoCursor(pxmap, 0, 0);

  te::qt::widgets::Info* info = new te::qt::widgets::Info(m_display->getDisplay(), infoCursor, GetSelectedLayersOnly(getLayerExplorer()));
  m_display->getDisplay()->setCurrentTool(info);

  connect(m_layerExplorer->getExplorer(), SIGNAL(selectedLayersChanged(const std::list<te::map::AbstractLayerPtr>&)), info, SLOT(setLayers(const std::list<te::map::AbstractLayerPtr>&)));
}

void te::qt::af::BaseApplication::onMapRemoveSelectionTriggered()
{
  std::list<te::map::AbstractLayerPtr> layers;

  te::qt::widgets::GetValidLayers(getLayerExplorer()->model(), QModelIndex(), layers);

  std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin();

  while (it != layers.end())
  {
    te::map::AbstractLayerPtr layer = (*it);

    layer->clearSelected();

    ++it;

    te::qt::af::evt::LayerSelectedObjectsChanged e(layer);
    m_app->trigger(&e);
  }
}

void te::qt::af::BaseApplication::onSelectionToggled(bool checked)
{
  if (!checked)
    return;

  te::qt::widgets::Selection* selection = new te::qt::widgets::Selection(m_display->getDisplay(), Qt::ArrowCursor, te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer()));
  m_display->getDisplay()->setCurrentTool(selection);

  connect(m_layerExplorer->getExplorer(), SIGNAL(selectedLayersChanged(const std::list<te::map::AbstractLayerPtr>&)), selection, SLOT(setLayers(const std::list<te::map::AbstractLayerPtr>&)));
  connect(selection, SIGNAL(layerSelectedObjectsChanged(const te::map::AbstractLayerPtr&)), SLOT(onLayerSelectedObjectsChanged(const te::map::AbstractLayerPtr&)));

  te::qt::af::evt::SelectionButtonToggled esel;
  m_app->trigger(&esel);
}

void te::qt::af::BaseApplication::onMapSRIDTriggered()
{
  te::qt::widgets::SRSManagerDialog srsDialog(this);
  srsDialog.setWindowTitle(tr("Choose the SRS"));

  if (srsDialog.exec() == QDialog::Rejected)
    return;

  std::pair<int, std::string> srid = srsDialog.getSelectedSRS();

  te::qt::af::evt::MapSRIDChanged mapSRIDChagned(srid);
  m_app->trigger(&mapSRIDChagned);

  m_display->getDisplay()->setSRID(srid.first);
}

void te::qt::af::BaseApplication::onMapSetUnknwonSRIDTriggered()
{
  std::pair<int, std::string> srid = std::make_pair(TE_UNKNOWN_SRS, "");
  te::qt::af::evt::MapSRIDChanged mapSRIDChagned(srid);
  m_app->trigger(&mapSRIDChagned);

  m_display->getDisplay()->setSRID(TE_UNKNOWN_SRS);
}

void te::qt::af::BaseApplication::onMeasureDistanceToggled(bool checked)
{
  if (!checked)
    return;

  QCursor measureDistanceCursor(QIcon::fromTheme("distance-measure-cursor").pixmap(m_mapCursorSize), 0, 0);

  te::qt::widgets::Measure* distance = new te::qt::widgets::Measure(m_display->getDisplay(), te::qt::widgets::Measure::Distance, measureDistanceCursor);
  m_display->getDisplay()->setCurrentTool(distance);
}

void te::qt::af::BaseApplication::onMeasureAreaToggled(bool checked)
{
  if (!checked)
    return;

  QCursor measureAreaCursor(QIcon::fromTheme("area-measure-cursor").pixmap(m_mapCursorSize), 0, 0);

  te::qt::widgets::Measure* area = new te::qt::widgets::Measure(m_display->getDisplay(), te::qt::widgets::Measure::Area, measureAreaCursor);
  m_display->getDisplay()->setCurrentTool(area);
}

void te::qt::af::BaseApplication::onMeasureAngleToggled(bool checked)
{
  if (!checked)
    return;

  QCursor measureAngleCursor(QIcon::fromTheme("angle-measure-cursor").pixmap(m_mapCursorSize), 0, 0);

  te::qt::widgets::Measure* angle = new te::qt::widgets::Measure(m_display->getDisplay(), te::qt::widgets::Measure::Angle, measureAngleCursor);
  m_display->getDisplay()->setCurrentTool(angle);
}

void te::qt::af::BaseApplication::onStopDrawTriggered()
{
  te::common::ProgressManager::getInstance().cancelTasks(te::common::TaskProgress::DRAW);
}

void te::qt::af::BaseApplication::onScaleComboBoxActivated()
{
  bool ok = false;

  double scale = m_scaleCmbBox->currentText().toDouble(&ok);

  if (ok)
  {
    m_display->getDisplay()->setScale(scale);
    m_display->getDisplay()->refresh(true);
  }
  else
  {
    QVariant var = m_scaleCmbBox->itemData(m_scaleCmbBox->currentIndex());

    if (var.isValid())
    {
      double scale = var.toDouble();

      m_display->getDisplay()->setScale(scale);
      m_display->getDisplay()->refresh(true);
    }
  }
}

void te::qt::af::BaseApplication::onScaleDisplayChanged()
{
  double scale = m_display->getDisplay()->getScale();

  m_scaleCmbBox->setItemText(m_scaleCmbBox->currentIndex(), QString::number(scale, 'f', 0));
}

void te::qt::af::BaseApplication::onInternalSettingsTriggered()
{
  te::qt::af::InternalSettingsDialog isd(this);
  isd.exec();
}

void te::qt::af::BaseApplication::onLayerRemoveTriggered()
{
  QModelIndexList ls = getLayerExplorer()->selectionModel()->selectedIndexes();

  if(ls.isEmpty())
  {
    QString msg = tr("Select at least one item to be removed!");
    QMessageBox::warning(this, tr("Remove item"), msg);

    return;
  }

  QString msg;
  QString questionTitle;

  if(ls.size() == 1)
  {
    msg = tr("Do you really want to remove the selected item?");
    questionTitle = tr("Remove Item");
  }
  else
  {
    msg = tr("Do you really want to remove the selected items?");
    questionTitle = tr("Remove Items");
  }

  int reply = QMessageBox::question(this, questionTitle, msg, QMessageBox::No, QMessageBox::Yes);

  if(reply == QMessageBox::No)
    return;

  std::list<te::map::AbstractLayerPtr> lays = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());
  std::list<te::map::AbstractLayerPtr> lays2;

  te::qt::widgets::GetChangedAndVisibleLayers(ls, lays2);

  getLayerExplorer()->removeSelectedItems();

  bool needRefresh = !lays2.empty();

  for(std::list<te::map::AbstractLayerPtr>::iterator it = lays.begin(); it != lays.end() && !needRefresh; ++it)
    if((*it)->getVisibility() == te::map::VISIBLE)
      needRefresh = true;

  te::qt::af::evt::LayerRemoved e(lays);
  emit triggered(&e);

  if(needRefresh)
  {
    lays.clear();
    te::qt::widgets::GetValidLayers(getLayerExplorer()->model(), QModelIndex(), lays);
    getMapDisplay()->setLayerList(lays);
    getMapDisplay()->refresh();
  }
}

void te::qt::af::BaseApplication::onLayerRenameTriggered()
{
  std::list<te::qt::widgets::TreeItem*> selectedLayerItems = getLayerExplorer()->getSelectedItems();

  if (selectedLayerItems.empty())
    return;
  
  bool renameAllowed = (selectedLayerItems.size() == 1 && (selectedLayerItems.front()->getType() == "LAYER" || selectedLayerItems.front()->getType() == "FOLDER"));

  if(!renameAllowed)
  {
    QString msg = tr("Select only one layer to be renamed!");
    QMessageBox::warning(this, tr("Rename Layer"), msg);

    return;
  }

  te::qt::widgets::TreeItem* item = selectedLayerItems.front();

  bool ok;
  QString text = QInputDialog::getText(this, m_app->getAppTitle(),
                                       tr("Rename Layer:"), QLineEdit::Normal,
                                       item->getAsString().c_str(), &ok);

  if(!ok)
    return;

  if(text.isEmpty())
  {
    QMessageBox::warning(this, m_app->getAppTitle(), tr("Enter the new name!"));
    return;
  }

  if(item->getType() == "LAYER")
    ((te::qt::widgets::LayerItem*)item)->getLayer()->setTitle(text.toUtf8().data());
  else if(item->getType() == "FOLDER")
    ((te::qt::widgets::FolderItem*)item)->setTitle(text.toUtf8().data());

  std::list<te::map::AbstractLayerPtr> ls;

  te::qt::af::evt::LayerRemoved e(ls);
//  e.
  emit triggered(&e);
}

void te::qt::af::BaseApplication::onLayerRemoveItemTriggered()
{
  //std::list<te::qt::widgets::AbstractTreeItem*> selectedItems = m_layerExplorer->getExplorer()->getSelectedItems();

  //std::list<te::qt::widgets::AbstractTreeItem*>::const_iterator it;
  //for (it = selectedItems.begin(); it != selectedItems.end(); ++it)
  //{
  //  te::qt::af::evt::ItemOfLayerRemoved evt((*it));
  //  m_app->trigger(&evt);
  //}
}

void te::qt::af::BaseApplication::onLayerPropertiesTriggered()
{
  std::list<te::map::AbstractLayerPtr> layers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());

  if(layers.empty())
  {
    QMessageBox::warning(this, m_app->getAppTitle(), tr("There's no selected layer."));
    return;
  }

  // Docking
  QDockWidget* doc = new QDockWidget(this, Qt::Dialog);

  te::map::AbstractLayerPtr l = *layers.begin();
  te::qt::widgets::LayerPropertiesInfoWidget* info = new te::qt::widgets::LayerPropertiesInfoWidget(l.get(), doc);

  doc->setWidget(info);
  doc->setWindowTitle(info->windowTitle());
  doc->setAttribute(Qt::WA_DeleteOnClose, true);

  doc->show();
}

void te::qt::af::BaseApplication::onLayerRemoveSelectionTriggered()
{
  std::list<te::map::AbstractLayerPtr> layers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());

  if(layers.empty())
  {
    QMessageBox::warning(this, m_app->getAppTitle(), tr("There's no selected layer."));
    return;
  }

  std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin();

  while(it != layers.end())
  {
    te::map::AbstractLayerPtr layer = (*it);

    if(!layer->isValid())
    {
      ++it;
      continue;
    }

    layer->clearSelected();

    ++it;

    te::qt::af::evt::LayerSelectedObjectsChanged e(layer);
    emit triggered(&e);
  }
}

void te::qt::af::BaseApplication::onLayerInvertSelectionTriggered()
{
  std::list<te::map::AbstractLayerPtr> layers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());

  if (layers.empty())
  {
    QMessageBox::warning(this, m_app->getAppTitle(), tr("There's no selected layer."));
    return;
  }

  std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin();

  while (it != layers.end())
  {
    te::map::AbstractLayerPtr layer = (*it);

    if (!layer->isValid())
    {
      ++it;
      continue;
    }

    std::auto_ptr<te::da::DataSetType> dt = layer->getSchema();
    std::auto_ptr<te::da::DataSet> ds = layer->getData();
    
    te::da::ObjectIdSet* allObjects = te::da::GenerateOIDSet(ds.get(), dt.get());

    const te::da::ObjectIdSet* selected = layer->getSelected();

    if(selected && (selected->size() > 0))
      allObjects->difference(selected);

    layer->clearSelected();

    layer->select(allObjects);

    ++it;

    te::qt::af::evt::LayerSelectedObjectsChanged e(layer);
    emit triggered(&e);
  }
}

void te::qt::af::BaseApplication::onLayerSRSTriggered()
{
  std::list<te::map::AbstractLayerPtr> layers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());

  if(layers.empty())
  {
    QMessageBox::warning(this, m_app->getAppTitle(), tr("There's no selected layer."));
    return;
  }
  te::qt::widgets::SRSManagerDialog srsDialog(this);
  srsDialog.setWindowTitle(tr("Choose the SRS"));

  if(srsDialog.exec() == QDialog::Rejected)
    return;

  std::pair<int, std::string> srid = srsDialog.getSelectedSRS();

  for (std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin(); it != layers.end(); ++it)
  {
    te::map::AbstractLayerPtr lay = *it;

    lay->setSRID(srid.first);
  }
}

void te::qt::af::BaseApplication::onLayerFitOnMapDisplayTriggered()
{
  try
  {
    std::list<te::map::AbstractLayerPtr> selectedLayers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());

    if(selectedLayers.empty())
    {
      QMessageBox::warning(this, m_app->getAppTitle(),
                           tr("Select a layer in the layer explorer!"));
      return;
    }
    else
    {
      std::list<te::map::AbstractLayerPtr>::iterator it = selectedLayers.begin();

      while(it != selectedLayers.end())
      {
        if(!it->get()->isValid())
        {
          QMessageBox::warning(this, m_app->getAppTitle(),
                               tr("There are invalid layers selected!"));

          return;
        }

        ++it;
      }
    }

    // The layer fitting will be accomplished only on the first layer selected
    te::map::AbstractLayerPtr selectedLayer = *(selectedLayers.begin());

    te::gm::Envelope env = selectedLayer->getExtent();

    te::qt::widgets::MapDisplay* display = m_display->getDisplay();

    if(display->getLayerList().empty())
    {
      std::list<te::map::AbstractLayerPtr> ls;
      te::qt::widgets::GetValidLayers(getLayerExplorer()->model(), QModelIndex(), ls);

      display->setLayerList(ls);
    }

    if((display->getSRID() == TE_UNKNOWN_SRS && selectedLayer->getSRID() == TE_UNKNOWN_SRS) || (display->getSRID() == selectedLayer->getSRID()))
    {
      display->setExtent(env, true);
      return;
    }

    if(display->getSRID() == TE_UNKNOWN_SRS && selectedLayer->getSRID() != TE_UNKNOWN_SRS)
    {
      display->setSRID(selectedLayer->getSRID());
      display->setExtent(env, true);
      return;
    }

    if(display->getSRID() == TE_UNKNOWN_SRS || selectedLayer->getSRID() == TE_UNKNOWN_SRS)
    {
      QMessageBox::warning(this, m_app->getAppTitle(), tr("The spatial reference system of the map display and the layer are not compatible!"));
      return;
    }

    if(display->getSRID() != selectedLayer->getSRID())
      env.transform(selectedLayer->getSRID(), display->getSRID());

    display->setExtent(env, true);
  }
  catch(const std::exception& e)
  {
    QMessageBox::warning(this, m_app->getAppTitle(), e.what());
  }

}

void te::qt::af::BaseApplication::onLayerFitSelectedOnMapDisplayTriggered()
{
  std::list<te::map::AbstractLayerPtr> selectedLayers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());
  if(selectedLayers.empty())
  {
    QString msg = tr("Select at least a layer to accomplish this operation!");
    QMessageBox::warning(this, m_app->getAppTitle(), msg);
    return;
  }
  else
  {
    std::list<te::map::AbstractLayerPtr>::iterator it = selectedLayers.begin();

    while(it != selectedLayers.end())
    {
      if(!it->get()->isValid())
      {
        QMessageBox::warning(this, m_app->getAppTitle(),
                             tr("There are invalid layers selected!"));
        return;
      }

      ++it;
    }
  }

  te::gm::Envelope finalEnv = te::map::GetSelectedExtent(selectedLayers, m_display->getDisplay()->getSRID(), false);

  if(!finalEnv.isValid())
  {
    QString msg = tr("Select object(s) in the selected layer(s) to accomplish this operation!");
    QMessageBox::warning(this, m_app->getAppTitle(), msg);
    return;
  }

  m_display->getDisplay()->setExtent(finalEnv, true);
}

void te::qt::af::BaseApplication::onLayerPanToSelectedOnMapDisplayTriggered()
{
  std::list<te::map::AbstractLayerPtr> selectedLayers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());
  if(selectedLayers.empty())
  {
    QString msg = tr("Select at least a layer to accomplish this operation!");
    QMessageBox::warning(this, m_app->getAppTitle(), msg);
    return;
  }
  else
  {
    std::list<te::map::AbstractLayerPtr>::iterator it = selectedLayers.begin();

    while(it != selectedLayers.end())
    {
      if(!it->get()->isValid())
      {
        QMessageBox::warning(this, m_app->getAppTitle(),
                             tr("There are invalid layers selected!"));
        return;
      }

      ++it;
    }
  }

  te::map::MapDisplay* display = m_display->getDisplay();

  te::gm::Envelope selectedExtent = te::map::GetSelectedExtent(selectedLayers, display->getSRID(), true);

  te::gm::Coord2D centerOfSelectedExtent = selectedExtent.getCenter();

  te::gm::Envelope displayExtent = display->getExtent();

  double halfWidth = displayExtent.getWidth() * 0.5;
  double halfHeight = displayExtent.getHeight() * 0.5;

  te::gm::Envelope newExtent;

  newExtent.m_llx = centerOfSelectedExtent.x - halfWidth;
  newExtent.m_lly = centerOfSelectedExtent.y - halfHeight;

  newExtent.m_urx = centerOfSelectedExtent.x + halfWidth;
  newExtent.m_ury = centerOfSelectedExtent.y + halfHeight;

  display->setExtent(newExtent);
}

void te::qt::af::BaseApplication::onLayerSaveSelectedObjectsTriggered()
{
  try
  {
    std::list<te::map::AbstractLayerPtr> selectedLayers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());

    if (selectedLayers.empty())
    {
      QString msg = tr("Select one layer to accomplish this operation!");
      QMessageBox::warning(this, m_app->getAppTitle(), msg);
      return;
    }
    else
    {
      std::list<te::map::AbstractLayerPtr>::iterator it = selectedLayers.begin();

      if (!it->get()->isValid())
      {
        QMessageBox::warning(this, m_app->getAppTitle(),
          tr("This is a invalid layer selected!"));
        return;
      }
    }

    te::map::AbstractLayerPtr selectedLayer = *selectedLayers.begin();

    te::qt::widgets::SaveSelectedObjectsDialog dlg(this);

    dlg.setWindowTitle(dlg.windowTitle() + " (" + tr("Layer") + ":" + selectedLayer->getTitle().c_str() + ")");

    dlg.setParameters(selectedLayer);

    if (dlg.exec() != QDialog::Accepted)
      return;

    te::map::AbstractLayerPtr layer = dlg.getLayer();

    if (!layer)
      return;

    te::qt::af::evt::LayerAdded evt(layer.get());
    emit triggered(&evt);

  }
  catch(const std::exception& e)
  {
    QMessageBox::warning(this, m_app->getAppTitle(), e.what());
  }

}

void te::qt::af::BaseApplication::onLayerCharEncodingHovered()
{
  std::list<te::map::AbstractLayerPtr> selectedLayers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());

  if (!selectedLayers.empty())
  {
    std::list<te::map::AbstractLayerPtr>::iterator it = selectedLayers.begin();

    m_encodingMenu->setLayer(*it);
  }
}


void te::qt::af::BaseApplication::onFullScreenToggled(bool checked)
{
  checked ? showFullScreen() : showMaximized();
}

void te::qt::af::BaseApplication::onLayerExplorerVisibilityChanged(bool visible)
{
  m_viewLayerExplorer->blockSignals(true);
  m_viewLayerExplorer->setChecked(visible);
  m_viewLayerExplorer->blockSignals(false);
}

void te::qt::af::BaseApplication::onDisplayDataTableChanged(bool visible)
{
  if (m_tables.empty())
    return;

  for (std::size_t i = 0; i < m_tables.size(); ++i)
  {
    if (visible)
      m_tables[i]->show();
    else
      m_tables[i]->hide();
  }

  m_viewDataTable->setChecked(visible);
}

void te::qt::af::BaseApplication::onStyleExplorerVisibilityChanged(bool visible)
{
  m_viewStyleExplorer->blockSignals(true);
  m_viewStyleExplorer->setChecked(visible);
  m_viewStyleExplorer->blockSignals(false);
}

void te::qt::af::BaseApplication::onLayerShowTableTriggered()
{
  std::list<te::map::AbstractLayerPtr> layers = te::qt::widgets::GetSelectedLayersOnly(getLayerExplorer());

  if(layers.empty())
  {
    QMessageBox::warning(this, m_app->getAppTitle(), tr("There's no selected layer."));
    return;
  }
  else
  {
    for(std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin(); it != layers.end(); ++it)
    {
      if(!(*it)->isValid())
      {
        QMessageBox::warning(this, m_app->getAppTitle(), tr("There are invalid layers selected!"));
        return;
      }
    }
  }


  te::map::AbstractLayerPtr lay = *layers.begin();

  if(lay->getSchema()->hasRaster())
    return;

  te::qt::af::DataSetTableDockWidget* doc = GetLayerDock(lay.get(), m_tables);

  if(doc == 0)
  {
    doc = new te::qt::af::DataSetTableDockWidget(this);
    doc->setLayer(lay.get());
    doc->setHighlightColor(m_app->getSelectionColor());
    addDockWidget(Qt::BottomDockWidgetArea, doc);

    connect(doc, SIGNAL(closed(te::qt::af::DataSetTableDockWidget*)), SLOT(onLayerTableClose(te::qt::af::DataSetTableDockWidget*)));
    connect(doc, SIGNAL(createChartDisplay(te::qt::widgets::ChartDisplayWidget*, te::map::AbstractLayer*)), SLOT(onChartDisplayCreated(te::qt::widgets::ChartDisplayWidget*, te::map::AbstractLayer*)));

    if(!m_tables.empty())
      tabifyDockWidget(m_tables[m_tables.size() - 1], doc);

    m_tables.push_back(doc);

    m_app->addListener(doc);
  }

  doc->show();
  doc->raise();

  m_viewDataTable->setChecked(true);

  m_viewDataTable->setEnabled(true);
}

void te::qt::af::BaseApplication::onLayerTableClose(te::qt::af::DataSetTableDockWidget* wid)
{
  std::vector<te::qt::af::DataSetTableDockWidget*>::iterator it;

  for (it = m_tables.begin(); it != m_tables.end(); ++it)
    if (*it == wid)
      break;

  if (it != m_tables.end())
  {
    m_app->removeListener(*it);
    m_tables.erase(it);
  }
}

void te::qt::af::BaseApplication::onChartDisplayCreated(te::qt::widgets::ChartDisplayWidget* chartDisplay, te::map::AbstractLayer* layer)
{
  try
  {
    te::qt::af::ChartDisplayDockWidget* doc = new te::qt::af::ChartDisplayDockWidget(chartDisplay, this);
    doc->setSelectionColor(m_app->getSelectionColor());
    doc->setWindowTitle("Histogram");
    doc->setWindowIcon(QIcon::fromTheme("chart-bar"));
    doc->setLayer(layer);
    doc->setAppController(m_app);

    m_app->addListener(doc);
    addDockWidget(Qt::RightDockWidgetArea, doc, Qt::Horizontal);
    doc->show();
  }
  catch (const std::exception& e)
  {
    QMessageBox::warning(this, m_app->getAppTitle(), e.what());
  }
}

void te::qt::af::BaseApplication::onLayerSelectionChanged(const te::map::AbstractLayerPtr& layer)
{
  te::qt::af::evt::LayerSelectedObjectsChanged e(layer);
  emit triggered(&e);
}

void te::qt::af::BaseApplication::onLayerSelectedObjectsChanged(const te::map::AbstractLayerPtr& layer)
{
  assert(layer.get());

  te::qt::af::evt::LayerSelectedObjectsChanged e(layer);
  m_app->trigger(&e);
}

void te::qt::af::BaseApplication::makeDialog()
{
  QMainWindow::setCentralWidget(m_ui->m_display);

  //start menu 
  initMenus();

  //start actions
  initActions();

  //start main components
  m_layerExplorer = new LayerExplorer(m_ui->m_layerExplorer);
  m_display = new MapDisplay(m_ui->m_display, m_app);
  m_styleExplorer = new StyleExplorer(m_ui->m_styleExplorer, m_app);

  initSlotsConnections();

  initStatusBar();

  initToolbars();

  m_viewLayerExplorer->setChecked(true);
  m_display->getDisplay()->setResizePolicy(te::qt::widgets::MapDisplay::Center);
  m_viewStyleExplorer->setChecked(false);
  m_styleExplorer->getExplorer()->setVisible(false);

  //define startup tool
  onSelectionToggled(true);
  m_mapSelection->setChecked(true);

  //connect components
  m_app->addListener(m_layerExplorer);
  m_app->addListener(m_display);
  m_app->addListener(m_styleExplorer);
  m_app->addListener(this);

  m_display->getDisplay()->connect(m_mapStopDrawing, SIGNAL(triggered()), SLOT(onDrawCanceled()));

  //char encoding
  m_encodingMenu = new te::qt::widgets::CharEncodingMenuWidget();
  m_layerCharEncoding->setMenu(m_encodingMenu->getMenu());

  //set app info
  setWindowTitle(m_app->getAppTitle());
  setWindowIcon(QIcon(m_app->getAppIconName()));
  setWindowIconText(m_app->getAppTitle());
}

void te::qt::af::BaseApplication::initFramework(const QString& cfgFile)
{
  m_app = &AppCtrlSingleton::getInstance();
  m_app->setConfigFile(cfgFile.toUtf8().data());
  m_app->initialize();

  //check user settings file
  QSettings user_settings(QSettings::IniFormat,
    QSettings::UserScope,
    QApplication::instance()->organizationName(),
    QApplication::instance()->applicationName());

  QFileInfo info(user_settings.fileName());

  if (!info.exists())
    createDefaultSettings();
}

void te::qt::af::BaseApplication::initStatusBar()
{
  // Status Bar
  m_statusbar = new QStatusBar(this);
  m_statusbar->setObjectName("StatusBar");
  setStatusBar(m_statusbar);

  // Selected status
  m_selected = new QLabel(m_statusbar);
  m_selected->setText(tr("Selected rows: 0"));
  m_statusbar->addPermanentWidget(m_selected);

  // Map SRID reset action
  QToolButton* mapUnknownSRIDToolButton = new QToolButton(m_statusbar);
  mapUnknownSRIDToolButton->setDefaultAction(m_mapUnknownSRID);
  m_statusbar->addPermanentWidget(mapUnknownSRIDToolButton);

  // Map SRID action
  QToolButton* mapSRIDToolButton = new QToolButton(m_statusbar);
  mapSRIDToolButton->setDefaultAction(m_mapSRID);
  m_statusbar->addPermanentWidget(mapSRIDToolButton);

  // Map SRID information
  m_mapSRIDLineEdit = new QLineEdit(m_statusbar);
  m_mapSRIDLineEdit->setFixedWidth(120);
  m_mapSRIDLineEdit->setAlignment(Qt::AlignHCenter);
  m_mapSRIDLineEdit->setEnabled(false);

  int srid = m_display->getDisplay()->getSRID();
  srid != TE_UNKNOWN_SRS ? m_mapSRIDLineEdit->setText("EPSG:" + QString::number(srid)) : m_mapSRIDLineEdit->setText(tr("Unknown SRS"));
  m_statusbar->addPermanentWidget(m_mapSRIDLineEdit);

  // Coordinate Line Edit
  m_coordinateLineEdit = new QLineEdit(m_statusbar);
  m_coordinateLineEdit->setFixedWidth(220);
  m_coordinateLineEdit->setAlignment(Qt::AlignHCenter);
  m_coordinateLineEdit->setReadOnly(true);
  m_coordinateLineEdit->setFocusPolicy(Qt::NoFocus);
  m_coordinateLineEdit->setText(tr("Coordinates"));
  m_statusbar->addPermanentWidget(m_coordinateLineEdit);

  // Stop draw action
  QToolButton* stopDrawToolButton = new QToolButton(m_statusbar);
  stopDrawToolButton->setDefaultAction(m_mapStopDrawing);
  m_statusbar->addPermanentWidget(stopDrawToolButton);

  // Scale Combo Box
  m_scaleCmbBox = new QComboBox(m_statusbar);
  m_scaleCmbBox->setToolTip(tr("Scale Information"));
  m_scaleCmbBox->setEditable(true);
  m_scaleCmbBox->setInsertPolicy(QComboBox::NoInsert);
  m_scaleCmbBox->setMinimumWidth(100);
  m_statusbar->addPermanentWidget(m_scaleCmbBox);

  m_scaleCmbBox->addItem("Scale");
  m_scaleCmbBox->addItem("1 000", QVariant((double)1000));
  m_scaleCmbBox->addItem("1 500", QVariant((double)1500));
  m_scaleCmbBox->addItem("2 000", QVariant((double)2000));
  m_scaleCmbBox->addItem("2 500", QVariant((double)2500));
  m_scaleCmbBox->addItem("5 000", QVariant((double)5000));
  m_scaleCmbBox->addItem("7 500", QVariant((double)7500));
  m_scaleCmbBox->addItem("10 000", QVariant((double)10000));
  m_scaleCmbBox->addItem("12 500", QVariant((double)12500));
  m_scaleCmbBox->addItem("15 000", QVariant((double)15000));
  m_scaleCmbBox->addItem("20 000", QVariant((double)20000));
  m_scaleCmbBox->addItem("25 000", QVariant((double)25000));
  m_scaleCmbBox->addItem("50 000", QVariant((double)50000));
  m_scaleCmbBox->addItem("100 000", QVariant((double)100000));
  m_scaleCmbBox->addItem("125 000", QVariant((double)125000));
  m_scaleCmbBox->addItem("150 000", QVariant((double)150000));
  m_scaleCmbBox->addItem("175 000", QVariant((double)175000));
  m_scaleCmbBox->addItem("200 000", QVariant((double)200000));
  m_scaleCmbBox->addItem("250 000", QVariant((double)250000));
  m_scaleCmbBox->addItem("500 000", QVariant((double)500000));
  m_scaleCmbBox->addItem("750 000", QVariant((double)750000));
  m_scaleCmbBox->addItem("1 000 000", QVariant((double)1000000));
  m_scaleCmbBox->addItem("1 250 000", QVariant((double)1250000));
  m_scaleCmbBox->addItem("1 500 000", QVariant((double)1500000));
  m_scaleCmbBox->addItem("1 750 000", QVariant((double)1750000));
  m_scaleCmbBox->addItem("2 000 000", QVariant((double)2000000));

  connect(m_scaleCmbBox, SIGNAL(activated(int)), this, SLOT(onScaleComboBoxActivated()));
  connect(m_scaleCmbBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(onScaleComboBoxActivated()));

  if (m_display->getDisplay())
    connect(m_display->getDisplay(), SIGNAL(extentChanged()), SLOT(onScaleDisplayChanged()));

  //internal settings
  QToolButton* internalSettingsToolButton = new QToolButton(m_statusbar);
  internalSettingsToolButton->setDefaultAction(m_internalSettings);
  m_statusbar->addPermanentWidget(internalSettingsToolButton);
}

void te::qt::af::BaseApplication::initActions()
{
  initAction(m_viewLayerExplorer, "view-layer-explorer", "View.Layer Explorer", tr("&Layer Explorer"), tr("Show or hide the layer explorer"), true, true, true, m_menubar);
  initAction(m_viewDataTable, "view-data-table", "View.Data Table", tr("&Data Table"), tr("Show or hide the data table"), true, true, true, m_menubar);
  initAction(m_viewStyleExplorer, "style", "View.Style Explorer", tr("&Style Explorer"), tr("Show or hide the style explorer"), true, true, true, m_menubar);
  initAction(m_viewFullScreen, "view-fullscreen", "View.Full Screen", tr("F&ull Screen"), tr(""), true, true, true, m_menubar);

  initAction(m_layerShowTable, "view-data-table", "Layer.Show Table", tr("S&how Table"), tr(""), true, false, true, m_menubar);
  initAction(m_layerRemove, "layer-remove", "Project.Remove Layer", tr("&Remove Item(s)"), tr("Remove items(s) from the project"), true, false, true, this);
  initAction(m_layerRename, "layer-rename", "Project.Rename Layer", tr("Rename Layer..."), tr("Rename layer"), true, false, true, this);
  initAction(m_layerRemoveObjectSelection, "pointer-remove-selection", "Layer.Remove Selection", tr("&Remove Selection"), tr(""), true, false, true, m_menubar);
  initAction(m_layerInvertObjectSelection, "pointer-invert-selection", "Layer.Invert Selection", tr("&Invert Selection"), tr(""), true, false, true, m_menubar);
  initAction(m_layerRemoveItem, "item-remove", "Layer.Remove Item", tr("&Remove Item"), tr(""), true, false, true, m_menubar);
  initAction(m_layerProperties, "layer-info", "Layer.Properties", tr("&Properties..."), tr(""), true, false, true, m_menubar);
  initAction(m_layerSRS, "layer-srs", "Layer.SRS", tr("&Inform SRS..."), tr(""), true, false, true, m_menubar);
  initAction(m_layerFitOnMapDisplay, "layer-fit", "Layer.Fit Layer on the Map Display", tr("Fit Layer"), tr("Fit the current layer on the Map Display"), true, false, true, m_menubar);
  initAction(m_layerFitSelectedOnMapDisplay, "zoom-selected-extent", "Layer.Fit Selected Features on the Map Display", tr("Fit Selected Features"), tr("Fit the selected features on the Map Display"), true, false, true, m_menubar);
  initAction(m_layerPanToSelectedOnMapDisplay, "pan-selected", "Layer.Pan to Selected Features on Map Display", tr("Pan to Selected Features"), tr("Pan to the selected features on the Map Display"), true, false, true, m_menubar);
  initAction(m_layerSaveSelectedObjects, "layer-save-selected-obj", "Layer.Save Selected Objects", tr("Save Selected Objects..."), tr("Save a new layer based on the selected objects from this layer"), true, false, true, this);
  initAction(m_layerCharEncoding, "char-encoding", "Layer.Encoding", tr("&Encoding..."), tr("Set the encoding to load data from selected layer"), true, false, true, m_menubar);

  initAction(m_mapDraw, "map-draw", "Map.Draw", tr("&Draw"), tr("Draw the visible layers"), true, false, true, m_menubar);
  initAction(m_mapZoomIn, "zoom-in", "Map.Zoom In", tr("Zoom &In"), tr(""), true, true, true, m_menubar);
  initAction(m_mapZoomOut, "zoom-out", "Map.Zoom Out", tr("Zoom &Out"), tr(""), true, true, true, m_menubar);
  initAction(m_mapPan, "pan", "Map.Pan", tr("&Pan"), tr(""), true, true, true, m_menubar);
  initAction(m_mapZoomExtent, "zoom-extent", "Map.Zoom Extent", tr("Zoom &Extent"), tr(""), true, false, true, m_menubar);
  initAction(m_mapPreviousExtent, "edit-undo", "Map.Previous Extent", tr("&Previous Extent"), tr(""), true, false, false, m_menubar);
  initAction(m_mapNextExtent, "edit-redo", "Map.Next Extent", tr("&Next Extent"), tr(""), true, false, false, m_menubar);
  initAction(m_mapInfo, "pointer-info", "Map.Info", tr("&Info"), tr(""), true, true, true, m_menubar);
  initAction(m_mapRemoveSelection, "pointer-remove-selection", "Map.Remove Selection", tr("&Remove Selection"), tr(""), true, false, true, m_menubar);
  initAction(m_mapSelection, "pointer-selection", "Map.Selection", tr("&Selection"), tr(""), true, true, true, m_menubar);
  initAction(m_mapMeasureDistance, "distance-measure", "Map.Measure Distance", tr("Measure &Distance"), tr(""), true, true, true, m_menubar);
  initAction(m_mapMeasureArea, "area-measure", "Map.Measure Area", tr("Measure &Area"), tr(""), true, true, true, m_menubar);
  initAction(m_mapMeasureAngle, "angle-measure", "Map.Measure Angle", tr("Measure &Angle"), tr(""), true, true, true, m_menubar);

  initAction(m_mapSRID, "srs", "Map.SRID", tr("&SRS..."), tr("Config the Map SRS"), true, false, true, m_menubar);
  initAction(m_mapUnknownSRID, "srs-unknown", "Map.UnknownSRID", tr("&Set Unknown SRS"), tr("Set the Map SRS to unknown"), true, false, true, m_menubar);
  initAction(m_mapStopDrawing, "map-draw-cancel", "Map.Stop Drawing", tr("&Stop Drawing"), tr("Stop all drawing tasks"), true, false, true, m_menubar);
  initAction(m_internalSettings, "gear", "Map.Internal Settings", tr("&Internal Settings"), tr("Application Internal Settings"), true, false, true, m_menubar);

  m_layerRename->setShortcut(QKeySequence(Qt::Key_F2));

  // Group the map tools
  QActionGroup* mapToolsGroup = new QActionGroup(this->menuBar());
  mapToolsGroup->setObjectName("Map.ToolsGroup");
  mapToolsGroup->addAction(m_mapZoomIn);
  mapToolsGroup->addAction(m_mapZoomOut);
  mapToolsGroup->addAction(m_mapPan);
  mapToolsGroup->addAction(m_mapInfo);
  mapToolsGroup->addAction(m_mapSelection);
  mapToolsGroup->addAction(m_mapRemoveSelection);
  mapToolsGroup->addAction(m_mapMeasureDistance);
  mapToolsGroup->addAction(m_mapMeasureArea);
  mapToolsGroup->addAction(m_mapMeasureAngle);
}

void te::qt::af::BaseApplication::initMenus()
{
  m_menubar = new QMenuBar(this);
  m_menubar->setObjectName("menubar");
  m_menubar->setGeometry(QRect(0, 0, 640, 21));

  setMenuBar(m_menubar);

  m_app->registerMenuBar(m_menubar);
}

void te::qt::af::BaseApplication::initToolbars()
{
  std::vector<QToolBar*> bars = te::qt::af::ReadToolBarsFromSettings(m_app, this);
  std::vector<QToolBar*>::iterator it;

  for (it = bars.begin(); it != bars.end(); ++it)
  {
    QToolBar* bar = *it;
    addToolBar(Qt::TopToolBarArea, bar);
    m_app->registerToolBar(bar->objectName(), bar);
  }
}

void te::qt::af::BaseApplication::initSlotsConnections()
{
  connect(m_mapDraw, SIGNAL(triggered()), SLOT(onDrawTriggered()));
  connect(m_mapZoomIn, SIGNAL(toggled(bool)), SLOT(onZoomInToggled(bool)));
  connect(m_mapZoomOut, SIGNAL(toggled(bool)), SLOT(onZoomOutToggled(bool)));
  connect(m_mapPreviousExtent, SIGNAL(triggered()), SLOT(onPreviousExtentTriggered()));
  connect(m_mapNextExtent, SIGNAL(triggered()), SLOT(onNextExtentTriggered()));
  connect(m_mapPan, SIGNAL(toggled(bool)), SLOT(onPanToggled(bool)));
  connect(m_mapZoomExtent, SIGNAL(triggered()), SLOT(onZoomExtentTriggered()));
  connect(m_mapInfo, SIGNAL(toggled(bool)), SLOT(onInfoToggled(bool)));
  connect(m_mapRemoveSelection, SIGNAL(triggered()), SLOT(onMapRemoveSelectionTriggered()));
  connect(m_mapSelection, SIGNAL(toggled(bool)), SLOT(onSelectionToggled(bool)));
  connect(m_mapMeasureDistance, SIGNAL(toggled(bool)), SLOT(onMeasureDistanceToggled(bool)));
  connect(m_mapMeasureArea, SIGNAL(toggled(bool)), SLOT(onMeasureAreaToggled(bool)));
  connect(m_mapMeasureAngle, SIGNAL(toggled(bool)), SLOT(onMeasureAngleToggled(bool)));

  connect(m_layerRemove, SIGNAL(triggered()), SLOT(onLayerRemoveTriggered()));
  connect(m_layerRename, SIGNAL(triggered()), SLOT(onLayerRenameTriggered()));
  connect(m_layerProperties, SIGNAL(triggered()), SLOT(onLayerPropertiesTriggered()));
  connect(m_layerInvertObjectSelection, SIGNAL(triggered()), SLOT(onLayerInvertSelectionTriggered()));
  connect(m_layerRemoveObjectSelection, SIGNAL(triggered()), SLOT(onLayerRemoveSelectionTriggered()));
  connect(m_layerRemoveItem, SIGNAL(triggered()), SLOT(onLayerRemoveItemTriggered()));
  connect(m_layerSRS, SIGNAL(triggered()), SLOT(onLayerSRSTriggered()));
  connect(m_layerFitOnMapDisplay, SIGNAL(triggered()), SLOT(onLayerFitOnMapDisplayTriggered()));
  connect(m_layerFitSelectedOnMapDisplay, SIGNAL(triggered()), SLOT(onLayerFitSelectedOnMapDisplayTriggered()));
  connect(m_layerPanToSelectedOnMapDisplay, SIGNAL(triggered()), SLOT(onLayerPanToSelectedOnMapDisplayTriggered()));
  connect(m_layerShowTable, SIGNAL(triggered()), SLOT(onLayerShowTableTriggered()));
  connect(m_layerSaveSelectedObjects, SIGNAL(triggered()), SLOT(onLayerSaveSelectedObjectsTriggered()));
  connect(m_layerCharEncoding, SIGNAL(hovered()), SLOT(onLayerCharEncodingHovered()));

  connect(m_mapSRID, SIGNAL(triggered()), SLOT(onMapSRIDTriggered()));
  connect(m_mapUnknownSRID, SIGNAL(triggered()), SLOT(onMapSetUnknwonSRIDTriggered()));
  connect(m_mapStopDrawing, SIGNAL(triggered()), SLOT(onStopDrawTriggered()));
  connect(m_internalSettings, SIGNAL(triggered()), SLOT(onInternalSettingsTriggered()));

  connect(m_viewFullScreen, SIGNAL(toggled(bool)), SLOT(onFullScreenToggled(bool)));

  connect(m_viewLayerExplorer, SIGNAL(toggled(bool)), m_ui->m_layerExplorerDockWidget, SLOT(setVisible(bool)));
//  connect(m_layerExplorer->getExplorer(), SIGNAL(visibilityChanged(bool)), this, SLOT(onLayerExplorerVisibilityChanged(bool)));

  connect(m_display, SIGNAL(hasPreviousExtent(bool)), m_mapPreviousExtent, SLOT(setEnabled(bool)));
  connect(m_display, SIGNAL(hasNextExtent(bool)), m_mapNextExtent, SLOT(setEnabled(bool)));

  connect(m_viewStyleExplorer, SIGNAL(toggled(bool)), m_styleExplorer->getExplorer(), SLOT(setVisible(bool)));
  connect(m_styleExplorer->getExplorer(), SIGNAL(visibilityChanged(bool)), this, SLOT(onStyleExplorerVisibilityChanged(bool)));
  connect(m_styleExplorer->getExplorer(), SIGNAL(repaintMapDisplay()), this, SLOT(onDrawTriggered()));

  connect(m_viewDataTable, SIGNAL(toggled(bool)), this, SLOT(onDisplayDataTableChanged(bool)));
}

void te::qt::af::BaseApplication::initAction(QAction*& act, const QString& icon, const QString& name,
  const QString& text, const QString& tooltip,
  bool iconVisibleInMenu, bool isCheckable,
  bool enabled, QObject* parent)
{
  act = new QAction(parent);

  if (!icon.isEmpty())
    act->setIcon(QIcon::fromTheme(icon));

  act->setObjectName(name);
  act->setText(text);
  act->setIconVisibleInMenu(iconVisibleInMenu);
  act->setCheckable(isCheckable);
  act->setEnabled(enabled);

#ifndef QT_NO_TOOLTIP
  act->setToolTip(tooltip);
#endif
}
