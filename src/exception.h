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

#include <stddef.h> // for NULL
#include <setjmp.h> // for jmp_buf and setjmp

/* Example usage:
 *
 * try {
 *     // block of code that could throws an exception
 * } catch(ExceptionOne) {
 *     printf("ExceptionOne!\n");
 *     // handle the exception (exit, or do what you want)
 * } catch(ExceptionTwo, ExceptionThree) as(e) {
 *     printf("%s at %s:%d", e->type, e->filename, e->line);
 * } catch() as(e) {
 *     // Any other exceptions that haven't been caught before
 *     // we decide to rethrow the exception to previous try/catch block
 *     printf("Unknown exception type: %s\n", e->type_str);
 *     rethrow;
 * }
 */

/* Example usage of throw:
 *
 * void ma_fonction(void *ptr)
 * {
 *     if (ptr == NULL)
 *         throw(NullPointerException, "ptr is NULL");
 *     // ...
 * }
 */

#define throw(type, message, ...) \
	exception_throw(#type, __FILE__, __func__, __LINE__, message, ##__VA_ARGS__)

#define try \
	exception_env_push_new(); \
	for(int _exception_bool = 1; _exception_bool; _exception_bool = 0, exception_cleanup()) \
	if (!setjmp(*exception_env_get()))

#define catch(...) \
	else if (exception_is_catched(#__VA_ARGS__))

#define as(e) \
	for(exception_t *e = exception_get(); e; e = NULL)

#define catch_any \
	else

#define rethrow \
	exception_rethrow()

#ifndef EXCEPTION_TYPE_LEN
#define EXCEPTION_TYPE_LEN 64
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
	/* Type of exception */
	char type[EXCEPTION_TYPE_LEN];

	/* Message given to 'throw', after being formatted */
	char message[EXCEPTION_MESSAGE_LEN];

	/* Respectively __FILE__, __LINE__ and __func__ of 'throw' call */
	char filename[EXCEPTION_FILENAME_LEN];
	unsigned int line;
	char function[EXCEPTION_FUNCTION_LEN];
} exception_t;

/* Functions below should not be used directly. Use macros instead */

/* Push a new empty jmp_buf variable on top of env stack */
void
exception_env_push_new(void);

/* Compare the type of thrown exception (if any) to a list of types
 * Types are passed as a single string containing a comma-separated list of
 * types.
 * Returns a true value if at least one type in list is equal
 * to exception type. Returns 0 if not. */
int
exception_is_catched(
	const char *types
);

/* Throws an exception.
 * type, filename, function and line correspond exactly to members of
 * exception_t struct.
 * fmt is a format string (as passed to printf). Other arguments are parameters
 * for this format string. The result string is stored in member message of
 * exception_t. */
void
exception_throw(
	const char *type,
	const char *filename,
	const char *function,
	unsigned int line,
	const char *fmt,
	...
);

/* Returns a pointer to last thrown exception */
exception_t *
exception_get(void);

/* Re-throws the same exception to previous try/catch block.
 * If there is no previous try/catch block, prints exception informations
 * and exit. */
void
exception_rethrow(void);

/* Clean what must to be cleaned after try/catch block */
void
exception_cleanup(void);

/* Returns a pointer to the last pushed jmp_buf variable */
jmp_buf *
exception_env_get(void);

#endif /* Not exception_h_included */
