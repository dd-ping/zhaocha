#include "QtGuiApplication1.h"
#include<qfiledialog.h>
#include<QPainter>
#include<qfont.h>
#include<qimage.h>
#include<qdebug.h>
#include<qvalidator.h>
#include<qmatrix.h>
#include<qmessagebox.h>
#include"qtextcodec.h"
QtGuiApplication1::QtGuiApplication1(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowTitle(QString::fromLocal8Bit("找茬[qt+vs+halcon入门项目]"));
	this->setAcceptDrops(true);
	{
		QRegExp regx("[1-9][0-9]+$");
		QValidator *validator = new QRegExpValidator(regx, ui.lineEdit);
		ui.lineEdit->setValidator(validator);
	}
	{
		QRegExp regx("[1-9][0-9]+$");
		QValidator *validator = new QRegExpValidator(regx, ui.lineEdit_2);
		ui.lineEdit_2->setValidator(validator);
	}
}
void QtGuiApplication1::openfiledialog() {
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
	QImage tm(fileName);
	ui.label->setPixmap(QPixmap::fromImage(tm).scaled(ui.label->width(),ui.label->height()));
	getImg(fileName);

}
void QtGuiApplication1::getImg(QString str)
{
	if (str == "")return;
	strname = str;
	// Local iconic variables
	HObject  ho_Image100, ho_ImageRotate, ho_GrayImage,ImageAbsDiff;
	HObject  ho_SharpenedImage2, ho_ImageMean1, ho_Regions, ho_ConnectedRegions;
	HObject  ho_RegionFillUp, ho_SelectedRegions, ho_Image_so;
	HObject  ho_Image_rd, ho_Image_cd, ho_Image_cp, ho_ImageZoomed1;
	HObject  ho_ImageAbsDiff, ho_ImageZoomed2, ho_ImageZoomed;
	HObject  ho_GrayImage2, ho_ImageMean, ho_Regions1, ho_ConnectedRegions1;
	HObject  ho_RegionFillUp1;

	// Local control variables
	HTuple  hv_Number, hv_i, hv_Width1, hv_Height1;
	HTuple  hv_Width3, hv_Height3, hv_Height, hv_Width2, hv_Height2;
	HTuple  hv_Width, hv_Number1, hv_Area, hv_Row, hv_Column;
	QByteArray ba = str.toLatin1();
	const char *cstr = ba.data();

	HTuple  ImageName(cstr);

	ReadImage(&ho_Image100, ImageName);
	RotateImage(ho_Image100, &ho_ImageRotate, 90, "constant");
	Rgb1ToGray(ho_ImageRotate, &ho_GrayImage);
	ShockFilter(ho_GrayImage, &ho_SharpenedImage2, 0.5, 10, "canny", 1);
	MeanImage(ho_SharpenedImage2, &ho_ImageMean1, 9, 9);
	Threshold(ho_SharpenedImage2, &ho_Regions, 202, 254);
	Connection(ho_Regions, &ho_ConnectedRegions);
	FillUp(ho_ConnectedRegions, &ho_RegionFillUp);
	SelectShape(ho_RegionFillUp, &ho_SelectedRegions, (HTuple("area").Append("rectangularity")),
		"and", (HTuple(672070).Append(0.59726)), (HTuple(811721).Append(1)));
	CountObj(ho_SelectedRegions, &hv_Number);
	if (0 != (hv_Number != 2))
	{
		SelectShape(ho_RegionFillUp, &ho_SelectedRegions, (HTuple("area").Append("rectangularity")),
			"and", (HTuple(50997.5).Append(0.5399)), (HTuple(100000).Append(1)));
		CountObj(ho_SelectedRegions, &hv_Number);
	}

	if (0 != (hv_Number != 2)) {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("图像发生错误咯"));
		return;
	}
	{
		HTuple end_val11 = hv_Number;
		HTuple step_val11 = 1;
		for (hv_i = 1; hv_i.Continue(end_val11, step_val11); hv_i += step_val11)
		{
			//获取元组中的单个元素
			SelectObj(ho_SelectedRegions, &ho_Image_so, hv_i);
			//缩小图像的阈
			//之前很不解为什么write_image之前为什么还要经过reduce_domain和crop_domain处理？
			//后面才知道连通分量计算connection中得到的对象是区域，并不是正真的图像。
			//reduce_domain就是通过区域来框定原图中的某个特定的图型
			ReduceDomain(ho_ImageRotate, ho_Image_so, &ho_Image_rd);
			//从已定义的灰度值中删除
			if (0 != (hv_i == 1))
			{
				CropDomain(ho_Image_rd, &ho_Image_cd);
			}
			if (0 != (hv_i == 2))
			{
				CropDomain(ho_Image_rd, &ho_Image_cp);
			}

			//将元素写出

		}
	}
	WriteImage(ho_Image_cp, "jpg", 0, "y");
	GetImageSize(ho_Image_cp, &hv_Width1, &hv_Height1);
	GetImageSize(ho_Image_cd, &hv_Width3, &hv_Height3);

	if (0 != (hv_Width1 > hv_Width3))
	{
		ZoomImageFactor(ho_Image_cd, &ho_ImageZoomed1, hv_Width1, hv_Height1, "constant");
		AbsDiffImage(ho_ImageZoomed1, ho_Image_cp, &ho_ImageAbsDiff, 1);
		SubImage(ho_ImageZoomed1, ho_Image_cp, &ImageAbsDiff, 1, 158);
	}
	else if (0 != (hv_Width3 > hv_Width1))
	{
		ZoomImageFactor(ho_Image_cp, &ho_ImageZoomed2, hv_Width3, hv_Height3, "constant");
		AbsDiffImage(ho_ImageZoomed2, ho_Image_cd, &ho_ImageAbsDiff, 1);
		SubImage(ho_ImageZoomed2, ho_Image_cd, &ImageAbsDiff, 1, 158);
	}
	else
	{
		AbsDiffImage(ho_Image_cd, ho_Image_cp, &ho_ImageAbsDiff, 1);
		SubImage(ho_Image_cd, ho_Image_cp,& ImageAbsDiff, 1, 158);

	}
	WriteImage(ImageAbsDiff, "jpg", 0, "y9");
	//div_image (Image_cp, Image_cp, ImageAbsDiff, 50, 10)
	GetImageSize(ho_ImageAbsDiff, &hv_Width2, &hv_Height2);
	ZoomImageFactor(ho_ImageAbsDiff, &ho_ImageZoomed, 0.5, 0.5, "constant");

	Rgb1ToGray(ho_ImageZoomed, &ho_GrayImage2);
	GetImageSize(ho_GrayImage2, &hv_Width, &hv_Height);

	if (ui.lineEdit_2->text().toInt() < 1)
		MeanImage(ho_GrayImage2, &ho_ImageMean, 5, 5);
	else
		MeanImage(ho_GrayImage2, &ho_ImageMean, ui.lineEdit_2->text().toInt(), ui.lineEdit_2->text().toInt());

	//crop_part (GrayImage1, ImagePart, 50, 50, Width, Height)
	//threshold (GrayImage2, Regions1, 77, 201)
	Threshold(ho_ImageMean, &ho_Regions1, 26, 123);
	Connection(ho_Regions1, &ho_ConnectedRegions1);
	FillUp(ho_ConnectedRegions1, &ho_RegionFillUp1);

	CountObj(ho_ConnectedRegions1, &hv_Number1);
	AreaCenter(ho_ConnectedRegions1, &hv_Area, &hv_Row, &hv_Column);

	//median_image (GrayImage, ImageMedian, 'circle', 1, 'mirrored')
	//write_image (Regions1, 'jpg', 0, 'y3')

	HTuple hv_uni, hv_Length;
	TupleLength(hv_Column, &hv_Length);
	QVector<double>list_x,list_y;
	{
		HTuple end_val46 = hv_Length;
		HTuple step_val46 = 1;
		for (hv_uni = 1; hv_uni.Continue(end_val46, step_val46); hv_uni += step_val46)
		{
			double xd = hv_Row[hv_uni - 1].D()*1.99;
			double yd = hv_Column[hv_uni - 1].D()*2;
			list_x.append(xd);
			list_y.append(yd);
		}
	}
	PointImg("y.jpg", list_x, list_y);
}

