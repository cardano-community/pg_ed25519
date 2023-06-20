SRCS = \
    src/fe.c \
    src/ge.c \
    src/sc.c \
    src/sha512.c \
    src/sign.c \
    src/verify.c \
    src/seed.c \
    src/keypair.c \
    pg_ed25519.c

EXTENSION = ed25519
MODULE_big = ed25519
DATA = ed25519--1.0.sql
OBJS = $(SRCS:.c=.o)
REGRESS = ed25519-test
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
