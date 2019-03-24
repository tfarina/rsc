#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fstrutils.h"
#include "vector.h"
#include "third_party/libcsv/csv.h"

typedef enum return_code_e {
  RC_OK,
  RC_ERROR,
} return_code_t;

typedef struct quote_s {
  char *date;
  double open;
  double high;
  double low;
  double close;
  double adj_close;
  int volume;
} quote_t;

typedef struct csv_state_s {
  /**
   * The next field (column number) to parse. */
  int field;

  /**
   * Determines wheter there has been any error during parsing.
   */
  int any_error;

  /**
   * Data of the quote that we are currently parsing.
   */
  quote_t cur_quote;

  /**
   * List of quotes parsed so far.
   */
  vector_t *quotes;
} csv_state_t;

typedef enum csv_column_e {
  CSV_COLUMN_DATE,
  CSV_COLUMN_OPEN,
  CSV_COLUMN_HIGH,
  CSV_COLUMN_LOW,
  CSV_COLUMN_CLOSE,
  CSV_COLUMN_ADJ_CLOSE,
  CSV_COLUMN_VOLUME
} csv_column_t;

static int ignore_first_line = 1;

static char *parse_str(char const *field, size_t length, return_code_t *rc) {
  if (length > 0) {
    char *str = (char *)malloc((length + 1) * sizeof(char));
    strncpy(str, field, length + 1);

    *rc = RC_OK;
    return str;
  } else {
    *rc = RC_ERROR;
    return NULL;
  }
}

static double parse_price(char const *field, size_t length, return_code_t *rc) {
  char *endptr;
  double price;

  price = (double)strtod(field, &endptr);
  if (length > 0 && (endptr == NULL || *endptr == '\0')) {
    *rc = RC_OK;
    return price;
  }

  *rc = RC_ERROR;
  return -1.0f;
}

/**
 * This functions is called each time a new csv field has been found.
 * It is responsible for determining which field is being parsed, and
 * updating the cur_quote object.
 */
static void csv_new_field_cb(void *field, size_t field_length, void *data) {
  csv_state_t *state = (csv_state_t *)data;
  char *buffer;
  return_code_t rc;

  if (state->field == -1) {
    return;
  }

  if (ignore_first_line) {
    return;
  }

  buffer = (char *)malloc((field_length + 1) * sizeof(char));
  strncpy(buffer, field, field_length);
  buffer[field_length] = '\0';

  switch (state->field) {
  case CSV_COLUMN_DATE:
    state->cur_quote.date = parse_str(buffer, field_length, &rc);
    break;
  case CSV_COLUMN_OPEN:
    state->cur_quote.open = parse_price(buffer, field_length, &rc);
    break;
  case CSV_COLUMN_HIGH:
    state->cur_quote.high = parse_price(buffer, field_length, &rc);
    break;
  case CSV_COLUMN_LOW:
    state->cur_quote.low = parse_price(buffer, field_length, &rc);
    break;
  case CSV_COLUMN_CLOSE:
    state->cur_quote.close = parse_price(buffer, field_length, &rc);
    break;
  case CSV_COLUMN_ADJ_CLOSE:
    state->cur_quote.adj_close = parse_price(buffer, field_length, &rc);
    break;
  case CSV_COLUMN_VOLUME:
    state->cur_quote.volume = atoi(buffer);
    break;
  default:
    rc = RC_ERROR;
    break;
  }

  free(buffer);

  if (rc == RC_OK) {
    state->field++;
  } else {
    state->any_error = 1;
    state->field = -1;
  }
}

/**
 * This functions is called each time a new row has been found.
 */
static void csv_new_row_cb(int c, void *data) {
  csv_state_t *state = (csv_state_t *)data;

  if (ignore_first_line) {
    ignore_first_line = 0;
    return;
  }

  if (state->field != -1) {
    /**
     * This makes a copy of cur_quote, and smells like a hack. But that
     * is because 'vector_push_back' does not make a copy and the lifetime
     * of cur_quote is short, so it does not outlive the call to
     * csv_read_quotes and without a copy we end up with pointers pointing
     * to garbage qutotes.
     */
    quote_t *copy = malloc(sizeof(quote_t));
    copy->date = state->cur_quote.date;
    copy->open = state->cur_quote.open;
    copy->high = state->cur_quote.high;
    copy->low = state->cur_quote.low;
    copy->close = state->cur_quote.close;
    copy->adj_close = state->cur_quote.adj_close;
    copy->volume = state->cur_quote.volume;
    vector_push_back(state->quotes, copy);
  }

  state->field = 0;
}

static void csv_read_quotes(char const *filename, vector_t *quotes) {
  FILE* fp;
  struct csv_parser parser;
  char buf[1024];
  size_t bytes_read;
  csv_state_t state;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open file %s: %s\n", filename, strerror(errno));
    return;
  }

  memset(&state, 0, sizeof(csv_state_t));
  state.quotes = quotes;

  if (csv_init(&parser, CSV_STRICT | CSV_APPEND_NULL | CSV_STRICT_FINI) != 0) {
    fprintf(stderr, "Failed to initialize csv parser\n");
    return;
  }

  while ((bytes_read = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
    if (csv_parse(&parser, buf, bytes_read, csv_new_field_cb, csv_new_row_cb, &state) != bytes_read) {
      fprintf(stderr, "Error while parsing %s: %s\n", filename, csv_strerror(csv_error(&parser)));
      csv_free(&parser);
      fclose(fp);
      return;
    }
  }

  csv_fini(&parser, csv_new_field_cb, csv_new_row_cb, &state);
  csv_free(&parser);

  if (ferror(fp)) {
    fprintf(stderr, "error reading file %s\n", filename);
    fclose(fp);
    return;
  }

  fclose(fp);
}

int main(int argc, char **argv) {
  char *filename;
  vector_t *quotes;
  size_t i;

  if (argc != 2) {
    fputs("usage: csvcat filename.csv\n", stderr);
    return 1;
  }

  filename = f_strdup(argv[1]);

  quotes = vector_alloc(256);

  csv_read_quotes(filename, quotes);

  for (i = 0; i < vector_size(quotes); i++) {
    quote_t *quote = vector_get(quotes, i);

    printf("%s,", quote->date);
    printf("%.4lf,", quote->open);
    printf("%.4lf,", quote->high);
    printf("%.4lf,", quote->low);
    printf("%.4lf,", quote->close);
    printf("%.4lf,", quote->adj_close);
    printf("%d\n", quote->volume);
  }

  vector_free(quotes);
  free(filename);
 
  return 0;
}