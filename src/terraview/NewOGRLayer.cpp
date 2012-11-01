#include "NewOGRLayer.h"

//! Terralib include files
#include <terralib/qt/widgets/utils/FileChooser.h>
#include <terralib/maptools/Layer.h>
//#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/datasource/DataSourceManager.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceCatalogLoader.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/se.h>

#include <terralib/maptools/LayerRenderer.h>

//! Qt include files
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileInfo>
#include <QMessageBox>

namespace te
{
  namespace gm
  {
    class Envelope;
  }
}

std::string getDataSetName(te::da::DataSource* ds, te::gm::Envelope*& box) 
{
// get a transactor to interact to the data source
  te::da::DataSourceTransactor* transactor = ds->getTransactor();

// from transactor, take a catalog loader to find out the datasets stored in the data source
  te::da::DataSourceCatalogLoader* cloader = transactor->getCatalogLoader();
  cloader->loadCatalog();

// now retrieve the name of the datasets
  boost::ptr_vector<std::string> datasets;

  cloader->getDataSets(datasets);

  assert(!datasets.empty());

  // Gets DataSet Type
  te::da::DataSetType* dt = cloader->getDataSetType(datasets[0]);

  // Default geometry property
  te::gm::GeometryProperty* geomProperty = dt->getDefaultGeomProperty();

  std::string datasetName = datasets[0];

//  te::da::DataSet* dataset = transactor->getDataSet(datasetName);

  box = cloader->getExtent(geomProperty);

  delete dt;

  delete cloader;

  delete transactor;

  return datasetName;
}


te::map::Layer* NewOGRLayer::getNewLayer(QWidget* parent)
{
  NewOGRLayer dlg(parent);

  if(dlg.exec() == QDialog::Accepted)
  {
    QString f = dlg.m_fc->getSelectedResource();
    QFileInfo info(f);

    if(!info.exists() || !info.isFile())
      QMessageBox::information(&dlg, tr("Fail to open shape file"), tr("Selected resource is not valid."));
    else
    {
      te::da::DataSourceManager& man = te::da::DataSourceManager::getInstance();
      QString id("ds_");
      id += QString::number(man.size());
      std::map<std::string, std::string> connInfo;
      connInfo["SOURCE"] = f.toLatin1().data();  

      te::da::DataSource* ds = man.get(id.toLatin1().data(), "OGR", connInfo);

      if(!ds->isOpened())
        ds->open(connInfo);
      
      if(ds->isValid() && ds->isOpened())
      {
        te::gm::Envelope* env = 0;

        std::string dset = getDataSetName(ds, env);

        std::auto_ptr<te::map::Layer> layer(new te::map::Layer(id.toLatin1().data(), info.baseName().toLatin1().data()));

        layer->setDataSetName(dset);
        layer->setDataSource(ds);

        // Creates a hard-coded style
        te::se::PolygonSymbolizer* symbolizer = new te::se::PolygonSymbolizer;
        symbolizer->setFill(te::se::CreateFill("#339966", "1.0"));
        symbolizer->setStroke(te::se::CreateStroke("#000000", "3", "1.0"));

        te::se::Rule* rule = new te::se::Rule;
        rule->push_back(symbolizer);

        te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
        style->push_back(rule);

        layer->setStyle(style);

        layer->setRenderer(new te::map::LayerRenderer());
        layer->setExtent(env);

        return layer.release();
      }
    }
  }

  return 0;
}

NewOGRLayer::NewOGRLayer(QWidget* parent) :
QDialog(parent)
{
  makeDialog();

  setWindowTitle(tr("Add an OGR layer"));
}

NewOGRLayer::~NewOGRLayer()
{
}

void NewOGRLayer::makeDialog()
{
  m_fc = new te::qt::widgets::FileChooser(this);
  m_fc->setFilterPattern(tr("Shape Files (*.shp *.SHP)"));

  QPushButton* ok_btn = new QPushButton(tr("&Ok"), this);
  QPushButton* can_btn = new QPushButton(tr("&Cancel"), this);

  QHBoxLayout* hLay = new QHBoxLayout;
  hLay->addWidget(ok_btn);
  hLay->addWidget(can_btn);
  hLay->insertSpacing(0, 10);

  QVBoxLayout* vLay = new QVBoxLayout;
  vLay->addWidget(m_fc);
  vLay->addLayout(hLay);
  vLay->insertSpacing(1, 10);

  QGridLayout* grid = new QGridLayout(this);
  grid->addLayout(vLay, 0, 0, 1, 1);

  connect(ok_btn, SIGNAL(pressed()), SLOT(accept()));
  connect(can_btn, SIGNAL(pressed()), SLOT(reject()));
}