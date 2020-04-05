//
// file:			common/ui/qt/sizeabledialog.hpp
// created on:		2020 Apr 05
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef COMMON_UI_QT_SIZEABLEDIALOG_HPP
#define COMMON_UI_QT_SIZEABLEDIALOG_HPP

#include <QDialog>

namespace common{ namespace ui { namespace qt{

class SizeableDialog : public ::QDialog
{
public:
	SizeableDialog( ::QWidget* pParent );
	virtual ~SizeableDialog() override;
	int exec() override;

};

}}}  // namespace common{ namespace ui { namespace qt{


#endif  // #ifndef COMMON_UI_QT_SIZEABLEDIALOG_HPP
