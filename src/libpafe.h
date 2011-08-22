/* $Id: libpafe.h,v 1.8 2009-10-31 08:54:53 hito Exp $ */
#ifndef __LIBPASORI_H
#define __LIBPASORI_H

/* DEFINE types */

#ifndef uint8
typedef unsigned char uint8;
#endif

#ifndef uint16
typedef unsigned short int uint16;
#endif

/* DEFINE pasori */

enum PASORI_TYPE {
  PASORI_TYPE_S310,
  PASORI_TYPE_S320,
  PASORI_TYPE_S330,
};

struct tag_pasori;

typedef struct tag_pasori pasori;
/*~DEFINE pasori */

typedef struct _ferica_area
{
  uint16 code, attr, bin;
  struct _ferica_area *next;
} felica_area;

/* DEFINE felica */

struct tag_felica
{
  pasori *p;
  uint16 systemcode;
  uint8 IDm[8];
  uint8 PMm[8];
  uint16 area_num;
  felica_area area[256];
  uint16 service_num;
  felica_area service[256];
  struct tag_felica *next;
};
typedef struct tag_felica felica;

struct tag_felica_block
{
  uint8 data[8];
};
typedef struct tag_felica_block felica_block;

typedef struct _felica_block_info
{
  uint16 service;
  uint8 mode;
  uint16 block;
} felica_block_info;

/*~DEFINE felica */

#include "pasori_command.h"
#include "felica_command.h"


/* DEFINE constants */

#define FELICA_IDM_LENGTH 8
#define FELICA_PMM_LENGTH 8
#define FELICA_BLOCK_LENGTH 16

#define FELICA_AREA_NUM_MAX 121

#define FELICA_POLLING_ANY   0xffff
#define FELICA_POLLING_SUICA 0x0003
#define FELICA_POLLING_EDY   0xfe00


#define FELICA_SERVICE_SUICA_IN_OUT  0x108f
#define FELICA_SERVICE_SUICA_HISTORY 0x090f
#define FELICA_SERVICE_IRUCA_HISTORY 0xde80
#define FELICA_SERVICE_EDY_NUMBER  0x110b
#define FELICA_SERVICE_EDY_HISTORY 0x170f


#define FELICA_CMD_POLLING 0
#define FELICA_ANS_POLLING 1

#define FELICA_CMD_REQUEST_SERVICE 2
#define FELICA_ANS_REQUEST_SERVCCE 3

#define FELICA_CMD_REQUEST_RESPONSE 4
#define FELICA_ANS_REQUEST_RESPONSE 5

#define FELICA_CMD_READ_WITHOUT_ENCRYPTION 6
#define FELICA_ANS_READ_WITHOUT_ENCRYPTION 7

#define FELICA_CMD_WRITE_WITHOUT_ENCRYPTION 8
#define FELICA_ANS_WRITE_WITHOUT_ENCRYPTION 9

#define FELICA_CMD_AUTHENTICATION1 0x10
#define FELICA_ANS_AUTHENTICATION1 0x11

#define FELICA_CMD_AUTHENTICATION2 0x12
#define FELICA_ANS_AUTHENTICATION2 0x13

#define FELICA_CMD_READ_FROM_SECURE_FILE 0x14
#define FELICA_ANS_READ_FROM_SECURE_FILE 0x15

#define FELICA_CMD_WRITE_TO_SECURE_FILE 0x16
#define FELICA_ANS_WRITE_TO_SECURE_FILE 0x17


#define FELICA_CMD_SEARCH_SERVICE_CODE 0x0a
#define FELICA_ANS_SEARCH_SERVICE_CODE 0x0b

#define FELICA_CMD_REQUEST_SYSTEM 0x0c
#define FELICA_ANS_REQUEST_SYSTEM 0x0d


#define PASORI2_CMD_SELF_DIAGNOSIS 0x52
#define PASORI2_ANS_SELF_DIAGNOSIS 0x53

#define PASORI2_CMD_GET_FIRMWARE_VERSION 0x58
#define PASORI2_ANS_GET_FIRMWARE_VERSION 0x59

#define PASORI2_CMD_SEND_PACKET 0x5c
#define PASORI2_ANS_SEND_PACKET 0x5d

#define PASORI2_DIAG_COMMUNICATION_LINE_TEST 0x00
#define PASORI2_DIAG_EEPROM_TEST 0x01
#define PASORI2_DIAG_RAM_TEST 0x02
#define PASORI2_DIAG_CPU_FUNCTION_TEST 0x03
#define PASORI2_DIAG_CPU_FANCTION_TEST 0x03
#define PASORI2_DIAG_POLLING_TEST_TO_CARD 0x10


#define PASORI_ERR_PARM 1
#define PASORI_ERR_MEM  2
#define PASORI_ERR_COM  3
#define PASORI_ERR_DATA 4
#define PASORI_ERR_CHKSUM 5
#define PASORI_ERR_FORMAT 6
#define PASORI_ERR_TYPE 7

/*~DEFINE constants */

#endif
