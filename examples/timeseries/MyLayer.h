#ifndef __MY_LAYER_H_
#define __MY_LAYER_H_

#include <terralib/maptools.h>

#include <terralib/qt/qwt/Plot.h>

class MyGrid;

class MyLayer : public te::map::Layer
{
public:
  MyLayer(const std::string& id, const std::string& title, AbstractLayer* parent = 0);
  ~MyLayer();

  void setDataGridOperation(te::map::DataGridOperation* op);
  te::map::DataGridOperation* getDataGridOperation();
  void setTemporal(bool);
  bool isTemporal();
  void createGrid();
  void deleteGrid(bool = true);
  MyGrid* getGrid();
  std::set<QwtPlot*> getPlots();
  void insertPlot(QwtPlot*);
  void removePlot(QwtPlot*);
  void setKeepOnMemory(bool);
  bool isKeepOnMemory();
  void setTooltipColumn(int);
  int getTooltipColumn();

private:
  MyGrid* m_grid;
  std::set<QwtPlot*>   m_plots;
  te::map::DataGridOperation* m_op; // pode nao ter grid e ter um grid operation (ou pode ter ambos).
  int m_tooltipColumn;
  bool m_temporal;
  bool m_keepOnMemory; // true: mantem DataGridOperation em memoria, false: deleta DataGridOperation quando este layer nao esta em uso
};

#endif
