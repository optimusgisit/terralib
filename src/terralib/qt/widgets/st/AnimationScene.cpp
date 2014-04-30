// TerraLib
#include "AnimationScene.h"
#include "TrajectoryItem.h"
#include "Animation.h"
#include "AnimationItem.h"
#include "../canvas/MapDisplay.h"

// Qt
#include <QtCore/QThread>
#include <QtGui/QGraphicsView>

te::qt::widgets::AnimationScene::AnimationScene(te::qt::widgets::MapDisplay* display, QObject* parent)
  : QGraphicsScene(parent),
  m_display(display),
  m_trajectoryPixmap(0),
  m_numberOfTrajectories(0),
  m_numberOfPixmaps(0)
{
  createNewPixmap();
}

te::qt::widgets::AnimationScene::~AnimationScene()
{
  delete m_trajectoryPixmap;
}

void te::qt::widgets::AnimationScene::createNewPixmap()
{
  delete m_trajectoryPixmap;
  m_trajectoryPixmap = new QPixmap(m_display->width(), m_display->height());
  m_trajectoryPixmap->fill(Qt::transparent);

  setSceneRect(0, 0, m_display->width(), m_display->height());  
  if(views().isEmpty() == false)
  {
    QGraphicsView* view = *(views().begin());
    view->resize(m_display->width()*2, m_display->height()*2);
  }
}

void te::qt::widgets::AnimationScene::addItem(te::qt::widgets::AnimationItem* item)
{
  QGraphicsScene::addItem(item);
  if(item->pixmap().isNull())
    m_numberOfPixmaps++;
  else
    m_numberOfTrajectories++;
}

void te::qt::widgets::AnimationScene::removeItem(te::qt::widgets::AnimationItem* item)
{
  QGraphicsScene::removeItem(item);
  if(item->pixmap().isNull())
    m_numberOfPixmaps--;
  else
    m_numberOfTrajectories--;
}

void te::qt::widgets::AnimationScene::clear()
{
  QGraphicsScene::clear();
  m_numberOfPixmaps = 0;
  m_numberOfTrajectories = 0;
}

void te::qt::widgets::AnimationScene::setMatrix()
{
  QList<QGraphicsItem*> list = items();
  QList<QGraphicsItem*>::iterator it;
  for(it = list.begin(); it != list.end(); ++it)
  {
    AnimationItem* ai = (AnimationItem*)(*it);
    ai->setMatrix();
  }
}

void te::qt::widgets::AnimationScene::setDuration(const int& duration)
{
  QList<QGraphicsItem*> list = items();
  QList<QGraphicsItem*>::iterator it;
  for(it = list.begin(); it != list.end(); ++it)
  {
    AnimationItem* ai = (AnimationItem*)(*it);
    ai->setDuration(duration);
  }
}

void te::qt::widgets::AnimationScene::setAutomaticPan(const QString& title)
{
  QList<QGraphicsItem*> list = items();
  QList<QGraphicsItem*>::iterator it;
  for(it = list.begin(); it != list.end(); ++it)
  {
    AnimationItem* ai = (AnimationItem*)(*it);
    if(ai->m_title == title)
      ai->m_automaticPan = !ai->m_automaticPan; // TOGGLE
    else
      ai->m_automaticPan = false;
  }
}

void te::qt::widgets::AnimationScene::draw(const int& curTime)
{
  QList<QGraphicsItem*> list = items();
  QList<QGraphicsItem*>::iterator it;
  for(it = list.begin(); it != list.end(); ++it)
  {
    AnimationItem* ai = (AnimationItem*)(*it);
    ai->m_curTimeDuration = curTime;
    ai->draw();
  }
}