void QtGuiApplication1::PointImg(QString strimg, QVector<double> d_x_list, QVector<double> d_y_list)
{
	QImage pr(strimg);
	QPainter p(&pr);
	p.setPen(QPen(Qt::red, 25));
	for (int i = 0; i < d_x_list.size(); i++) {
		int xi = (d_x_list[i]);
		int yi = (d_y_list[i]);
		p.drawRect(yi, xi, 2, 2);
	}
	p.end();
	QMatrix matrix;
	matrix.rotate(90.0);
	pr = pr.transformed(matrix, Qt::FastTransformation);
	ui.label_Respon->setPixmap(QPixmap::fromImage(pr).scaled(ui.label_Respon->width(), ui.label_Respon->height()));
	{
		QImage po("y9.jpg");
		po = po.transformed(matrix, Qt::FastTransformation);
		ui.label_2->setPixmap(QPixmap::fromImage(po).scaled(ui.label_2->width(), ui.label_2->height()));
	}
}

void QtGuiApplication1::dragEnterEvent(QDragEnterEvent * event)
{
	event->acceptProposedAction();
}

void QtGuiApplication1::dropEvent(QDropEvent * event)
{
	QString name = event->mimeData()->urls().first().toLocalFile();
	QImage tm(name);
	ui.label->setPixmap(QPixmap::fromImage(tm).scaled(ui.label->width(), ui.label->height()));
	getImg(name);
}

void QtGuiApplication1::regist()
{

	getImg(strname);
}
