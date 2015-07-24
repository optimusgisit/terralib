#include "TreeItem.h"

te::qt::widgets::TreeItem::TreeItem(const std::string &type) :
  m_type(type)
{
  m_children.reset(new Queue);
  m_parent = 0;
}

te::qt::widgets::TreeItem::~TreeItem()
{
  while(m_children->getSize() > 0)
    delete m_children->remove(0);
}

void te::qt::widgets::TreeItem::setParent(TreeItem* item)
{
  m_parent = item;
}

void te::qt::widgets::TreeItem::addChild(TreeItem* item)
{
  m_children->add(item);

  item->setParent(this);
}

void te::qt::widgets::TreeItem::insertChild(TreeItem* item, const size_t& pos) throw (te::common::Exception)
{
  if(pos > m_children->getSize())
    throw te::common::Exception("Index out of boundaries.");

  m_children->insert(item, (unsigned int) pos);

  item->setParent(this);
}

te::qt::widgets::TreeItem* te::qt::widgets::TreeItem::getChild(const size_t& pos) const throw (te::common::Exception)
{
  if(pos >= m_children->getSize())
    throw te::common::Exception("Index out of boundaries.");

  return m_children->getValue((unsigned int) pos);
}

te::qt::widgets::TreeItem* te::qt::widgets::TreeItem::getParent() const
{
  return m_parent;
}

te::qt::widgets::TreeItem* te::qt::widgets::TreeItem::removeChild(const size_t& pos) throw (te::common::Exception)
{
  if(pos >= m_children->getSize())
    throw te::common::Exception("Index out of boundaries.");

  return m_children->remove((unsigned int)pos);
}

size_t te::qt::widgets::TreeItem::getChildrenCount(const std::string& type) const
{
  if(type.empty())
    return (size_t) m_children->getSize();

  size_t count = 0;
  te::common::NodeT<TreeItem*>* aux = m_children->m_first;

  while (aux != NULL)
  {
    if(aux->m_value->m_type == type)
      count++;

    aux = aux->m_next;
  }

  return count;
}

bool te::qt::widgets::TreeItem::hasChildren() const
{
  return (getChildrenCount("") != 0);
}

int te::qt::widgets::TreeItem::getPosition()
{
  return (m_parent == 0) ? -1 : (int) m_parent->m_children->position(this);
}

te::qt::widgets::VISIBLE te::qt::widgets::TreeItem::isVisible() const
{
  return NONE;
}

void te::qt::widgets::TreeItem::setVisible(const VISIBLE&, const bool&, const bool&)
{
}

std::string te::qt::widgets::TreeItem::getType() const
{
  return m_type;
}

void te::qt::widgets::TreeItem::removeAllChilds()
{
  while(m_children->getSize() > 0)
    delete m_children->remove(0);
}
