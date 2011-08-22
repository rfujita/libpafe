/* $Id: felica_command.c,v 1.5 2009-10-09 07:43:13 hito Exp $ */
#ifdef HAVE_CONFIG_H 
#include "config.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "libpafe.h"

#define DATASIZE 255

/* read w/o encrypt */

static int
_felica_pasori_read(pasori *p, uint8 *data, int *size, int ofst)
{
  int len, i;
  uint8 buf[DATASIZE + 1];

  len = *size;

  if (len > DATASIZE)
    return -1;

  i = pasori_read(p, buf, &len);

  if (i)
    return i;

  len -= ofst;
  if (len > *size)
    len = *size;

  memcpy(data, buf + ofst, len);

  *size = len;

  return i;
}

static int
felica_pasori_read(pasori *p, uint8 *data, int *size)
{
  int ofst;

  switch (pasori_type(p)) {
  case PASORI_TYPE_S310:
  case PASORI_TYPE_S320:
    ofst = 0;
    break;
  case PASORI_TYPE_S330:
    ofst = 2;
    break;
  default:
    return PASORI_ERR_TYPE;
  }

  return _felica_pasori_read(p, data, size, ofst);
}


static int
search_service(int n, int *list, int val)
{
  int i;

  for (i = 0; i < n; i++) {
    if (list[i] == val) return i;
  }
  return -1;
}

static int
pack_block_info(int n, felica_block_info *info, int *service_num, uint8 *services, uint8 *data)
{
  int i, len, snum, s, slist[16] = {0};
  uint8 *blklist;

  snum = 0;
  len = 0;
  blklist = data;
  for (i = 0; i < n; i++) {
    if (len >= DATASIZE) return -1;

    s = search_service(snum, slist, info[i].service);
    if (s < 0) {
      if (snum >= (int) (sizeof(slist)/sizeof(*slist)))
	return -1;

      slist[snum] = info[i].service;
      services[snum * 2] = L8(info[i].service);
      services[snum *2 + 1] = H8(info[i].service);
      s = snum;
      snum++;
    }

    blklist[0] = ((info[i].mode << 4) & 0x07) + (s & 0x0f);
    if (info[i].block > 255) {
      blklist[1] = L8(info[i].block);
      blklist[2] = H8(info[i].block);
      blklist += 3;
      len += 3;
    } else {
      blklist[0] |= 0x80;		/* element size == 2 */
      blklist[1] = info[i].block;
      blklist += 2;
      len += 2;
    }
  }

  *service_num = snum;
  return len;
}

int
felica_read(felica * f, int *n, felica_block_info *info, uint8 *data)
{
  uint8 cmd[DATASIZE + 1];
  uint8 resp[DATASIZE + 1];
  uint8 blklist[DATASIZE], services[DATASIZE];
  int i, size, blen, snum, num;

  if (f == NULL || n == NULL || info == NULL || data == NULL)
    return PASORI_ERR_PARM;

  num = *n;

  if (num == 0)
    return 0;

  blen = pack_block_info(num, info, &snum, services, blklist);
  if (blen < 0 || blen + FELICA_IDM_LENGTH + snum + 6 > DATASIZE)
    return PASORI_ERR_PARM;


  cmd[0] = FELICA_CMD_READ_WITHOUT_ENCRYPTION;
  memcpy(cmd + 1, f->IDm, FELICA_IDM_LENGTH);

  cmd[FELICA_IDM_LENGTH + 1] = snum;
  memcpy(cmd + FELICA_IDM_LENGTH + 2, services, snum * 2);

  cmd[FELICA_IDM_LENGTH + snum * 2 + 2] = num;

  memcpy(cmd + FELICA_IDM_LENGTH + snum * 2 + 3, blklist, blen);

  size = FELICA_IDM_LENGTH + snum * 2 + blen + 3;
  i = pasori_write(f->p, cmd, &size);
  if (i) {
    return i;
  }

  size = DATASIZE;
  i = felica_pasori_read(f->p, resp, &size);
  if (i) {
    return i;
  }

  if (resp[0] != FELICA_ANS_READ_WITHOUT_ENCRYPTION)
    return PASORI_ERR_DATA;

  if (resp[FELICA_IDM_LENGTH + 1] != 0) {
    Log("ERROR %02X %02X\n", resp[FELICA_IDM_LENGTH + 1], resp[FELICA_IDM_LENGTH + 2]);
    return PASORI_ERR_DATA;
  }

  if (resp[FELICA_IDM_LENGTH + 3] < num)
    num = resp[FELICA_IDM_LENGTH + 3];

  memcpy(data, &resp[FELICA_IDM_LENGTH + 4], FELICA_BLOCK_LENGTH * num);
  *n = num;

  return 0;
}

