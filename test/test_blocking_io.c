/*
 * test_blocking_io.c
 *
 * Test for synchronous io helper methods.
 *
 * Copyright (C) 2022 Jordan Sun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "../src/blocking_io.h"
#include <string.h>
#include <assert.h>

#define BUF_LEN 16
#define NL_LEN 1

int main(int argc, char *argv[])
{
    char nl[NL_LEN] = {'\n'};

    /* test write */
    for (int i = 0; i < argc; ++i)
    {
        ssize_t len = strlen(argv[i]);
        assert(blocking_write(STDOUT_FILENO, argv[i], len) == len);
        assert(blocking_write(STDOUT_FILENO, nl, NL_LEN) == NL_LEN);
    }

    /* test read */
    char buf[BUF_LEN];
    assert(blocking_read(STDIN_FILENO, buf, BUF_LEN) == BUF_LEN);
    assert(blocking_write(STDOUT_FILENO, buf, BUF_LEN) == BUF_LEN);
    assert(blocking_write(STDOUT_FILENO, nl, NL_LEN) == NL_LEN);

    return 0;
}