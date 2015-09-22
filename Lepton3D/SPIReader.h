#ifndef SPIREADER_H
#define SPIREADER_H

#include <QtCore>
#include <QImage>
#include <stdint.h>
#include <arpa/inet.h>

#include "Palettes.h"
#include "SPI.h"

#define PACKET_SIZE 164
#define RAW_PACKET_SIZE_UINT16 (PACKET_SIZE / 2)
#define ROW_SIZE_UINT16 80
#define ROWS_PER_FRAME 60

#define RAW_FRAME_SIZE_UINT16 (RAW_PACKET_SIZE_UINT16 * ROWS_PER_FRAME)
#define FRAME_SIZE_UINT16 (ROW_SIZE_UINT16 * ROWS_PER_FRAME)

class SPIReader {

public:
  SPIReader();
  ~SPIReader();
  void Open(int spiPort);
  void Close();
  bool Poll();
  bool FrameBufferReady;

  QImage Image;
  uint16_t FrameBuffer[FRAME_SIZE_UINT16];

protected:
  int GetSPIFileDescriptor();
  void OnFrameComplete();

private:
  int _spiPort;
  uint8_t rawdata[PACKET_SIZE * ROWS_PER_FRAME];

  int resets, restarts, row;
  bool is_big_endian;
};

#endif
