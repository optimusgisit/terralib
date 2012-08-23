#include "HLDelegateDecorator.h"

//TerraLib include files
#include <terralib/common/Exception.h>

//Qt4 include files
#include <QColor>
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>


namespace te 
{
  namespace qt 
  {
    namespace widgets 
    {
      HLDelegateDecorator::~HLDelegateDecorator()
      {
      }

      void HLDelegateDecorator::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
      {
        if(!index.isValid())
          return;

        std::string key = getPKey(index);

        if(!key.empty() && HighlightDelegate::m_enabled && (m_hlOids.find(key) != m_hlOids.end()))
          painter->fillRect(option.rect, m_color);
        else if(m_decorated != 0)
        {
          m_decorated->paint(painter, option, index);
          return;
        }

        HighlightDelegate::paint(painter, option, index);
      }

      void HLDelegateDecorator::setPKeysColumns(const std::vector<size_t>& pKeyCols)
      {
        HighlightDelegate::setPKeysColumns(pKeyCols);

        if(m_decorated != 0)
          m_decorated->setPKeysColumns(pKeyCols);
      }

      void HLDelegateDecorator::setClassColor(const size_t& cPosition, const QColor& color)
      {
        HighlightDelegate* del = getDecorated(cPosition);

        if(del == 0)
          throw te::common::Exception(tr("Class position out of boundaries.").toAscii().data());

        del->setHighlightColor(color);
      }

      void HLDelegateDecorator::setClassSelection(const size_t& cPosition, const std::set<std::string>& sel)
      {
        HighlightDelegate* del = getDecorated(cPosition);

        if(del == 0)
          throw te::common::Exception(tr("Class position out of boundaries.").toAscii().data());

        del->setHighlightedPkeys(sel);
      }

      HighlightDelegate* HLDelegateDecorator::getDecorated(const size_t& pos)
      {
        if(pos == 0)
          return this;

        if((m_decorated == 0) && (pos > 0))
          throw te::common::Exception(tr("Delegate position out of boundaries.").toAscii().data());

        HLDelegateDecorator* del = dynamic_cast<HLDelegateDecorator*>(m_decorated);

        if(del == 0)
        {
          if(pos > 1)
            throw te::common::Exception(tr("Delegate position out of boundaries.").toAscii().data());

          return m_decorated;
        }

        return del->getDecorated(pos -1);
      }

      size_t HLDelegateDecorator::getGroupPosition(const std::string& pKey) const
      {
        std::set<std::string>::iterator it = m_hlOids.find(pKey);

        if(it == m_hlOids.end())
        {
          if(m_decorated == 0)
            return 0;

          HLDelegateDecorator* aux = dynamic_cast<HLDelegateDecorator*>(m_decorated);

          return (aux != 0) ? (aux->getGroupPosition(pKey) + 1) : 1;
        }

        return 0;
      }

      size_t HLDelegateDecorator::getNumberOfClasses() const
      {
        if(m_decorated == 0)
          return 0;

        HLDelegateDecorator* dec = dynamic_cast<HLDelegateDecorator*>(m_decorated);

        if(dec == 0)
          return 1;

        return dec->getNumberOfClasses() + 1;
      }

      void HLDelegateDecorator::clearSelection()
      {
        if(m_decorated != 0)
          m_decorated->clearSelection();

        HighlightDelegate::clearSelection();
      }

      HighlightDelegate* HLDelegateDecorator::getDelegate(const size_t& numClasses)

      {
        HighlightDelegate* dele = new HighlightDelegate;

        for(size_t i = 0; i<numClasses; i++)
          dele = new HLDelegateDecorator(dele);

        return dele;
      }

      HLDelegateDecorator::HLDelegateDecorator(HighlightDelegate* decorated) :
      te::common::Decorator<HighlightDelegate>(decorated)
      {
        if(m_decorated)
        {
          m_decorated->setParent(this);
          m_pkeys = m_decorated->getPKeysColumns();
        }
      }
    } // namespace te::qt::widgets
  } // namespace te::qt
} // namespace te
