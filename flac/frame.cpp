#include <iostream>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <media/stagefright/foundation/ABase.h>
#include <utils/RefBase.h>
#include <utils/String8.h>
#include <FLACDecoder.h>
using namespace android;

//const int mNumericalData = 0x7F000001;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t * data, size_t size) {
	const bool outputFloat=0x7F000001;
	int in_size=size;
	size_t out_size=50*size;
	uint8_t* in=static_cast<uint8_t*>(malloc(in_size));
	uint8_t* out=static_cast<uint8_t*>(malloc(out_size));
	memcpy(in,data,size);
	FLACDecoder *mFLACDecoder;
	mFLACDecoder = FLACDecoder::Create();
	mFLACDecoder->decodeOneFrame(in,in_size,out,&out_size,outputFloat);
	delete mFLACDecoder;
	free(in);
	free(out);
	return 0;
}
