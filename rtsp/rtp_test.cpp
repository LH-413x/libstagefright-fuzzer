//#define LOG_NDEBUG 0
#define LOG_TAG "rtp_test"
#include <utils/Log.h>

#include <binder/ProcessState.h>

#include <media/DataSource.h>
#include <media/stagefright/foundation/base64.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/SimpleDecodingSource.h>

#include <ARTPSession.h>
#include <ASessionDescription.h>
#include <UDPPusher.h>

using namespace android;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t * data, size_t size) {
    uint8_t* in=static_cast<uint8_t*>(malloc(size));
    memcpy(in,data,size);
    sp<ASessionDescription> desc = new ASessionDescription;
    desc->setTo(in, size);
    return 0;
}
