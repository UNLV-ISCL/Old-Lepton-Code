#include "SPIReader.h"

SPIReader::SPIReader() {
  _spiPort = -1;
  resets = 0;
  restarts = 0;
  row = 0;
  is_big_endian = (htonl(42) == 42);
  
  Image = QImage(80, 60, QImage::Format_RGB888);
  FrameBufferReady = false;
}

SPIReader::~SPIReader() {
  Close();
}

void SPIReader::Open(int spiPort) {
  _spiPort = spiPort;
  SpiOpenPort(_spiPort);
}
void SPIReader::Close() {
  if (_spiPort > -1) SpiClosePort(_spiPort);
  _spiPort = -1;
}

int SPIReader::GetSPIFileDescriptor() {
  // Kind of a kludge, but I'm just working with the existing SPI.cpp demo code and its global variables:
  return _spiPort ? spi_cs1_fd : spi_cs0_fd;
}

// Returns true if a new frame is available, otherwise false.
bool SPIReader::Poll() {
  // Read PACKET_SIZE bytes from the SPI port into the current slot in rawdata.
  read(GetSPIFileDescriptor(), rawdata + row*sizeof(uint8_t)*PACKET_SIZE, sizeof(uint8_t)*PACKET_SIZE);

  // The second byte of the row should be the same as the row number; if not, we've lost sync; start over.
  if (rawdata[row*PACKET_SIZE+1] != row) {
    row = 0;
    resets += 1;
    usleep(10);

    // If there are a LOT of resets, time out the VoSPI protocol, we're way out of sync.
    // At 20Mbit/s, 750 frames * 164 bytes/frame = 49ms... the device should provide a new frame very 37ms.
    if (resets == 750) {
      // TODO: This will actually time out BOTH SPI buses, i.e. any other SPIReader objects in use.
      // It's not a bad approach (the side effects are mild), but it's sloppy.
      // Need to use a system timer, and simply keep returning false when polled until the timer elapses.

      int oldPort = _spiPort;
      Close();
      // Wait 200ms; officially it takes 185ms for the VoSPI protocol to reset.
      usleep(200000);
      Open(oldPort);

      // Don't keep timing out the VoSPI protocol if we don't pick up immediately on the next pass, just start over:
      resets = 0;
      // Still, we record the number of total restarts.
      restarts += 1;
    }
    return false;
  }

  row++;

  if (row == ROWS_PER_FRAME) {
    OnFrameComplete();
    return true;
  }
  return false;
}

// Copies new frame image data into FrameBuffer (discarding headers and flipping byte order if necessary).
// Updates QImage Image with the new frame image data.
void SPIReader::OnFrameComplete() {
  // Debug: Report excessive numbers of resets for a particular frame:
  if (resets >= 150 || restarts > 0) {
    qDebug() << "Completed frame CS=" << _spiPort << " had excessive resets: " << resets << " restarts: " << restarts;
  }

  // Next poll starts from scratch.
  resets = 0;
  restarts = 0;
  row = 0;
  
  uint16_t value;
  uint16_t minValue = 65535;
  uint16_t maxValue = 0;

  int frameBufferOffset = 0;
  uint16_t *rawdataShorts = (uint16_t*)rawdata;
  for (int i=0; i<RAW_FRAME_SIZE_UINT16; i++) {
    // Skip the header bytes of each row.
    if (i % RAW_PACKET_SIZE_UINT16 < 2) {
      continue;
    }
    
    // Swap byte order if we aren't big-endian (likely):
    if (!is_big_endian) {
      int temp = rawdata[i*2];
      rawdata[i*2] = rawdata[i*2+1];
      rawdata[i*2+1] = temp;
    }
    
    // Copy values from rawdata to the FrameBuffer.
    FrameBuffer[frameBufferOffset] = rawdataShorts[i];
    frameBufferOffset++;

    // AGC functionality.  Find the minima/maxima of the greyscale data in order to set the RGB values of the QImage in a moment.
    value = rawdataShorts[i];
    if (value > maxValue) maxValue = value;
    if (value < minValue) minValue = value;
  }

  // Indicate that we now have a valid frame.
  FrameBufferReady = true;

  // Load the QImage "Image" with pixels for debug display.
  float diff = maxValue - minValue;
  float scale = 255/diff;
  QRgb color;
  for (int i=0; i<FRAME_SIZE_UINT16; i++) {
    value = (FrameBuffer[i] - minValue) * scale;
    const int *colormap = colormap_ironblack;
    color = qRgb(colormap[3*value], colormap[3*value+1], colormap[3*value+2]);
    Image.setPixel(i % ROW_SIZE_UINT16, i / ROW_SIZE_UINT16, color);
  }
}