int
felica_read_single(felica *f, int servicecode, int mode, uint8 addr, uint8 *data)
{
  int n;
  felica_block_info info;

  info.service = servicecode;
  info.mode = mode;
  info.block = addr;
  n = 1;

  return felica_read(f, &n, &info, data);
}

felica *
felica_polling(pasori *pp, uint16 systemcode, uint8 RFU, uint8 timeslot)
{
  felica *f;
  uint8 cmd[5];
  uint8 resp[DATASIZE + 1];
  int l, n, ofst;

  Log("%s\n", __func__);

  if (pp == NULL)
    return NULL;

  cmd[0] = (uint8) FELICA_CMD_POLLING;	/* command code */
  cmd[1] = H8(systemcode);
  cmd[2] = L8(systemcode);
  cmd[3] = RFU;				/* zero */
  cmd[4] = timeslot;
  n = 5;

  switch (pasori_type(pp)) {
  case PASORI_TYPE_S310:
  case PASORI_TYPE_S320:
    ofst = 0;
    pasori_write(pp, cmd, &n);
    break;
  case PASORI_TYPE_S330:
    ofst = 3;
    pasori_list_passive_target(pp, cmd, &n);
    break;
  default:
    return NULL;
  }

  n = DATASIZE;
  l = _felica_pasori_read(pp, resp, &n, ofst);

  if (l) {
    return NULL;
  }

  if (resp[0] != FELICA_ANS_POLLING) {
    Log("!!!INVALID RETURN VALUE (%d)\n", resp[0]);
    return NULL;
  }
  f = (felica *) malloc(sizeof(felica));
  f->p = pp;
  memcpy(f->IDm, &resp[1], FELICA_IDM_LENGTH);
  memcpy(f->PMm, &resp[9], FELICA_PMM_LENGTH);
  f->systemcode = systemcode;
  f->area_num = 0;
  f->service_num = 0;

  return f;
}

int
felica_get_idm(felica *f, uint8 *idm)
{
  if (f == NULL || idm == NULL)
    return PASORI_ERR_PARM;

  memcpy(idm, f->IDm, FELICA_IDM_LENGTH);

  return 0;
}

int
felica_get_pmm(felica *f, uint8 *pmm)
{
  if (f == NULL || pmm == NULL)
    return PASORI_ERR_PARM;

  memcpy(pmm, f->PMm, FELICA_PMM_LENGTH);

  return 0;
}

static void
set_area_code(felica *f, uint16 data)
{
  int n;

  if (f == NULL)
    return;
  
  if (!(data & 0x3e)) {
    n = f->area_num++;
    f->area[n].code = data >> 6;
    f->area[n].attr = data & 0x3f;
    f->area[n].bin = data;
  } else {
    n = f->service_num++;
    f->service[n].code = data >> 6;
    f->service[n].attr = data & 0x3f;
    f->service[n].bin = data;
  }
}

int 
felica_search_service(felica *f)
{
  uint8 cmd[DATASIZE + 1];
  uint8 resp[DATASIZE + 1];
  uint16 idx;
  int n, t;

  Log("%s\n", __func__);

  if (f == NULL)
    return PASORI_ERR_PARM;

  cmd[0] = FELICA_CMD_SEARCH_SERVICE_CODE;
  memcpy(cmd + 1, f->IDm, FELICA_IDM_LENGTH);

  f->area_num = 0;
  f->service_num = 0;

  idx = 0;
  while (1) {
    cmd[FELICA_IDM_LENGTH + 1] = L8(idx);
    cmd[FELICA_IDM_LENGTH + 2] = H8(idx);

    n = FELICA_IDM_LENGTH + 3;
    pasori_write(f->p, cmd, &n);

    n = DATASIZE;
    felica_pasori_read(f->p, resp, &n);

    if (resp[0] != FELICA_ANS_SEARCH_SERVICE_CODE)
      return PASORI_ERR_DATA;

    if (resp[FELICA_IDM_LENGTH + 1] == 0xff)
      break;

    t = resp[FELICA_IDM_LENGTH + 1] + (resp[FELICA_IDM_LENGTH + 2] << 8);
    set_area_code(f, t);
    idx++;
  }
  return 0;
}

