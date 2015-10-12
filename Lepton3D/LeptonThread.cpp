#include "LeptonThread.h"

LeptonThread::LeptonThread() : QThread() { }

LeptonThread::~LeptonThread() { }

void LeptonThread::run() {
  left.Open(0);
  right.Open(1);
  // Note that cv::Mat is a wrapper to the underlying data representation -- we only build these once, and they point to our frame buffers.
  leftImage = cv::Mat(ROWS_PER_FRAME, ROW_SIZE_UINT16, CV_16UC1, left.FrameBuffer);
  rightImage = cv::Mat(ROWS_PER_FRAME, ROW_SIZE_UINT16, CV_16UC1, right.FrameBuffer);
  disparityImage = QImage(80, 60, QImage::Format_RGB888);

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
  // Ensure we have at least one frame of data available on both sensors before generating the map.
  if (!left.FrameBufferReady || !right.FrameBufferReady) return;
  
  // StereoBM::operator() expects 8-bit, single-channel data, not 16-bit.  Downsample.

  // Allocate off the heap so it's cleaned up when we exit.
  cv::Mat l = cv::Mat(ROWS_PER_FRAME, ROW_SIZE_UINT16, CV_8UC1);
  cv::Mat r = cv::Mat(ROWS_PER_FRAME, ROW_SIZE_UINT16, CV_8UC1);

  // Convert the 14-bit gamut linearly to 8-bit.  Low-contrast.
  leftImage.convertTo(l, CV_8UC1, 256.0/16384);
  rightImage.convertTo(r, CV_8UC1, 256.0/16384);
  // normalize(leftImage, l, 0, 255, CV_MINMAX, CV_8UC1);
  // normalize(rightImage, r, 0, 255, CV_MINMAX, CV_8UC1);
  
  // cv::StereoSGBM sbm = cv::StereoSGBM();
  // sbm.minDisparity = 0;
  // sbm.numberOfDisparities = 16;
  // sbm.SADWindowSize = 5;
  // sbm.preFilterCap = 10;
  // sbm.uniquenessRatio = 10;
  // sbm.P1 = 0;
  // sbm.P2 = 0;
  // sbm.speckleWindowSize = 0;
  // sbm.speckleRange = 0;
  // sbm.disp12MaxDiff = 0;
  // sbm.fullDP = false;

  cv::StereoBM sbm = cv::StereoBM(cv::StereoBM::FISH_EYE_PRESET, 16, 7);

  cv::Mat disparityMap = cv::Mat(ROWS_PER_FRAME, ROW_SIZE_UINT16, CV_16UC1);
  sbm(l, r, disparityMap); // CV_16SC1
  normalize(disparityMap, disparityOutput, 0, 255, CV_MINMAX, CV_8UC1);

  for (int y=0; y<disparityOutput.rows; y++) {
    const uchar* rowptr = disparityOutput.ptr(y);
    for (int x=0; x<disparityOutput.cols; x++) {
      disparityImage.setPixel(x, y, qRgb(rowptr[x], rowptr[x], rowptr[x]));
    }
  }

  emit updateCenterImage(disparityImage);
}

void LeptonThread::performFFC() {
  lepton_perform_ffc();
}

void LeptonThread::toggleAGC() {
  lepton_toggle_agc();
}
