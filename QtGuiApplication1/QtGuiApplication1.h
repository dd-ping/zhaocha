#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplication1.h"
#include<QDragEnterEvent>
#include<QMimeData>
#include<QDropEvent>
#include"halconcpp/HalconCpp.h"
#include"Halcon.h"
#include"qstring.h"
#include"qvector.h"
#include"halconcpp/HTuple.h"
#include"halconcpp/HRegion.h"
#include"halconcpp/HImage.h"
#include"halconcpp/HIOStream.h"
#if !defined(USE_IOSTREAM_H)
using namespace std;
#endif
using namespace HalconCpp;
class QtGuiApplication1 : public QMainWindow
{
	Q_OBJECT
public:
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);
	
private:
	Ui::QtGuiApplication1Class ui;
	QString strname;
	void getImg(QString str);
	void PointImg(QString strimg, QVector<double> d_x_list, QVector<double> d_y_list);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
signals:
public slots:
	void openfiledialog();
	void regist();
};
