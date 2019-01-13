/*
 * SQLITE DATABASE SCHEMA FOR THE cli DATABASE.
 *
 * To recreate the database: sqlite3 users.db < schema.sql
 *
 */

PRAGMA foreign_keys=OFF;

BEGIN TRANSACTION;

/* User table --------------------------------------------------------------- */

CREATE TABLE user (
  uid INTEGER PRIMARY KEY,
  fname TEXT,
  lname TEXT,
  email TEXT UNIQUE
);

/* Initial Data ------------------------------------------------------------- */

/*
 * Populating with standard values.
 */

INSERT INTO "user" VALUES (1, 'John', 'Smith', 'jsmith@yahoo.com');

COMMIT;
