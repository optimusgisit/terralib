#include "ProjectEditor.h"
#include "ProjectInfo.h"

// Qt
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>

te::qt::af::ProjectEditor::ProjectEditor(QWidget* parent) :
QDialog(parent)
{
  QDialog::setWindowTitle(tr("Project properties editor"));
  QDialog::setWindowModality(Qt::ApplicationModal);

  //Buttons
  QPushButton* okButton = new QPushButton(tr("&Ok"), this);
  QPushButton* cancelButton = new QPushButton(tr("&Cancel"), this);
  QPushButton* helpButton = new QPushButton(tr("&Help"), this);

  QHBoxLayout* hlay = new QHBoxLayout;

  hlay->addWidget(helpButton);
  hlay->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  hlay->addWidget(okButton);
  hlay->addWidget(cancelButton);
  // --------------------------------------------------------------------

  // Project info
  m_info = new ProjectInfo(this);

  QVBoxLayout* vlay = new QVBoxLayout;

  vlay->addWidget(m_info);
  vlay->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  vlay->addLayout(hlay);

  QGridLayout* grd = new QGridLayout(this);
  grd->addLayout(vlay, 0, 0, 1, 1);
  // --------------------------------------------------------------------

  // Signal/Slot Connections
  connect(okButton, SIGNAL(pressed()), SLOT(onOkPushButtonPressed()));
  connect(cancelButton, SIGNAL(pressed()), SLOT(reject()));
  connect(helpButton, SIGNAL(pressed()), SLOT(onHelpPushButtonPressed()));
}

te::qt::af::ProjectEditor::~ProjectEditor()
{
}

void te::qt::af::ProjectEditor::setProject(Project* proj)
{
  m_info->setProject(proj);
}

void te::qt::af::ProjectEditor::onOkPushButtonPressed()
{
  m_info->updateProjectInfo();
  QDialog::accept();
}

void te::qt::af::ProjectEditor::onHelpPushButtonPressed()
{
}
