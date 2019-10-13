#include <iostream>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <gsmamr_dec.h>

const char* magic="#!AMR\n";
const uint32_t kFrameSizes[] = {12, 13, 15, 17, 19, 20, 26, 31};
extern "C" int LLVMFuzzerTestOneInput(const uint8_t * data, size_t size) {
	void* amrHandle;
	size_t index=0;
	int len_magic=strlen(magic);
	if(size<len_magic || memcmp(data,magic,len_magic)){
		return 0;
	}
	index+=len_magic;

	int kInputBufferSize=64;
	int kSamplesPerFrame = 160;
	int kBitsPerSample = 16;

	void* inputBuf = malloc(kInputBufferSize);
	void* outputBuf= malloc(kSamplesPerFrame * kBitsPerSample/8);

	assert(inputBuf != NULL);
	
	GSMInitDecode(&amrHandle, (Word8*)"AMRNBDecoder");

	while(true){
		uint8_t mode;
		if(index+sizeof(mode) >= size){
			goto END;
		}
		mode=data[index];
		index+=1;
		Frame_Type_3GPP frameType = (Frame_Type_3GPP)((mode >> 3) & 0x0f);
		if (frameType >= AMR_SID){
			goto END;
		}
		int32_t frameSize = kFrameSizes[frameType];
		if(index+frameSize >= size){
			goto END;
		}
		memcpy(inputBuf, data,frameSize);
		index+=frameSize;
		int32_t decodeStatus;
        	decodeStatus = AMRDecode(amrHandle, frameType, (uint8_t*)inputBuf,(int16_t*)outputBuf, MIME_IETF);
		if(decodeStatus == -1) {
			goto END;
		}
	}	
END:
	free(inputBuf);
	free(outputBuf);
	GSMDecodeFrameExit(&amrHandle);
	return 0;
}
