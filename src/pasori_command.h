/* $Id: pasori_command.h,v 1.3 2009-07-17 07:28:35 hito Exp $ */
#ifndef __PASORI_COMMAND_H
#define __PASORI_COMMAND_H

pasori *pasori_open(void);
int pasori_init(pasori *p);
void pasori_close(pasori *p);
int pasori_send(pasori *p, uint8 *data, int *size);
int pasori_recv(pasori *p, uint8 *data, int *size);
int pasori_packet_write(pasori *p, uint8 *data, int *size);
int pasori_packet_read(pasori * p, uint8 * data, int *size);
int pasori_write(pasori *p, uint8 *data, int *size);
int pasori_read(pasori *p, uint8 *data, int *size);
int pasori_reset(pasori * p);
int pasori_version(pasori *p, int *v1, int *v2);
int pasori_type(pasori *p);

int pasori_test(pasori *p, int code, uint8 *data, int *size, uint8 *rdata, int *rsize);
int pasori_test_echo(pasori *p, uint8 *data, int *size);
int pasori_test_eprom(pasori *p);
int pasori_test_ram(pasori *p);
int pasori_test_cpu(pasori *p);
int pasori_test_polling(pasori *p);
int pasori_list_passive_target(pasori *pp, unsigned char *payload, int *size);

void pasori_set_timeout(pasori *p, int timeout);

#ifdef DEBUG
#define Log printf
#else
#define Log 1 ? (void) 0 : printf
#endif

#define L8(x) (x & 0xff)
#define H8(x) ((x >> 8) & 0xff)

#endif
