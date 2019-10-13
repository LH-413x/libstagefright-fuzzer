#include <iostream>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <pvamrwbdecoder.h>

//const char* magic="#!AMR-WB";

const int kNumSamplesPerFrameWB   = 320;

static size_t getFrameSize(unsigned FT) {
    static const size_t kFrameSizeWB[10] = {
        132, 177, 253, 285, 317, 365, 397, 461, 477, 40
    };

    if (FT >= 10) {
        return 1;
    }

    size_t frameSize = kFrameSizeWB[FT];

    frameSize = (frameSize + 7) / 8 + 1;

    return frameSize;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t * data, size_t size) {
	//ignore magic number
	if(size <= 1){
		return 0;	
	}
	int16_t mInputSampleBuffer[477];
	uint8_t* mydata=static_cast<uint8_t*>(malloc(size));
	memcpy(mydata,data,size);

	int16_t mode = ((mydata[0] >> 3) & 0x0f) % 9;

	int32_t memReq = pvDecoder_AmrWbMemRequirements();
        void* mDecoderBuf = malloc(memReq);
	void *mState;
	int16_t *mDecoderCookie;
        pvDecoder_AmrWb_Init(&mState, mDecoderBuf, &mDecoderCookie);

	size_t frameSize = getFrameSize(mode);
	if(frameSize  > size){
		free(mDecoderBuf);
		free(mydata);
		return 0;
	}
	int out_size=kNumSamplesPerFrameWB * sizeof(int16_t);
	void *out=malloc(out_size);	
	int16_t frameType;
        RX_State_wb rx_state;

        mime_unsorting(
                mydata+1,
                mInputSampleBuffer,
                &frameType, &mode, 1, &rx_state);

        int16_t numSamplesOutput;

        pvDecoder_AmrWb(
                mode, mInputSampleBuffer,
                static_cast<int16_t*> (out),
                &numSamplesOutput,
                mDecoderBuf, frameType, mDecoderCookie);
	
	free(mydata);
	free(mDecoderBuf);
	free(out);
	return 0;
}
