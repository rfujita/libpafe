/* $Id: pasori_test.c,v 1.5 2009-10-09 07:43:14 hito Exp $ */
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "libpafe.h"

int
test(pasori *p)
{
  unsigned char data[] = "test data.";
  int n = sizeof(data) - 1;
  
  printf("Echo test... ");
  if (pasori_test_echo(p, data, &n)) {
    printf("error!\n");
    return 1;
  } else {
    printf("success\n");
  }

  printf("EPROM test... ");
  if (pasori_test_eprom(p)) {
    printf("error!\n");
    return 1;
  } else {
    printf("success\n");
  }

  printf("RAM test... ");
  if (pasori_test_ram(p)) {
    printf("error!\n");
    return 1;
  } else {
    printf("success\n");
  }

  printf("CPU test... ");
  if (pasori_test_cpu(p)) {
    printf("error!\n");
    return 1;
  } else {
    printf("success\n");
  }

  printf("Polling test... ");
  if (pasori_test_polling(p)) {
    printf("error!\n");
    return 1;
  } else {
    printf("success\n");
  }
  return 0;
}

void
show_version(pasori *p)
{
  int v1, v2;

  if (pasori_version(p, &v1, &v2)) {
    printf("cannot get version.\n");
    return;
  }

  switch (p->type) {
  case PASORI_TYPE_S310:
    printf("PaSoRi (RC-S310)\n firmware version %d.%02d\n", v1, v2);
    break;
  case PASORI_TYPE_S320:
    printf("PaSoRi (RC-S320)\n firmware version %d.%02d\n", v1, v2);
    break;
  case PASORI_TYPE_S330:
    printf("PaSoRi (RC-S330)\n firmware version %d.%02d\n", v1, v2);
    break;
  default:
    printf("unknown hardware.\n");
  }

}

int
main(void)
{
  pasori *p;

  p = pasori_open();
  if (!p) {
    printf("error\n");
    exit(-1);
  }
  pasori_init(p);

  show_version(p);

  if ((p->type == PASORI_TYPE_S310 || p->type == PASORI_TYPE_S320) && test(p)) {
    pasori_close(p);
    return 1;
  }

  pasori_close(p);

  return 0;
}
