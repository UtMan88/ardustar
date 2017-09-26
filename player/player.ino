#include <Arduboy2.h>
#include "bitmaps.h"

#define FRAME_RATE 30

// make an instance of the Arduboy2 class used for many functions
Arduboy2 arduboy;

#define ARDBITMAP_SBUF arduboy.getBuffer()
#include <ArdBitmap.h>
ArdBitmap<WIDTH, HEIGHT> ardbitmap;

unsigned long counter = 0;

void setup() {
	arduboy.begin();
	arduboy.setFrameRate(FRAME_RATE);
}

void loop() {
	if (!(arduboy.nextFrame()))
		return;

	arduboy.clear();

	ardbitmap.drawCompressed(WIDTH / 2, HEIGHT / 2, PLYR_0, WHITE, ALIGN_CENTER, MIRROR_NONE);

	arduboy.display();

	counter++;

}

