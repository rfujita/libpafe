/* $Id: felica_dump.c,v 1.6 2009-07-17 07:30:55 hito Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "libpafe.h"

void
mydump(uint8 * p, int size)
{
  int i;
  for (i = 0; i != size; i++) {
    printf("%02X", p[i]);
  }
}

void
show_idminfo(uint8 * idm)
{
  time_t mtime;
  time_t ntime;
  struct tm fepoc;
  signed int d;

  fepoc.tm_sec = 0;
  fepoc.tm_min = 0;
  fepoc.tm_hour = 0;
  fepoc.tm_mday = 1;
  fepoc.tm_mon = 0;
  fepoc.tm_year = 100;
  fepoc.tm_isdst = 0;

  mtime = mktime(&fepoc);
  ntime = time(NULL);

  d = idm[4] + idm[5] * 0x100;
  if (d & 0x8000) {
    d = 0 - (0x10000 - d);
  }
  mtime += d * 24 * 60 * 60;
  fepoc = *localtime(&mtime);	/* FIXME:structure copy */

  printf("# lpdump : %s", ctime(&ntime));

  printf("# --- IDm info (FeliCa) ---\n");
  printf("# Manufacture Date = %d/%d/%d\n", fepoc.tm_year + 1900,
	 fepoc.tm_mon + 1, fepoc.tm_mday);
  printf("#               SN = %d\n", idm[6] + idm[7] * 0x100);
  printf("# Manufacture Code = %04X\n", idm[0] + idm[1] * 0x100);
  printf("#      Equip. Code = %04X\n", idm[2] + idm[3] * 0x100);
}

struct __tag__sinfo
{
  int c;
  char *d;
};

typedef struct __tag__sinfo sinfo;
const sinfo Sifo[] = {
  {0, " Area Code                 "},
  {4, " Ramdom Access R/W         "},
  {5, " Random Access Read only   "},
  {6, " Cyclic Access R/W         "},
  {7, " Cyclic Access Read only   "},
  {8, " Purse (Direct)            "},
  {9, " Purse (Cashback/decrement)"},
  {10, " Purse (Decrement)         "},
  {11, " Purse (Read Only)         "},
  {-1, " INVALID or UNKNOWN        "}
};

void
print_service_info(uint16 attr)
{
  unsigned int j;

  for (j = 0; j < sizeof(Sifo) / sizeof(*Sifo); j++) {
    if ((attr >> 1) == Sifo[j].c) {
      printf("%s", Sifo[j].d);
      break;
    }
  }

  if (!(attr & 1)) {
    printf(" (PROTECTED) ");
  }
  printf("\n");
}

void
dump_service(felica * f)
{
  uint8 b[16];
  int i, j, r;

  r = felica_search_service(f);
  if (r)
    return;

  /* print header */
  printf("# card IDm = ");
  mydump(f->IDm, 8);
  printf("\n# card PMm = ");
  mydump(f->PMm, 8);

  printf("\n# area num = %d", f->area_num);
  printf("\n# service num = %d", f->service_num);

  for (i = 0; i < f->area_num; i++) {
    printf("\n# AREA #%d = %04X (%05x)", i, f->area[i].code, f->area[i].attr);
  }
  printf("\n");

  /* dump services */
  for (i = 0; i < f->service_num; i++) {
    printf("# ");
    printf("%04X:%04x", f->service[i].code, f->service[i].attr);

    print_service_info(f->service[i].attr);

    if (f->service[i].attr & 1) {
      j = 0;
      while (!felica_read_single(f, f->service[i].bin, 0, j, b)) {
	printf("  ");
	printf("%04X:%04X:", f->service[i].code, j);
	mydump(b, 16);
	printf("\n");
	j++;
      }
    }
  }
}

void
enum_service(felica * f)
{
  felica *ff;
  uint16 resp[256];
  int i, n, r;

  n  = sizeof(resp)/sizeof(*resp);
  r = felica_request_system(f, &n, resp);
  if (r){
    printf("can't get service\n");
    return;
  }

  printf("  system num %d\n", n);

  for (i = 0; i < n; i++) {
    ff = felica_polling(f->p, resp[i], 0, 0);
    if (ff == NULL) {
      printf("can't get service  %d\n", i);
      break;
    }
    printf("# FELICA SYSTEM_CODE = %04X\n", resp[i]);
    dump_service(ff);
    free(ff);
  }
}

int
main(void)
{
  pasori *p;
  felica *f;

  p = pasori_open();
  if (!p) {
    printf("error\n");
    exit(-1);
  }
  pasori_init(p);
  f = felica_polling(p, FELICA_POLLING_ANY, 0, 0);
  if (f) {
    show_idminfo(f->IDm);
    enum_service(f);
  }
  free(f);
  pasori_close(p);

  return 0;
}
