#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QtCore>
#include <QWidget>
#include <QLabel>


//we extend QLabel to give it an extra slot, setImage
//this is because we can't pass a QPixmap from our thread
//so we have to pass a QImage and turn the QImage into a QPixmap on our end

class ImageLabel : public QLabel {
  Q_OBJECT;

  public:
    ImageLabel(QWidget *parent = 0);
    ~ImageLabel();

  public slots:
    void setImage(QImage);
};

#endif
