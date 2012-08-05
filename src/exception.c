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

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exception.h"

typedef struct exception_env_stack_s {
	jmp_buf env;
	struct exception_env_stack_s *prev;
} exception_env_stack_t;

/* Global variable which is a pointer to the top of environment stack
 * (a stack is needed for nested try/catch blocks) */
exception_env_stack_t *_global_exception_env_stack;

/* Global variable that contains information about last thrown exception */
exception_t _global_exception = {
	.type = 0,
	.type_str = {0},
	.message = {0},
	.filename = {0},
	.function = {0},
	.line = 0
};

void exception_set(int type, const char *type_str, const char *filename,
	const char *function, unsigned int line, const char *fmt, va_list va_ptr)
{
	if (type_str != NULL) {
		strncpy(_global_exception.type_str, type_str, EXCEPTION_TYPE_STR_LEN - 2);
		_global_exception.type_str[EXCEPTION_TYPE_STR_LEN - 1] = '\0';
	} else {
		_global_exception.type_str[0] = '\0';
	}

	if (filename != NULL) {
		strncpy(_global_exception.filename, filename, EXCEPTION_FILENAME_LEN - 2);
		_global_exception.filename[EXCEPTION_FILENAME_LEN - 1] = '\0';
	} else {
		_global_exception.filename[0] = '\0';
	}

	if (function != NULL) {
		strncpy(_global_exception.function, function, EXCEPTION_FUNCTION_LEN - 2);
		_global_exception.function[EXCEPTION_FUNCTION_LEN - 1] = '\0';
	} else {
		_global_exception.function[0] = '\0';
	}

	if (fmt != NULL) {
		vsnprintf(_global_exception.message, EXCEPTION_MESSAGE_LEN, fmt, va_ptr);
		_global_exception.message[EXCEPTION_MESSAGE_LEN - 1] = '\0';
	} else {
		_global_exception.message[0] = '\0';
	}

	_global_exception.type = type;
	_global_exception.line = line;
}

exception_t * exception_get(void)
{
	return &_global_exception;
}

void exception_env_push_new(void)
{
	exception_env_stack_t *new;

	new = malloc(sizeof(exception_env_stack_t));
	if (new == NULL) {
		fprintf(stderr, "libexception: FATAL: Not enough memory. Exiting.\n");
		exit(EXIT_FAILURE);
	}
	new->prev = _global_exception_env_stack;
	_global_exception_env_stack = new;
}

jmp_buf * exception_env_get(void)
{
	return (_global_exception_env_stack)
		? &(_global_exception_env_stack->env)
		: NULL;
}

void exception_env_pop(void)
{
	exception_env_stack_t *cur;

	cur = _global_exception_env_stack;
	_global_exception_env_stack = cur->prev;
	free(cur);
}

void exception_print_uncaught_message_and_exit(void)
{
	exception_t *e;

	e = exception_get();
	fprintf(stderr, "Uncaught exception [%s] %s at %s:%d (%s)\n",
		e->type_str, e->message, e->filename, e->line, e->function);

	exit(EXIT_FAILURE);
}

void exception_throw(int type, const char *type_str, const char *filename,
	const char *function, unsigned int line, const char *fmt, ...)
{
	jmp_buf *env;
	va_list va_ptr;

	va_start (va_ptr, fmt);
	exception_set(type, type_str, filename, function, line, fmt, va_ptr);
	va_end(va_ptr);

	if ( (env = exception_env_get()) ) {
		longjmp(*env, type);
	}

	/* Not within a try block, complain and exit */
	exception_print_uncaught_message_and_exit();
}

void exception_rethrow(void)
{
	jmp_buf *env;
	exception_t *e;

	exception_env_pop();
	if ( (env = exception_env_get()) ) {
		e = exception_get();
		longjmp(*env, e->type);
	}

	/* Not within a try block, complain and exit */
	exception_print_uncaught_message_and_exit();
}

int exception_is_catched(int type, ...)
{
	va_list va_ptr;
	exception_t *e;
	int catched = 0;

	e = exception_get();
	if (type == e->type) {
		catched = 1;
	} else {
		int t;
		va_start(va_ptr, type);
		do {
			t = va_arg(va_ptr, int);
		} while (t != e->type && t != 0);
		va_end(va_ptr);
		catched = (t == e->type) ? 1 : 0;
	}

	return catched;
}

void exception_cleanup(void)
{
	exception_env_pop();
	exception_set(0, NULL, NULL, NULL, 0, NULL, NULL);
}
