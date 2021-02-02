#ifndef __UBUS_H__
#define __UBUS_H__

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include <netinet/in.h>
#include <netinet/ether.h>

#include <libubox/avl.h>
#include <libubox/uloop.h>

//#include "timing.h"

#include "QMIThread.h"


void * ubus_main(void * arg);

#endif /* __UBUS_H__ */

