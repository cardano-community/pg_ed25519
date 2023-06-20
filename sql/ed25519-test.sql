--
-- Ed25519
--
CREATE EXTENSION ed25519;

-- sign
SELECT ed25519_sign('\x1234567890'::bytea, '\x3328f55b26464930804140710a4a7f0e7825f25b7313e2fcd99e7e7c1f4f5332'::bytea);

-- verify successful
SELECT ed25519_verify('\x1234567890'::bytea, '\x258cf6f4b4c2ae09b7c4bd351af0a86a3346983fce8fe107e030309fc3641c44'::bytea, '\xddfd1e8b8c4d8f0b62060e9dbc992a0118ea51b3793e593116f6fb668c7ec0e5f5a1b314e13462cd0289248d399232e970809d214238e507b3b9dfb2930b2b0f'::bytea);

-- verify failed since the messages are different
SELECT ed25519_verify('\x0987654321'::bytea, '\x258cf6f4b4c2ae09b7c4bd351af0a86a3346983fce8fe107e030309fc3641c44'::bytea, '\xddfd1e8b8c4d8f0b62060e9dbc992a0118ea51b3793e593116f6fb668c7ec0e5f5a1b314e13462cd0289248d399232e970809d214238e507b3b9dfb2930b2b0f'::bytea);

-- verify failed since the verification key is wrong
SELECT ed25519_verify('\x1234567890'::bytea, '\xeabf67797723cccf93981098031b920200c9096030be071a5a17344f871a388e'::bytea, '\xddfd1e8b8c4d8f0b62060e9dbc992a0118ea51b3793e593116f6fb668c7ec0e5f5a1b314e13462cd0289248d399232e970809d214238e507b3b9dfb2930b2b0f'::bytea);

-- verify failed since the signature is wrong
SELECT ed25519_verify('\x1234567890'::bytea, '\x258cf6f4b4c2ae09b7c4bd351af0a86a3346983fce8fe107e030309fc3641c44'::bytea, '\x2ad1ef33653457c19e5abbfa96bd0bde2a24e0b24fb51bc45b6ae3f7f13fc9907000550aa3386a5726624acb1bb98fc1b1c2b8a5c2d7e7dbee4e91a6cea90203'::bytea);

-- create_keypair, sign and verify successful
WITH keys AS (
    SELECT * FROM ed25519_create_keypair()
)
SELECT ed25519_verify('Test message', (SELECT verification_key FROM keys), (SELECT ed25519_sign('Test message', (SELECT signing_key FROM keys))));

-- create_keypair, sign and verify failed
WITH keys AS (
    SELECT * FROM ed25519_create_keypair()
)
SELECT ed25519_verify('Message B', (SELECT verification_key FROM keys), (SELECT ed25519_sign('Message A', (SELECT signing_key FROM keys))));
