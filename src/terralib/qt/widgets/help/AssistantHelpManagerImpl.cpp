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
  \file terralib/qt/widgets/help/AssistantHelpManagerImpl.cpp

  \brief An implementation of HelpManager that uses QAssistant to show help files.
*/

// TerraLib
#include "AssistantHelpManagerImpl.h"

// STL
#include <iostream>

// Qt
#include <QDir>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QProcess>

void TeInitAssistant(QProcess* proc, const QString& qHFileName)
{
  QStringList args;
  QString app;

#if !defined(Q_OS_MAC)
  app += QLatin1String("assistant");
#else
  app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
#endif

  args << QLatin1String("-collectionFile")
       << qHFileName.toLatin1().data()
       << QLatin1String("-enableRemoteControl");

  proc->start(app, args);

  if (!proc->waitForStarted())
    QMessageBox::critical(0, QObject::tr("Remote Control"), QObject::tr("Could not start Qt Assistant from local \"%1\".").arg(app));
}


te::qt::widgets::AssistantHelpManagerImpl::AssistantHelpManagerImpl(QObject* parent, const QString& qtHFileName) :
QObject(parent),
  m_proc(0),
  m_qHFileName(qtHFileName)
{
}

te::qt::widgets::AssistantHelpManagerImpl::~AssistantHelpManagerImpl()
{
  if(m_proc != 0)
  {
    QByteArray ba;
    bool tag = m_regDocs.size() > 1;

    for(int i = 0; i < m_regDocs.size();i++)
    {
      ba.append("unregister " + m_regDocs.value(i));

      if(tag)
          ba.append((i == (m_regDocs.size()-1)) ? "\n" : ";"); 
      else
        ba.append('\n');
    }

    m_proc->write(ba);

    m_proc->close();
  }

  delete m_proc;
}

void te::qt::widgets::AssistantHelpManagerImpl::showHelp (const QString& htmRef)
{
  if(m_proc == 0)
    m_proc = new QProcess;

  QByteArray ba;
  QStringList::iterator it;

  switch(m_proc->state())
  {
    case QProcess::NotRunning:
      TeInitAssistant(m_proc, m_qHFileName);

      ba.append("setSource " + htmRef.toLocal8Bit() + "\n");
      m_proc->write(ba);

    break;

    case QProcess::Running:
      ba.append("setSource " + htmRef + ";");
      ba.append("syncContents\n");

      m_proc->write(ba);
    break;

    default:
    break;
  }
}

void te::qt::widgets::AssistantHelpManagerImpl::appendDoc(const QString& docRef)
{
  if(m_regDocs.contains(docRef))
    return;

  m_regDocs.append(docRef);

  QString app;
  QStringList args;
  QByteArray ba;
  int r;

  if ((m_proc == 0) || m_proc->state() == QProcess::NotRunning)
  {
  #if !defined(Q_OS_MAC)
    app += QLatin1String("assistant");
  #else
    app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
  #endif

    args << QLatin1String("-collectionFile")
      << m_qHFileName.toLatin1().data();

    args << QLatin1String("-register")
         << docRef
         << QLatin1String("-quiet");

    r = QProcess::execute(app, args);
  }
  else if(m_proc->state() == QProcess::Running)
  {
    ba.append(QLatin1String("register ") + docRef.toLocal8Bit() + '\n');
    m_proc->write(ba);
  }
}

