#include "impl/num.h"
#include "impl/field.h"
#include "impl/group.h"
#include "impl/ecmult.h"
#include "impl/ecdsa.h"

void secp256k1_start(void) {
    secp256k1_fe_start();
    secp256k1_ge_start();
    secp256k1_ecmult_start();
}

void secp256k1_stop(void) {
    secp256k1_ecmult_stop();
    secp256k1_ge_stop();
    secp256k1_fe_stop();
}

int secp256k1_ecdsa_verify(const unsigned char *msg, int msglen, const unsigned char *sig, int siglen, const unsigned char *pubkey, int pubkeylen) {
    int ret = -3;
    secp256k1_num_t m; 
    secp256k1_num_init(&m);
    secp256k1_ecdsa_sig_t s;
    secp256k1_ecdsa_sig_init(&s);
    secp256k1_gej_t q;
    secp256k1_num_set_bin(&m, msg, msglen);

    if (!secp256k1_ecdsa_pubkey_parse(&q, pubkey, pubkeylen)) {
        ret = -1;
        goto end;
    }
    if (!secp256k1_ecdsa_sig_parse(&s, sig, siglen)) {
        ret = -2;
        goto end;
    }
    if (!secp256k1_ecdsa_sig_verify(&s, &q, &m)) {
        ret = 0;
        goto end;
    }
    ret = 1;
end:
    secp256k1_ecdsa_sig_free(&s);
    secp256k1_num_free(&m);
    return ret;
}

int secp256k1_ecdsa_sign(const unsigned char *message, int messagelen, unsigned char *signature, int *signaturelen, const unsigned char *seckey, const unsigned char *nonce) {
    secp256k1_num_t sec, non, msg;
    secp256k1_num_init(&sec);
    secp256k1_num_init(&non);
    secp256k1_num_init(&msg);
    secp256k1_num_set_bin(&sec, seckey, 32);
    secp256k1_num_set_bin(&non, nonce, 32);
    secp256k1_num_set_bin(&msg, message, messagelen);
    secp256k1_ecdsa_sig_t sig;
    secp256k1_ecdsa_sig_init(&sig);
    int ret = secp256k1_ecdsa_sig_sign(&sig, &sec, &msg, &non);
    if (ret) {
        secp256k1_ecdsa_sig_serialize(signature, signaturelen, &sig);
    }
    secp256k1_ecdsa_sig_free(&sig);
    secp256k1_num_free(&msg);
    secp256k1_num_free(&non);
    secp256k1_num_free(&sec);
    return ret;
}
