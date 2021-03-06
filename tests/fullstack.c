/*

  Copyright (c) 2016 Martin Sustrik

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom
  the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

*/

#include <assert.h>

#include "../dsock.h"

int main() {
    int rc;
    ssize_t sz;
    char buf[200];
    const char key[] = "01234567890123456789012345678901";

    int h1[2];
    rc = unix_pair(h1);
    assert(rc == 0);
    int h2_0 = bthrottler_start(h1[0], 1000, 10, 1000, 10);
    assert(h2_0 >= 0);
    int h2_1 = bthrottler_start(h1[1], 1000, 10, 1000, 10);
    assert(h2_1 >= 0);
    int h3_0 = nagle_start(h2_0, 2000, 100);
    assert(h3_0 >= 0);
    int h3_1 = nagle_start(h2_1, 2000, 100);
    assert(h3_0 >= 0);
    int h4_0 = pfx_start(h3_0);
    assert(h4_0 >= 0);
    int h4_1 = pfx_start(h3_1);
    assert(h4_1 >= 0);
    //int h5_0 = keepalive_start(h4_0, 1000, 4000, "KEEPALIVE", 9);
    //assert(h5_0 >= 0);
    //int h5_1 = keepalive_start(h4_1, 1000, 4000, "KEEPALIVE", 9);
    //assert(h5_0 >= 0);
    int h6_0 = nacl_start(h4_0, key, 32, -1);
    assert(h6_0 >= 0);
    int h6_1 = nacl_start(h4_1, key, 32, -1);
    assert(h6_1 >= 0);
    //int h7_0 = lz4_start(h6_0);
    //assert(h7_0 > = 0);
    //int h7_1 = lz4_start(h6_1);
    //assert(h7_1 > = 0);

    rc = msend(h6_0, "ABC", 3, -1);
    assert(rc == 0);
    rc = msend(h6_0, "DEF", 3, -1);
    assert(rc == 0);
    sz = mrecv(h6_1, buf, 3, -1);
    assert(sz == 3);
    assert(buf[0] == 'A' && buf[1] == 'B' && buf[2] == 'C');
    sz = mrecv(h6_1, buf, 3, -1);
    assert(sz == 3);
    assert(buf[0] == 'D' && buf[1] == 'E' && buf[2] == 'F');
    rc = msend(h6_1, "GHI", 3, -1);
    assert(rc == 0);
    sz = mrecv(h6_0, buf, 3, -1);
    assert(sz == 3);
    assert(buf[0] == 'G' && buf[1] == 'H' && buf[2] == 'I');


    rc = hclose(h6_1);
    assert(rc == 0);
    rc = hclose(h6_0);
    assert(rc == 0);

    return 0;
}

