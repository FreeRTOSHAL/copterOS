/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 */
#ifndef LC_H_
#define LC_H_
#include <system.h>

#define LC_MSG_SIZE 31

#define LC_TYPE_ACT 0x0
#define LC_TYPE_FAILT 0x1
#define LC_TYPE_PID 0x2
#define LC_TYPE_SELECT 0x3
#define LC_TYPE_CONTROL 0x4
#define LC_TYPE_EMERGENCY 0x5

#define LC_COUNT (LC_TYPE_EMERGENCY + 1)
#define LC_BUFFER_SIZE 8

struct lc_msg {
	uint8_t type;
	uint8_t data[LC_MSG_SIZE];
} PACKED;

struct lc;
struct lc *lc_init();
int32_t lc_registerCallback(struct lc *lc, uint8_t type, void (*callback)(struct lc *lc, struct lc_msg *msg));
int32_t lc_send(struct lc *lc, struct lc_msg *msg);
int32_t lc_sendAct(struct lc *lc);
int32_t lc_sendFailt(struct lc *lc);
int32_t lc_deinit(struct lc *lc);
#endif