int 
felica_request_service(felica *f, int *n, uint16 *list, uint16 *data)
{
  uint8 cmd[DATASIZE + 1];
  uint8 resp[DATASIZE + 1];
  int i, len, r;

  Log("%s\n", __func__);

  if (f == NULL || list == NULL || data == NULL)
    return PASORI_ERR_PARM;

  cmd[0] = FELICA_CMD_REQUEST_SERVICE;
  memcpy(cmd + 1, f->IDm, FELICA_IDM_LENGTH);

  for (i = 0; i < *n; i++) {
    if (FELICA_IDM_LENGTH + i * 2 + 3 >= DATASIZE) {
      break;
    }

    len = FELICA_IDM_LENGTH + i * 2 + 3;

    cmd[len - 1] = list[i] & 0xff; 
    cmd[len] = list[i] >> 8; 
  }
  cmd[FELICA_IDM_LENGTH + 1] = i;

  r = pasori_write(f->p, cmd, &len);
  if (r)
    return r;

  len = DATASIZE;
  r = felica_pasori_read(f->p, resp, &len);
  if (r)
    return r;

  if (resp[0] != FELICA_ANS_REQUEST_SERVCCE)
    return PASORI_ERR_DATA;

  *n = resp[FELICA_IDM_LENGTH + 1];
  for (i = 0; i < *n; i++) {
    len = FELICA_IDM_LENGTH + i * 2 + 3;

    if (len >= DATASIZE)
      break;

    data[i] = (resp[len - 1] << 8) + resp[len]; 
  }

  return 0;
}

int 
felica_request_response(felica *f, uint8 *mode)
{
  uint8 cmd[DATASIZE + 1];
  uint8 resp[DATASIZE + 1];
  int len, r;

  Log("%s\n", __func__);

  if (f == NULL || mode == NULL)
    return PASORI_ERR_PARM;

  cmd[0] = FELICA_CMD_REQUEST_RESPONSE;
  memcpy(cmd + 1, f->IDm, FELICA_IDM_LENGTH);

  len = FELICA_IDM_LENGTH + 1;
  r = pasori_write(f->p, cmd, &len);
  if (r)
    return r;

  len = DATASIZE;
  r = felica_pasori_read(f->p, resp, &len);
  if (r)
    return r;
  
  if (resp[0] != FELICA_ANS_REQUEST_RESPONSE)
    return PASORI_ERR_DATA;

  *mode = resp[FELICA_IDM_LENGTH + 1];

  return 0;
}

int 
felica_request_system(felica *f, int *n, uint16 *data)
{
  uint8 cmd[DATASIZE + 1];
  uint8 resp[DATASIZE + 1];
  int i, len, r, num;

  Log("%s\n", __func__);

  if (f == NULL || n == 0 || data == NULL)
    return PASORI_ERR_PARM;

  cmd[0] = FELICA_CMD_REQUEST_SYSTEM;
  memcpy(cmd + 1, f->IDm, FELICA_IDM_LENGTH);
  len = FELICA_IDM_LENGTH + 1;

  r = pasori_write(f->p, cmd, &len);
  if (r)
    return r;

  len = DATASIZE;
  r = felica_pasori_read(f->p, resp, &len);
  if (r)
    return r;
  
  if (resp[0] != FELICA_ANS_REQUEST_SYSTEM)
    return PASORI_ERR_DATA;

  num = resp[FELICA_IDM_LENGTH + 1];
  if (num > *n)
    num = *n;

  for (i = 0; i < num; i++) {
    len = FELICA_IDM_LENGTH + i * 2 + 3;

    if (len >= DATASIZE)
      break;

    data[i] = (resp[len - 1] << 8) + resp[len];
  }
  *n = num;

  return 0;
}
