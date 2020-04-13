//
// file:			common/ui/qt/sizeablewidget.impl.hpp
// created on:		2020 Apr 05
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef COMMON_UI_QT_SIZEABLEWIDGET_IMPL_HPP
#define COMMON_UI_QT_SIZEABLEWIDGET_IMPL_HPP


#ifndef COMMON_UI_QT_SIZEABLEWIDGET_HPP
#include "sizeablewidget.hpp"
#endif

#include <typeinfo>
#include <QSettings>

template <typename ParentWidget>
template <class... Args >
::common::ui::qt::SizeableWidget<ParentWidget>::SizeableWidget(const char* a_settingsKey, Args&&... a_args)
	:
	  ParentWidget(a_args...),
	  m_originPoint(nullptr)
{
	m_bSettingSaved = false;
	m_settingsKey = a_settingsKey?a_settingsKey:typeid(*this).name();
}


template <typename ParentWidget>
template <class... Args >
::common::ui::qt::SizeableWidget<ParentWidget>::SizeableWidget(const ::QPoint* a_pOriginPoint, const char* a_settingsKey, Args&&... a_args)
	:
	  ParentWidget(a_args...),
	  m_originPoint(a_pOriginPoint)
{
	m_bSettingSaved = false;
	m_settingsKey = a_settingsKey?a_settingsKey:typeid(*this).name();
}


template <typename ParentWidget>
::common::ui::qt::SizeableWidget<ParentWidget>::~SizeableWidget()
{
	if(!m_bSettingSaved){
		::QSettings aSettings;
		const ::QString settingsKeyPlusGeo = m_settingsKey + "/Geo/";

		aSettings.setValue(settingsKeyPlusGeo + "LeftTop", ParentWidget::mapToGlobal( ::QPoint(0,0)) );
		aSettings.setValue(settingsKeyPlusGeo + "Size", ParentWidget::size() );
	}
}


template <typename ParentWidget>
void ::common::ui::qt::SizeableWidget<ParentWidget>::showEvent(QShowEvent *a_event)
{
	::QSettings aSettings;
	::QWidget* pParent = static_cast<QWidget*>( ParentWidget::parent() );
	::QPoint aLeftTop;
	::QSize aSize;
	const ::QString settingsKeyPlusGeo = m_settingsKey + "/Geo/";

	if(m_originPoint){
		aLeftTop = *m_originPoint;
	}
	else if(pParent){
		::QPoint leftTopGlb = pParent->mapToGlobal( ::QPoint(0,0)) ;
		::QPoint rightBottomGlb = pParent->mapToGlobal( ::QPoint(pParent->width(),pParent->height())) ;
		aLeftTop.setX( (rightBottomGlb.x()+leftTopGlb.x()-pParent->width())/2  );
		aLeftTop.setY( (rightBottomGlb.y()+leftTopGlb.y()-pParent->height())/2 );
	}
	else{
		// we have global window
		aLeftTop = aSettings.value(settingsKeyPlusGeo + "LeftTop", ParentWidget::mapToGlobal( ::QPoint(0,0)) ).toPoint();
	}

	aSize=aSettings.value(settingsKeyPlusGeo + "Size", ParentWidget::sizeHint() ).toSize();
	ParentWidget::setGeometry(aLeftTop.x(),aLeftTop.y(),aSize.width(),aSize.height());
	ParentWidget::showEvent(a_event);
}


template <typename ParentWidget>
void ::common::ui::qt::SizeableWidget<ParentWidget>::hideEvent(QHideEvent *a_event)
{
	::QSettings aSettings;
	const ::QString settingsKeyPlusGeo = m_settingsKey + "/Geo/";

	aSettings.setValue(settingsKeyPlusGeo + "LeftTop", ParentWidget::mapToGlobal( ::QPoint(0,0)) );
	aSettings.setValue(settingsKeyPlusGeo + "Size", ParentWidget::size() );
	m_bSettingSaved = true;
	ParentWidget::hideEvent(a_event);
}


#endif  // #ifndef COMMON_UI_QT_SIZEABLEWIDGET_IMPL_HPP
