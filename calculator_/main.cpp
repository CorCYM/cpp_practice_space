#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>

static const char* g_p = NULL;
static const char* g_line_start = NULL;

static void skip_space(void) {
	while (*g_p && isspace((unsigned char)*g_p)) {
		g_p++;
	}
}

static void print_caret_at_current(const char* msg) {
	fprintf(stderr, "Error: %s\n", msg);
	if (!g_line_start) return;
	fprintf(stderr, "%s\n", g_line_start);
	size_t col = (size_t)(g_p - g_line_start);
	fprintf(stderr, " ");
	for (size_t i = 0; i < col; ++i) {
		fputc((g_line_start[i] == '\t') ? '\t' : ' ', stderr);
	}
	fprintf(stderr, "^\n");
}


static bool parse_number(double *out) {
	skip_space();
	const char* start = g_p;
	int seen_digit = 0;
	int seen_dot = 0;

	while (*g_p) {
		if (isdigit((unsigned char)*g_p)) {
			seen_digit = 1;
			g_p++;
		}
		else if (*g_p == '.' && !seen_dot) {
			seen_dot = 1;
			g_p++;
		}
		else {
			break;
		}
	}

	if (!seen_digit) {
		return false;
	}

	errno = 0;
	char buf[128];
	size_t len = (size_t)(g_p - start);
	if (len >= sizeof(buf)) len = sizeof(buf) - 1;
	memcpy(buf, start, len);
	buf[len] = '\0';

	char* endptr;
	double val = strtod(buf, &endptr);
	if (errno != 0 || endptr == buf) {
		return false;
	}
	*out = val;
	return true;
}

static bool parse_expr(double *out);

static bool parse_identifier(char name[32]) {
	skip_space();
	const char* s = g_p;
	if (!isalpha((unsigned char)*s)) {
		return false;
	}

	size_t i = 0;
	while (isalnum((unsigned char)*g_p) || *g_p == '_') {
		if (i + 1 < 32) name[i++] = *g_p;
		g_p++;
	}
	name[i] = '\0';
	return i > 0;
}

static bool parse_primary(double* out) {
	skip_space();
	if (*g_p == '(') {
		g_p++;
		if (!parse_expr(out)) {
			return false;
		}
		skip_space();
		if (*g_p != ')') {
			fprintf(stderr, "Error: expected ')' \n");
			return false;
		}
		g_p++;
		return true;
	}

	{
		char id[32];
		const char* save = g_p;
		if (parse_identifier(id)) {
			skip_space();
			if (*g_p == '(') {
				g_p++;
				double arg;
				if (!parse_expr(&arg)) {
					return false;
				}
				skip_space();
				if (*g_p != ')') {
					print_caret_at_current("expected ')' after function argument");
					return false;
				}
				g_p++;

				if (strcmp(id, "sin") == 0) *out = sin(arg);
				else if (strcmp(id, "cos") == 0) *out = cos(arg);
				else if (strcmp(id, "tan") == 0) *out = tan(arg);
				else if (strcmp(id, "sqrt") == 0) {
					if (arg < 0) {
						print_caret_at_current("sqrt domain error (< 0)");
						return false;
					}
					*out = sqrt(arg);
				}
				else if (strcmp(id, "exp") == 0) *out = exp(arg);
				else if (strcmp(id, "ln") == 0) {
					if (arg <= 0) {
						print_caret_at_current("ln domain error (<= 0)");
						return false;
					}
					*out = log(arg);
				}
				else if (strcmp(id, "log") == 0) {
					if (arg <= 0) {
						print_caret_at_current("log10 domain error (<= 0)");
						return false;
					}
					*out = log10(arg);
				}
				else if (strcmp(id, "abs") == 0) *out = fabs(arg);
				else { 
					print_caret_at_current("unknown function");
					return false;
				}

				return true;
			}
			else {
				if (strcmp(id, "pi") == 0) {
					*out = M_PI;
					return true;
				} 
				else if (strcmp(id, "e") == 0) {
					*out = M_E;
					return true;
				}
				else { g_p = save; }
			}
		}
	}

	if (parse_number(out)) return true;

	char buf[64];
	snprintf(buf, sizeof(buf), "a number, a constant, or '(' expected (found '%c')", *g_p ? *g_p : '#');
	print_caret_at_current(buf);
	return false;
}

static bool parse_power(double* out) {
	if (!parse_primary(out)) {
		return false;
	}

	skip_space();
	if (*g_p == '^') {
		g_p++;
		double rhs;
		if (!parse_power(&rhs)) {
			return false;
		}
		errno = 0;
		double val = pow(*out, rhs);
		if (errno != 0) {
			perror("pow");
			return false;
		}
		*out = val;
	}
	return true;
}

static bool parse_unary(double* out) {
	skip_space();
	if (*g_p == '+' || *g_p == '-') {
		int sign = (*g_p == '-') ? -1 : 1;
		g_p++;
		double val;
		if (!parse_unary(&val)) {
			return false;
		}
		*out = sign * val;
		return true;
	}

	return parse_power(out);
}




static bool parse_term(double* out) {
	if (!parse_unary(out)) {
		return false;
	}

	while (1) {
		skip_space();
		char op = *g_p;
		if (op != '*' && op != '/' && op != '%') {
			break;
		}
		g_p++;

		double rhs;
		if (!parse_unary(&rhs)) {
			return false;
		}

		if (op == '*') {
			*out = (*out) * rhs;
		}
		else if (op == '/') {
			if (rhs == 0.0) {
				print_caret_at_current("division by zero");
				return false;
			}
			*out = (*out) / rhs;
		}
		else {
			if (rhs == 0.0) {
				print_caret_at_current("modulo by zero");
				return false;
			}
			*out = fmod(*out, rhs);
		}
	}
	return true;
}

static bool parse_expr(double* out) {
	if (!parse_term(out)) {
		return false;
	}

	while (1) {
		skip_space();
		char op = *g_p;
		if (op != '+' && op != '-') {
			break;
		}
		g_p++;
		double rhs;
		if (!parse_term(&rhs)) {
			return false;
		}

		if (op == '+') {
			*out += rhs;
		}
		else {
			*out -= rhs;
		}
	}
	
	return true;
}

static bool eval_line(const char* line, double* result) {
	g_line_start = line;
	g_p = line;
	double val;
	if (!parse_expr(&val)) {
		return false;
	}
	skip_space();
	if (*g_p != '\0' && *g_p != '\n') {
		char buf[64];
		snprintf(buf, sizeof(buf), "unconsumed character '%c'", *g_p);
		print_caret_at_current(buf);
		return false;
	}
	*result = val;
	return true;
}

int main(void) {
	char buf[512];
	printf("C calculator\n");
	printf("  ops: + - * / %% ^   | functions: sin cos tan sqrt exp ln log abs | const: pi e\n");
	//printf("  examples: 1+4/2*3, 2^3^2, -3^2, (-3)^2, sqrt(2), log(100), ln(e)\n");
	printf("Type 'quit' to exit.\n");

	while (1) {
		printf("> ");
		if (!fgets(buf, sizeof(buf), stdin)) {
			break;
		} 
		if (strncmp(buf, "quit", 4) == 0 || strncmp(buf, "exit", 4) == 0) {
			break;
		}

		bool only_space = true;
		for (char* p = buf; *p; ++p) {
			if (!isspace((unsigned char)*p)) {
				only_space = false;
				break;
			}
		}
		if (only_space) {
			continue;
		}

		double ans;
		if (eval_line(buf, &ans)) {
			printf("= %.12g\n", ans);
		}
		else {
			// Error is expressed by stderr
		}
	}

	return 0;
}