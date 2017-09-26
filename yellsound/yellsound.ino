#include <Arduboy2.h>
#include <ArdVoice.h>
#include "voices.h"

Arduboy2 arduboy;
ArdVoice ardvoice;

void setup() {
	arduboy.begin();
	arduboy.setFrameRate(30);
	ardvoice.playVoice(YELL_SOUND_q10);
}


void loop() {
	if (!(arduboy.nextFrame()))
		return;

	if (arduboy.pressed(B_BUTTON)){
		ardvoice.playVoice(YELL_SOUND_q10);
	}

	if (arduboy.pressed(A_BUTTON)){
		ardvoice.stopVoice();
	}
}

