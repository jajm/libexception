#include <stdio.h>
#include "exception.h"

void parse_expr(char *expr) {
	if (expr == NULL) {
		/* If within a try block, throw will jump immediately at the
		 * end of this block.
		 * If not, throw prints an error message and exit */
		throw(NullArgException, "expr is NULL");
	}
	if (expr[0] != '/') {
		throw(InvalidExpressionException, "Invalid expression: '%s'", expr);
	}
	/* ... */
}

int main(int argc, char **argv)
{
	try {
		parse_expr("/valid_expr/");  // OK, program continue
		parse_expr("invalid_expr"); // Exception will be thrown
		parse_expr("/other_expression"); // Not called
	}

	/* Can catch one or several exception types in one single catch
	 * statement.
	 * Multiple catch statements are allowed */
	/* as(e) allow to retrieve exception informations into e.
	 * Can be ommited */
	catch (NullArgException, InvalidExpressionException) as (e) {
		printf("Exception catched!\n");
		printf("Exception type: %s\n", e->type);
		printf("Exception message: %s\n", e->message);
		printf("Exception file: %s\n", e->filename);
		printf("Exception function: %s\n", e->function);
		printf("Exception line: %d\n", e->line);
		/* Prints:
			Exception type: InvalidExpressionException
			Exception message: Invalid expression: 'invalid_expr'
			Exception file: main.c
			Exception function: parse_expr
			Exception line: 12
		*/
	}

	/* Catch any other exception that hasn't been caught */
	catch() as (other) {
		printf("Unknown exception type: %s\n", other->type);
		/* rethrow throws the same exception to the previous try/catch
		 * block. If there is no previous try/catch block, prints an
		 * error message and exit */
		rethrow;
	}

	return 0;
}
