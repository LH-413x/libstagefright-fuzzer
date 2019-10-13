#include <iostream>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <mp4dec_api.h>
#include <iostream>

//uint32_t mWidth=0xFFF;
//uint32_t mHeight=0xFFF;

uint32_t mHeight=0x1000;
uint32_t mWidth=0x1000;

void fuzz_MPEG4(uint8_t* input, size_t in_size){
	tagvideoDecControls *mHandle;
	MP4DecodingMode mode = H263_MODE ;
	uint8_t *vol_data[1];
	vol_data[0]=input;
	int32_t vol_size = in_size;
	//set input size and ptr, output width and height
	mHandle = new tagvideoDecControls;
	memset(mHandle, 0, sizeof(tagvideoDecControls));
	bool success = PVInitVideoDecoder(mHandle, vol_data, &vol_size, 1, mWidth, mHeight, mode);
	if (!success) {
		return;
	}
	MP4DecodingMode actualMode = PVGetDecBitstreamMode(mHandle);
	if (mode != actualMode) {
		return ;
	}
	PVSetPostProcType((VideoDecControls *) mHandle, 0);
	uint64_t yFrameSize = (uint64_t)mWidth * mHeight;
	uint32_t out_size = yFrameSize / 2 + yFrameSize;	
	//set output buffer ptr
	uint8_t* out_buf=static_cast<uint8_t*>(malloc(out_size));
	PVSetReferenceYUV(mHandle, out_buf);

	VopHeaderInfo header_info;
	uint32_t timestamp = 0xFFFFFFFF;
	int32_t in_size_t = in_size;
	uint8_t *input_t = input;
	uint32_t useExtTimestamp = false;
	if (PVDecodeVopHeader(mHandle, &input_t, &timestamp, &in_size_t, &header_info, &useExtTimestamp, out_buf) != true){
		goto END;
	}
	if (PVDecodeVopBody(mHandle, &in_size_t) != PV_TRUE) {
		goto END;
	}
END:
	free(out_buf);
	PVCleanUpVideoDecoder(mHandle);
	delete mHandle;
	return ;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	uint8_t* input=static_cast<uint8_t*>(malloc(size));
	memcpy(input, data, size);
	fuzz_MPEG4(input, size);
	return 0;
}
