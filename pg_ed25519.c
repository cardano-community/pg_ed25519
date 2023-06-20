#include "postgres.h"
#include <string.h>
#include "fmgr.h"
#include "funcapi.h"
#include "src/ed25519.h"
#include "src/sha512.h"
#include "src/ge.h"
#include "src/sc.h"

#define ED25519_KEY_LENGTH 32
#define ED25519_SEED_LENGTH 32
#define ED25519_DIGEST_LENGTH 64

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_sign);

Datum pg_sign(PG_FUNCTION_ARGS)
{
    unsigned char   skey[ED25519_KEY_LENGTH*2];
    unsigned char   vkey[ED25519_KEY_LENGTH];

    bytea   *message = PG_GETARG_BYTEA_PP(0);
    bytea   *signing_key = PG_GETARG_BYTEA_PP(1);
    bytea   *signature;

    size_t  data_len = VARSIZE_ANY_EXHDR(message);

    if (VARSIZE_ANY_EXHDR(signing_key) != ED25519_KEY_LENGTH)
    {
        ereport(
            ERROR,
            (
                errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Signing key must have 32 bytes length")
            )
        );
    }

    ed25519_create_keypair(vkey, skey, (unsigned char *) VARDATA_ANY(signing_key));

    signature = (bytea *) palloc(ED25519_DIGEST_LENGTH + VARHDRSZ);
    SET_VARSIZE(signature, ED25519_DIGEST_LENGTH + VARHDRSZ);

    ed25519_sign((unsigned char *) VARDATA_ANY(signature),
                 (const unsigned char *) VARDATA_ANY(message),
                 data_len,
                 vkey,
                 skey);

    PG_FREE_IF_COPY(message, 0);
    PG_FREE_IF_COPY(signing_key, 1);

    PG_RETURN_BYTEA_P(signature);
}

PG_FUNCTION_INFO_V1(pg_verify);

Datum pg_verify(PG_FUNCTION_ARGS)
{
    bytea   *message = PG_GETARG_BYTEA_PP(0);
    bytea   *verification_key = PG_GETARG_BYTEA_PP(1);
    bytea   *signature = PG_GETARG_BYTEA_PP(2);

    size_t  data_len = VARSIZE_ANY_EXHDR(message);
    int16   res;

    if (VARSIZE_ANY_EXHDR(signature) != ED25519_DIGEST_LENGTH)
    {
        ereport(
            ERROR,
            (
                errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Signature must have 64 bytes length")
            )
        );
    }

    if (VARSIZE_ANY_EXHDR(verification_key) != ED25519_KEY_LENGTH)
    {
        ereport(
            ERROR,
            (
                errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Verification key must have 32 bytes length")
            )
        );
    }

    res = ed25519_verify((const unsigned char *) VARDATA_ANY(signature), (const unsigned char *) VARDATA_ANY(message), data_len, (const unsigned char *) VARDATA_ANY(verification_key));

    PG_FREE_IF_COPY(message, 0);
    PG_FREE_IF_COPY(verification_key, 1);
    PG_FREE_IF_COPY(signature, 2);

    PG_RETURN_BOOL(res == 1);
}

PG_FUNCTION_INFO_V1(pg_create_keypair);

Datum pg_create_keypair(PG_FUNCTION_ARGS)
{
    unsigned char   seed[ED25519_SEED_LENGTH];
    unsigned char   skey[ED25519_KEY_LENGTH];
    unsigned char   vkey[ED25519_KEY_LENGTH];

    TupleDesc   tupdesc = CreateTemplateTupleDesc(2);
    HeapTuple   tuple;
    bool        nulls[2] = { false, false };
    Datum       values[2];

    bytea   *signing_key;
    bytea   *verification_key;

    ed25519_create_seed(seed);
    ed25519_create_keypair(vkey, skey, seed);

    signing_key = palloc(VARHDRSZ + ED25519_KEY_LENGTH);
    SET_VARSIZE(signing_key, VARHDRSZ + ED25519_KEY_LENGTH);
    memcpy(VARDATA(signing_key), seed, ED25519_KEY_LENGTH);

    verification_key = palloc(VARHDRSZ + ED25519_KEY_LENGTH);
    SET_VARSIZE(verification_key, VARHDRSZ + ED25519_KEY_LENGTH);
    memcpy(VARDATA(verification_key), vkey, ED25519_KEY_LENGTH);

    TupleDescInitEntry(tupdesc, (AttrNumber) 1, "signing_key", BYTEAOID, -1, 0);
    TupleDescInitEntry(tupdesc, (AttrNumber) 2, "verification_key", BYTEAOID, -1, 0);
    tupdesc = BlessTupleDesc(tupdesc);

    values[0] = PointerGetDatum (signing_key);
    values[1] = PointerGetDatum (verification_key);

    tuple = heap_form_tuple(tupdesc, values, nulls);

    PG_RETURN_DATUM(HeapTupleGetDatum(tuple));
}
