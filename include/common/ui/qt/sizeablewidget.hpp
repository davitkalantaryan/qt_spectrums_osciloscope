//
// file:			common/ui/qt/sizeablewidget.hpp
// created on:		2020 Apr 05
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef COMMON_UI_QT_SIZEABLEWIDGET_HPP
#define COMMON_UI_QT_SIZEABLEWIDGET_HPP

#include <QWidget>
#include <QPoint>
#include <QString>

#define PROP_TO_SET_KEY(_settingKey,_propField)	( (_settingKey) + "/" #_propField )

#define GET_PROP_FROM_SETT(_settings,_settingKey,_container,_propField, _defValue, _convertFunc)		\
	_container->_propField = (_settings).value( PROP_TO_SET_KEY(_settingKey,_propField) , _defValue)._convertFunc()
#define WRITE_PROP_TO_SETT(_settings,_settingKey,_container,_propField)		\
	(_settings).setValue( PROP_TO_SET_KEY(_settingKey,_propField) , _container->_propField)

namespace common{ namespace ui { namespace qt{

template <typename ParentWidget>
class SizeableWidget : public ParentWidget
{
public:
	template <class... Args >
	SizeableWidget( const char* settingsKey, Args&&... args );
	template <class... Args >
	SizeableWidget( const ::QPoint* pOriginPoint, const char* settingsKey, Args&&... args );
	virtual ~SizeableWidget() override;

protected:
	virtual void showEvent(QShowEvent *event) override;
	virtual void hideEvent(QHideEvent *event) override;

protected:
	const ::QPoint*		m_originPoint;
	::QString			m_settingsKey;
	bool				m_bSettingSaved;

};

}}}  // namespace common{ namespace ui { namespace qt{


#ifndef COMMON_UI_QT_SIZEABLEWIDGET_IMPL_HPP
#include "sizeablewidget.impl.hpp"
#endif

#endif  // #ifndef COMMON_UI_QT_SIZEABLEWIDGET_HPP
