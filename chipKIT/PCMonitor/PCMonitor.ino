
#include <PG25664CG.h>

#include "majenko.h"

// Select one of these options:

/* Slow, but flexible. You can use any pins on the board, but
 * as a result it's quite long winded to manipulate the pins.
 *
 * Pin list is board pin numbers in the sequence:
 * DC, WR, RD, CS, RESET, D0, D1, D2, D3, D4, D5, D6, D7
 */
PG25664CG tft(0, 1, 2, 11, 12, 3, 4, 5, 6, 7, 8, 9, 10);

/* For fast operation use an entire 16-bit port (PORT B) and
 * do direct port manipulation.  The pin numbers are the bit
 * position within PORT B, not the pin numbers on the board.
 * The last number is the starting bit offset for 8 consecutive
 * bits for the data value.
 *
 * Pins in this order: DC WR, RD, CS, RESET, DATA
 */
// PG25664CG_PORTB tft(0, 1, 2, 3, 4, 8);

/* The fastest operation possible - Parallel Master Port.
 * You are restricted to a specific set of pins (except reset)
 * but operation is blisteringly fast.
 *
 * Only parameter specified is the reset pin number since this
 * is not part of the PMP specification.
 */
//PG25664CG_PMP tft(9);

struct sensors {
	int32_t processor;
	int32_t motherboard;
	int32_t chipset;
	int32_t core0;
	int32_t core1;
	int32_t core2;
	int32_t core3;
	int32_t ambient;
	int32_t radeon;
};

struct sensors sensor_data;

void receivePacket(int type, uint8_t *data, uint32_t len) {
	if (data[0] == 1) {
		if (len == sizeof(struct sensors) + 1) {
			memcpy(&sensor_data, data + 1, len - 1);
		}
	}
}

void setup() {
	HID.begin();
	HID.onReceive(receivePacket);
	tft.initializeDevice();
	tft.fillScreen(Color::Black);
	tft.setTextColor(10);
	tft.openWindow(0, 0, 256, 64);

	for (int i = 0; i < 256 * 64; i++) {
		tft.windowData(header_data[i]);
	}

	tft.closeWindow();
	delay(2000);
	tft.fillScreen(Color::Black);
}

const uint32_t tmul = 60;
const uint32_t tlen = 25 * 6;
int temps[tlen];
int y = 0;
int x = 0;

void loop() {
	tft.startBuffer();
	tft.fillScreen(Color::Black);
	tft.setCursor(0, 8);
	tft.println("Fans                    Temperatures");
	tft.printf("  Processor:   %4d RPM   Core 0:  %2d C\n", sensor_data.processor, sensor_data.core0);
	tft.printf("  Motherboard: %4d RPM   Core 1:  %2d C\n", sensor_data.motherboard, sensor_data.core1);
	tft.printf("  Chipset:     %4d RPM   Core 2:  %2d C\n", sensor_data.chipset, sensor_data.core2);
	tft.printf("                          Core 3:  %2d C\n", sensor_data.core3);
	tft.printf("                          Ambient: %2d C\n", sensor_data.ambient);
	y += (sensor_data.core0 + sensor_data.core1 + sensor_data.core2 + sensor_data.core3) / 4;
	x++;

	if (x == tmul) {
		x = 0;
		y /= tmul;

		for (int i = 0; i < tlen - 1; i++) {
			temps[i] = temps[i + 1];
		}

		temps[tlen - 1] = y;
		y = 0;
	}

	for (int i = 0; i < 8; i++) {
		tft.drawLine(0, 64 - i, tlen, 64 - i, 8 - i);
		tft.drawLine(0, 64 - 16 + i, tlen, 64 - 16 + i, 8 - i);
	}

	for (int i = 0; i < tlen - 1; i++) {
		tft.drawLine(i, 64 - temps[i] / 4, i + 1, 64 - temps[i + 1] / 4, 15);
	}

	tft.endBuffer();
	delay(100);
}
