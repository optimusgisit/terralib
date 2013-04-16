#include <ui_ToolbarsWidgetForm.h>

#include "ToolbarsWidget.h"
#include "MenuBarModel.h"

#include "../ApplicationController.h"
#include "../Utils.h"

// Qt
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QToolBar>

void SetToolbars(QComboBox* cmb, std::vector< QList<QAction*> >& acts)
{
  std::vector<QToolBar*> bars = te::qt::af::ApplicationController::getInstance().getToolBars();
  std::vector<QToolBar*>::iterator it;

  for(it=bars.begin(); it != bars.end(); ++it)
  {
    QToolBar* tb = *it;
    cmb->addItem(tb->objectName(), QVariant::fromValue<QObject*>(tb));
    acts.push_back(tb->actions());
  }
}

void SetActions(QListView* view)
{
  te::qt::af::MenuBarModel* model = new te::qt::af::MenuBarModel(te::qt::af::ApplicationController::getInstance().getMenuBar("menubar"), view);
  view->setModel(model);
}

void UpdateActions(QList<QAction*>& acts, QAction* act, const bool& toAdd)
{
  if(toAdd)
    acts.append(act);
  else
    acts.removeAll(act);
}

void RemoveBars(const std::set<QToolBar*>& bars)
{
  std::set<QToolBar*>::const_iterator it;

  for(it = bars.begin(); it != bars.end(); ++it)
  {
    QToolBar* bar = *it;

    te::qt::af::RemoveToolBarFromSettings(bar);

    te::qt::af::ApplicationController::getInstance().removeToolBar(bar->objectName());

    delete bar;
  }
}

te::qt::af::ToolbarsWidget::ToolbarsWidget(QWidget* parent) :
AbstractSettingWidget(parent),
m_ui(new Ui::ToolbarsWidgetForm)
{
  m_ui->setupUi(this);

  SetToolbars(m_ui->m_toolbarsComboBox, m_actions);

  SetActions(m_ui->m_actionsListViewWidget);

  //Setting icons
  m_ui->m_addToolButton->setIcon(QIcon::fromTheme("list-add"));
  m_ui->m_removeToolButton->setIcon(QIcon::fromTheme("list-remove"));
  m_ui->m_actionsListViewWidget->setSelectionMode(QAbstractItemView::SingleSelection);

  // Signal/Slots connection
  connect (m_ui->m_toolbarsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(currentToolbarChanged(int)));
  connect (m_ui->m_addToolButton, SIGNAL(clicked()), SLOT(onAddToolbarButtonClicked()));
  connect (m_ui->m_removeToolButton, SIGNAL(clicked()), SLOT(onRemoveToolbarButtonClicked()));
  connect (m_ui->m_actionsListViewWidget->model(), SIGNAL(updateAction(QAction*, const bool&)), SLOT(updateActions(QAction*, const bool&)));

  currentToolbarChanged(0);

  m_resumeText = tr("Add, remove or modify system tool bars.");
}

te::qt::af::ToolbarsWidget::~ToolbarsWidget()
{
  delete m_ui;
}

void te::qt::af::ToolbarsWidget::saveChanges()
{
  // Updating actions
  for (int i=0; i<m_ui->m_toolbarsComboBox->count(); i++)
  {
    QToolBar* bar = (QToolBar*) m_ui->m_toolbarsComboBox->itemData(i, Qt::UserRole).value<QObject*>();
    bar->clear();
    bar->addActions(m_actions[i]);
  }

  // Updating new toolbars
  std::set<QToolBar*>::iterator it;

  for(it = m_createdBars.begin(); it != m_createdBars.end(); ++it)
  {
    QToolBar* bar = *it;
    
    AddToolBarToSettings(bar);

    te::qt::af::ApplicationController::getInstance().addToolBar(bar->objectName(), bar);
  }

  // Removed toolbars
  RemoveBars(m_removedToolBars);

  m_createdBars.clear();
  m_removedToolBars.clear();

  changeApplyButtonState(false);
}

void te::qt::af::ToolbarsWidget::currentToolbarChanged(int idx)
{
  QList<QAction*> acts = m_actions[idx];
  ((te::qt::af::MenuBarModel*)m_ui->m_actionsListViewWidget->model())->updateActionsState(acts);
}

void te::qt::af::ToolbarsWidget::onAddToolbarButtonClicked()
{
  bool ok;
  QString text = QInputDialog::getText(this, tr("Creating tool bar"), tr("Tool bar name:"), QLineEdit::Normal, tr("Name of the new toolbar"), &ok);

  if (!ok || text.isEmpty())
  {
    QMessageBox::warning(this, tr("Creating tool bar"), tr("Empty tool bar name not allowed!"));
    return;
  }

  QToolBar* bar = new QToolBar;
  bar->setObjectName(text);

  m_createdBars.insert(bar);

  int count = m_ui->m_toolbarsComboBox->count();

  m_ui->m_toolbarsComboBox->addItem(bar->objectName(), QVariant::fromValue<QObject*>(bar));

  QList<QAction*> acts;
  m_actions.push_back(acts);

  changeApplyButtonState(true);

  m_ui->m_toolbarsComboBox->setCurrentIndex(count);
}

void te::qt::af::ToolbarsWidget::onRemoveToolbarButtonClicked()
{
  QString msg = tr("Did you really want to remove tool bar?");

  if(QMessageBox::question(this, tr("Tool bars customization"), msg, QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
    return;
  
  int idx = m_ui->m_toolbarsComboBox->currentIndex();

  QToolBar* bar = (QToolBar*)m_ui->m_toolbarsComboBox->itemData(idx, Qt::UserRole).value<QObject*>();

  m_removedToolBars.insert(bar);

  m_ui->m_toolbarsComboBox->removeItem(idx);

  changeApplyButtonState(true);
}

void te::qt::af::ToolbarsWidget::onApplyButtonClicked()
{
  saveChanges();
}

void te::qt::af::ToolbarsWidget::updateActions(QAction* act, const bool& toAdd)
{
  int idx = m_ui->m_toolbarsComboBox->currentIndex();
  UpdateActions(m_actions[idx], act, toAdd);

  changeApplyButtonState(true);
}

void te::qt::af::ToolbarsWidget::changeApplyButtonState(const bool& state)
{
  m_hasChanged = state;
  emit updateApplyButtonState(m_hasChanged);
}

void te::qt::af::ToolbarsWidget::hideEvent(QHideEvent * event)
{
  if(m_hasChanged)
  {
    QString msg = tr("There are unsaved changes. Do you want to save it?");

    if(QMessageBox::question(this, tr("Tool bars customization"), msg, QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
      saveChanges();
  }

  QWidget::hideEvent(event);
}
