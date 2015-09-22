#ifndef TEXTTHREAD
#define TEXTTHREAD

#include <ctime>
#include <stdint.h>

#include <QThread>
#include <QtCore>
#include <QPixmap>
#include <QImage>

#include <opencv.hpp>
#include <highgui.hpp>
#include <calib3d.hpp>

#include <libconfig.h++>

#include "Lepton_I2C.h"
#include "SPIReader.h"

class LeptonThread : public QThread
{
  Q_OBJECT;

public:
  LeptonThread();
  ~LeptonThread();

  void run();

public slots:
  void performFFC();
  void toggleAGC();

signals:
  void updateText(QString);
  void updateLeftImage(QImage);
  void updateCenterImage(QImage);
  void updateRightImage(QImage);

protected:
  SPIReader left, right;
  cv::Mat leftImage, rightImage, disparityOutput;
  QImage disparityImage;
  void generateDepthMap();
  
private:

};

#endif
