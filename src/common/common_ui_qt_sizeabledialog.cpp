//
// file:			src/common/common_ui_qt_sizeabledialog.cpp
// created on:		2020 Apr 05
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <common/ui/qt/sizeabledialog.hpp>
#include <typeinfo>
#include <ui/qt/application.hpp>  // todo: make really independent

common::ui::qt::SizeableDialog::SizeableDialog( ::QWidget* a_pParent )
	:
	  ::QDialog( a_pParent )
{
}


common::ui::qt::SizeableDialog::~SizeableDialog()
{
	//
}


int common::ui::qt::SizeableDialog::exec()
{
	int nReturn;
	NewSettings& aSettings = thisApp()->settings();
	::QWidget* pParent = static_cast<QWidget*>(parent());
	::QPoint aGeo;
	::QSize aSize;
	const ::QString  classNamePluGeoBase = QString(typeid(*this).name()) + "::Geometry::";

	aSize=aSettings.value(classNamePluGeoBase+"Size",sizeHint()).toSize();

	if(aSettings.contains(classNamePluGeoBase+"TopLeft")){
		aGeo=aSettings.value(classNamePluGeoBase+"TopLeft").toPoint();
	}
	else{
		::QPoint leftTopGlb = pParent->mapToGlobal( ::QPoint(0,0)) ;
		::QPoint rightBottomGlb = pParent->mapToGlobal( ::QPoint(pParent->width(),pParent->height())) ;
		aGeo.setX( (rightBottomGlb.x()+leftTopGlb.x()-pParent->width())/2  );
		aGeo.setY( (rightBottomGlb.y()+leftTopGlb.y()-pParent->height())/2 );
	}

	setGeometry(aGeo.x(),aGeo.y(),aSize.width(),aSize.height());

	nReturn= ::QDialog::exec();

	aSettings.setValue(classNamePluGeoBase+"TopLeft",mapToGlobal( ::QPoint(0,0) ));
	aSettings.setValue(classNamePluGeoBase+"Size",size());

	return nReturn;
}
