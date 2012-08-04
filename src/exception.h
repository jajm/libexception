/*
 * Copyright 2012 Julian Maurice
 *
 * This file is part of libexception.
 *
 * libexception is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libexception is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libexception.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef exception_h_included
#define exception_h_included

#include <stdlib.h> // for NULL
#include <setjmp.h> // for jmp_buf

#define throw(type, message, ...) \
	exception_throw(type, #type, __FILE__, __func__, __LINE__, message, ##__VA_ARGS__)

#define try \
	exception_env_push_new(); \
	jmp_buf *_exception_env = exception_env_get(); \
	for(int _exception_bool = 1; _exception_bool; _exception_bool = 0, exception_cleanup()) \
	if (!setjmp(*_exception_env))

#define catch(...) \
	else if (exception_is_catched(__VA_ARGS__, 0))

#define as(e) \
	for(exception_t *e = exception_get(); e; e = NULL)

#define catch_any \
	else

#define rethrow \
	exception_rethrow()

#ifndef EXCEPTION_TYPE_STR_LEN
#define EXCEPTION_TYPE_STR_LEN 64
#endif

#ifndef EXCEPTION_MESSAGE_LEN
#define EXCEPTION_MESSAGE_LEN 256
#endif

#ifndef EXCEPTION_FILENAME_LEN
#define EXCEPTION_FILENAME_LEN 64
#endif

#ifndef EXCEPTION_FUNCTION_LEN
#define EXCEPTION_FUNCTION_LEN 64
#endif

typedef struct {
	int type;
	char type_str[EXCEPTION_TYPE_STR_LEN];
	char message[EXCEPTION_MESSAGE_LEN];

	char filename[EXCEPTION_FILENAME_LEN];
	unsigned int line;
	char function[EXCEPTION_FUNCTION_LEN];
} exception_t;

void
exception_env_push_new(void);

int
exception_is_catched(
	int type,
	...
);

void
exception_throw(
	int type,
	const char *type_str,
	const char *filename,
	const char *function,
	unsigned int line,
	const char *fmt,
	...
);

exception_t *
exception_get(void);

void
exception_rethrow(void);

void
exception_cleanup(void);

jmp_buf *
exception_env_get(void);

#endif /* Not exception_h_included */
