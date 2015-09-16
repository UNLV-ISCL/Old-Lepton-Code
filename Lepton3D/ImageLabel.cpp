#include "ImageLabel.h"

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent) { }
ImageLabel::~ImageLabel() { }

// When an external caller calls setImage, we'll set the label's pixmap and scale it appropriately.
void ImageLabel::setImage(QImage image) {
  QPixmap pixmap = QPixmap::fromImage(image);
  int w = this->width();
  int h = this->height();
  setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
}
