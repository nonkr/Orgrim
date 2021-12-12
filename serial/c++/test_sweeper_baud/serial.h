#pragma once

int OpenTTY(const char *pDevice, bool bBlock);

int SetBaud(int fd, int speed);

int SetOpt(int fd, int dataBits, int parity, int stopBits);
