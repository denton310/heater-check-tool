#ifndef HEATER_H_
#define HEATER_H_

enum state {WAIT = 0, BLOW_TEST, GLOW_TEST, DOS_TEST, BD_TEST, TEST_OK};

int heater_check();

#endif /* HEATER_H_ */