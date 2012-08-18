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

#define EXCEPTION_TYPE_LEN 64
#define EXCEPTION_MESSAGE_LEN 256
#define EXCEPTION_FILENAME_LEN 64
#define EXCEPTION_FUNCTION_LEN 64

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

/* Global variable that contains information about last thrown exception */
exception_t _global_exception = {
	.type = {0},
	.message = {0},
	.filename = {0},
	.function = {0},
	.line = 0
};

const char * exception_get_type(void)
{
	return _global_exception.type;
}

const char * exception_get_message(void)
{
	return _global_exception.message;
}

const char * exception_get_filename(void)
{
	return _global_exception.filename;
}

const char * exception_get_function(void)
{
	return _global_exception.function;
}

unsigned int exception_get_line(void)
{
	return _global_exception.line;
}

exception_reader_t _global_exception_reader = {
	.type = &exception_get_type,
	.message = &exception_get_message,
	.filename = &exception_get_filename,
	.function = &exception_get_function,
	.line = &exception_get_line
};

void exception_set(const char *type, const char *filename,
	const char *function, unsigned int line, const char *fmt, va_list va_ptr)
{
	if (type != NULL) {
		strncpy(_global_exception.type, type, EXCEPTION_TYPE_LEN - 2);
		_global_exception.type[EXCEPTION_TYPE_LEN - 1] = '\0';
	} else {
		_global_exception.type[0] = '\0';
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

	_global_exception.line = line;
}

exception_reader_t * exception_get(void)
{
	return &_global_exception_reader;
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
	exception_t *e = &_global_exception;

	fprintf(stderr, "Uncaught exception [%s] %s at %s:%d (%s)\n",
		e->type, e->message, e->filename, e->line, e->function);

	exit(EXIT_FAILURE);
}

void exception_throw(const char *type, const char *filename,
	const char *function, unsigned int line, const char *fmt, ...)
{
	jmp_buf *env;
	va_list va_ptr;

	va_start (va_ptr, fmt);
	exception_set(type, filename, function, line, fmt, va_ptr);
	va_end(va_ptr);

	if ( (env = exception_env_get()) ) {
		longjmp(*env, 1);
	}

	/* Not within a try block, complain and exit */
	exception_print_uncaught_message_and_exit();
}

void exception_rethrow(void)
{
	jmp_buf *env;

	exception_env_pop();
	if ( (env = exception_env_get()) ) {
		longjmp(*env, 1);
	}

	/* Not within a try block, complain and exit */
	exception_print_uncaught_message_and_exit();
}

int exception_is_valid_char(char c)
{
	/* This includes all alphanumeric characters ([0-9A-Za-z])
	 * plus the following characters:
	 * : ; < = > ? @ [ \ ] ^ _ ` */
	if (c >= 48 && c <= 122)
		return 1;

	return 0;
}

int exception_is_separator(char c)
{
	/* All non-valid chars are considered separators */
	return !exception_is_valid_char(c);
}

int exception_is_catched(const char *types)
{
	exception_t *e = &_global_exception;
	int catched = 0;
	unsigned int i = 0, j = 0;

	/* No exception was thrown */
	if (e->type == NULL)
		return 0;

	/* If types is a zero-length string, return true so that
	 * catch() without parameters catch all exceptions */
	if (types[0] == '\0')
		return 1;

	while (types[i] != '\0') {
		if (types[i] != e->type[j]) {
			/* Jump to next type */
			while (exception_is_valid_char(types[i])) {
				i++;
			}
			while (types[i] != '\0'
			&& exception_is_separator(types[i])) {
				i++;
			}
			j = 0;
		} else {
			i++; j++;
			if (e->type[j] == '\0'
			&& exception_is_separator(types[i])) {
				catched = 1;
				break;
			}
		}
	}

	return catched;
}

void exception_cleanup(void)
{
	exception_env_pop();
	exception_set(NULL, NULL, NULL, 0, NULL, NULL);
}
