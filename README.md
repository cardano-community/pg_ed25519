# pg_ed25519
PostgreSQL extension for a modern and secure digital signature algorithm based on performance-optimized elliptic curves

## About
The extension provides Ed25519 public-key signature system for [PostgreSQL](https://www.postgresql.org/) using portable C implementations of [Ed25519](https://github.com/orlp/ed25519) based on the SUPERCOP "ref10" implementation

## Install

Before you begin the install, you may also need to install the '-dev' PostgreSQL packages to build the extension:

```
sudo apt update
sudo apt install postgresql-server-dev-all
```

Build and install pg_ed25519:

```
cd /path/to/pg_ed25519
make
sudo make install
```

Run tests:

```
make installcheck
```

Enable extension:

```
CREATE EXTENSION ed25519;
```

## Uninstall

Disable extension:

```
DROP EXTENSION ed25519;
```

Uninstall pg_ed25519:

```
cd /path/to/pg_ed25519
sudo make uninstall
```
## Usage

Extension has 3 methods. The message parameter can have text or bytea type, the signing_key/verification_key parameters are 32 bytes long bytea, the signature parameter is 64 bytes long bytea:

```
ed25519_create_keypair()

ed25519_sign(
    message bytea,
    signing_key bytea
)

ed25519_sign(
    message text,
    signing_key bytea
)

ed25519_verify(
    message bytea,
    verification_key bytea,
    signature bytea
)

ed25519_verify(
    message text,
    verification_key bytea,
    signature bytea
)
```

Example:

```
SELECT * from ed25519_create_keypair();
                            signing_key                             |                          verification_key
--------------------------------------------------------------------+--------------------------------------------------------------------
 \x3b9eb76590e309eb69e7eee23101327b14fe68e87e1e11823437a5020958756f | \xeabf67797723cccf93981098031b920200c9096030be071a5a17344f871a388e
(1 row)

SELECT ed25519_sign('Test message', '\x3b9eb76590e309eb69e7eee23101327b14fe68e87e1e11823437a5020958756f'::bytea);
                                                            ed25519_sign
------------------------------------------------------------------------------------------------------------------------------------
 \xbf50f623b6e423e04fb1014222ab37caf954917e861f59ac55fd00f27a7c9e2d0eebeb250cbdae75f4458c7b69a4d49836727b3140d9afa0832a2de7b57c3e07
(1 row)

SELECT ed25519_verify('Test message', '\xeabf67797723cccf93981098031b920200c9096030be071a5a17344f871a388e'::bytea, '\xbf50f623b6e423e04fb1014222ab37caf954917e861f59ac55fd00f27a7c9e2d0eebeb250cbdae75f4458c7b69a4d49836727b3140d9afa0832a2de7b57c3e07'::bytea);
 ed25519_verify
----------------
 t
(1 row)
```

