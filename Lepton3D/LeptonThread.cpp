#include "LeptonThread.h"

LeptonThread::LeptonThread() : QThread() { }

LeptonThread::~LeptonThread() { }

void LeptonThread::run() {
  left.Open(0);
  right.Open(1);
  // Note that cv::Mat is a wrapper to the underlying data representation -- we only build these once!
  leftImage = cv::Mat(ROW_SIZE_UINT16, ROWS_PER_FRAME, CV_16U, left.FrameBuffer);
  // rightImage = cv::Mat(ROW_SIZE_UINT16, ROWS_PER_FRAME, CV_16U, right.FrameBuffer);

  // Note that execution is single-threaded; although either SPIReader may be mid-read, its FrameBuffer will always be complete with most-recent frame.
  // We alternate between each camera, and rebuild the depth map each time a new frame is available.
  while (true) {
    if (left.Poll()) {
      emit updateLeftImage(left.Image);
      generateDepthMap();
    }
    if (right.Poll()) {
      emit updateRightImage(right.Image);
      generateDepthMap();
    }
  }
}

void LeptonThread::generateDepthMap() {
}

void LeptonThread::performFFC() {
  lepton_perform_ffc();
}

void LeptonThread::toggleAGC() {
  lepton_toggle_agc();
}
