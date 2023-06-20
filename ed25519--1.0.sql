-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION ed25519" to load this file. \quit

CREATE FUNCTION ed25519_sign(message bytea, signing_key bytea)
RETURNS bytea
AS 'MODULE_PATHNAME', 'pg_sign'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION ed25519_sign(message text, signing_key bytea)
RETURNS bytea
AS 'MODULE_PATHNAME', 'pg_sign'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION ed25519_verify(message bytea, verification_key bytea, signature bytea)
RETURNS boolean
AS 'MODULE_PATHNAME', 'pg_verify'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION ed25519_verify(message text, verification_key bytea, signature bytea)
RETURNS boolean
AS 'MODULE_PATHNAME', 'pg_verify'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION ed25519_create_keypair()
RETURNS TABLE(signing_key bytea, verification_key bytea)
AS 'MODULE_PATHNAME', 'pg_create_keypair'
LANGUAGE C VOLATILE;
