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
	uint8_t* in=static_cast<uint8_t*>(malloc(size));
	uint8_t* out=static_cast<uint8_t*>(malloc(size));
	memcpy(in,data,size);
	FLACDecoder *mFLACDecoder;
	mFLACDecoder = FLACDecoder::Create();
	mFLACDecoder->parseMetadata(in,size);
	delete mFLACDecoder;
	free(in);
	free(out);
	return 0;
}
