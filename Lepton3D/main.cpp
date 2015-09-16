#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QMessageBox>

#include <QColor>
#include <QLabel>
#include <QtDebug>
#include <QString>
#include <QPushButton>

#include "LeptonThread.h"
#include "ImageLabel.h"

int main( int argc, char **argv )
{
	//create the app
	QApplication a( argc, argv );
	
	QWidget *myWidget = new QWidget;

  // Three 320x240 images with 20px padding horizontally, 20px top and 80px below for buttons.
  int WINDOW_WIDTH = 320*3+20, WINDOW_HEIGHT = 240+60;
	myWidget->setGeometry(200, 200, WINDOW_WIDTH, WINDOW_HEIGHT);

	//create an image placeholder for imageLabel
	//fill the top left corner with red, just bcuz
  // QImage myImage;
  // myImage = QImage(640, 480, QImage::Format_RGB888);

  /*
	QRgb red = qRgb(255,0,0);
	for(int i=0;i<80;i++) {
		for(int j=0;j<60;j++) {
			myImage.setPixel(i, j, red);
		}
	}
  */

	LeptonThread *thread = new LeptonThread();

	//create a label, and set it's image to the placeholder
	ImageLabel leftImage(myWidget);
	leftImage.setGeometry(10, 10, 320, 240);
	QObject::connect(thread, SIGNAL(updateLeftImage(QImage)), &leftImage, SLOT(setImage(QImage)));

	ImageLabel centerImage(myWidget);
	centerImage.setGeometry(330, 10, 320, 240);
	QObject::connect(thread, SIGNAL(updateCenterImage(QImage)), &centerImage, SLOT(setImage(QImage)));

	ImageLabel rightImage(myWidget);
	rightImage.setGeometry(650, 10, 320, 240);
	QObject::connect(thread, SIGNAL(updateRightImage(QImage)), &rightImage, SLOT(setImage(QImage)));

	//create a FFC button
	QPushButton *button1 = new QPushButton("Perform FFC", myWidget);
	button1->setGeometry(WINDOW_WIDTH/3-50, WINDOW_HEIGHT-40, 100, 30);
	QObject::connect(button1, SIGNAL(clicked()), thread, SLOT(performFFC()));

	// Toggle AGC
	QPushButton *button2 = new QPushButton("Toggle AGC", myWidget);
	button2->setGeometry(WINDOW_WIDTH*2/3-50, WINDOW_HEIGHT-35, 100, 30);
	QObject::connect(button2, SIGNAL(clicked()), thread, SLOT(toggleAGC()));

	//create a thread to gather SPI data
	//when the thread emits updateImage, the label should update its image accordingly
	
	thread->start();
	
	myWidget->show();

	return a.exec();
}

