/* $Id: felica_command.h,v 1.3 2008-01-15 12:24:26 hito Exp $ */
#ifndef __FELICA_COMMAND_H
#define __FELICA_COMMAND_H

felica *felica_polling(pasori *p, uint16 systemcode, uint8 RFU, uint8 timeslot);
int felica_get_idm(felica *f, uint8 *idm);
int felica_get_pmm(felica *f, uint8 *idm);
int felica_read(felica * f, int *n, felica_block_info *info, uint8 *data);
int felica_read_single(felica * f, int servicecode, int mode, uint8 addr, uint8 *data);
int felica_write(felica *f, int servicecode, int mode, uint8 addr, uint8 *data);
int felica_request_service(felica *f, int *n, uint16 *list, uint16 *data);
int felica_request_response(felica *f, uint8 *mode);
int felica_search_service(felica *f);
int felica_request_system(felica *f, int *n, uint16 *data);

#endif
