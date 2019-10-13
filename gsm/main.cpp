#include <iostream>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

extern "C" {
#include <gsm.h>
}

int decode_gsm(gsm handle,int16_t *out,uint8_t *in, size_t inSize){
    int ret = 0;
    while (inSize > 0) {
        gsm_decode(handle, in, out);
        in += 33;
        inSize -= 33;
        out += 160;
        ret += 160;
        gsm_decode(handle, in, out);
        in += 32;
        inSize -= 32;
        out += 160;
        ret += 160;
    }
    return ret;
}

const int kMaxNumSamplesPerFrame = 16384;
const int kMSGSMFrameSize = 65;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if(size > kMaxNumSamplesPerFrame){
        return 0;
    }
    if(((size / kMSGSMFrameSize) * kMSGSMFrameSize) != size){
        return 0;
    }
    int out_size = (size / kMSGSMFrameSize) * 320;
    int msopt = 1;
    uint8_t* in= static_cast<uint8_t*>(malloc(size));
    memcpy(in,data,size);
    int16_t* out= static_cast<int16_t*>(malloc(out_size*sizeof(int16_t)));
    gsm mGsm = gsm_create();
    if(!mGsm){
        std::cout << "fail to get mGsm" << std::endl;
        goto END;
    }

    gsm_option(mGsm, GSM_OPT_WAV49, &msopt);
    decode_gsm(mGsm, out, in,size);
END:
    free(in);
    free(out);
    gsm_destroy(mGsm);
    return 0;
}
