#include <iostream>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "pvmp3decoder_api.h"
#include "mp3reader.h"
#include "audio_utils/sndfile.h"
enum {
    kInputBufferSize = 10 * 1024,
    kOutputBufferSize = 4608 * 2,
};

void fuzz(void* data, size_t size) {

	FILE *input = fmemopen(data,size,"r");

    // Initialize the config.
    tPVMP3DecoderExternal config;
    config.equalizerType = flat;
    config.crcEnabled = false;

    // Allocate the decoder memory.
    uint32_t memRequirements = pvmp3_decoderMemRequirements();
    void *decoderBuf = malloc(memRequirements);
    assert(decoderBuf != NULL);

    // Initialize the decoder.
    pvmp3_InitDecoder(&config, decoderBuf);

    // Open the input file.
    Mp3Reader mp3Reader;
    bool success = mp3Reader.init(input);
    if (!success) {
        free(decoderBuf);
        return ;
    }

    // Open the output file.
    SF_INFO sfInfo;
    memset(&sfInfo, 0, sizeof(SF_INFO));
    sfInfo.channels = mp3Reader.getNumChannels();
    sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfInfo.samplerate = mp3Reader.getSampleRate();
    //SNDFILE *handle = sf_open(argv[2], SFM_WRITE, &sfInfo);

    // Allocate input buffer.
    uint8_t *inputBuf = static_cast<uint8_t*>(malloc(kInputBufferSize));
    assert(inputBuf != NULL);

    // Allocate output buffer.
    int16_t *outputBuf = static_cast<int16_t*>(malloc(kOutputBufferSize));
    assert(outputBuf != NULL);

    // Decode loop.
    int retVal = EXIT_SUCCESS;
    while (1) {
        // Read input from the file.
        uint32_t bytesRead;
        bool success = mp3Reader.getFrame(inputBuf, &bytesRead);
        if (!success) break;

        // Set the input config.
        config.inputBufferCurrentLength = bytesRead;
        config.inputBufferMaxLength = 0;
        config.inputBufferUsedLength = 0;
        config.pInputBuffer = inputBuf;
        config.pOutputBuffer = outputBuf;
        config.outputFrameSize = kOutputBufferSize / sizeof(int16_t);

        ERROR_CODE decoderErr;
        decoderErr = pvmp3_framedecoder(&config, decoderBuf);
        if (decoderErr != NO_DECODING_ERROR) {
            fprintf(stderr, "Decoder encountered error\n");
            retVal = EXIT_FAILURE;
            break;
        }
    }

    // Close input reader and output writer.
    mp3Reader.close();

    // Free allocated memory.
    free(inputBuf);
    free(outputBuf);
    free(decoderBuf);

}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	void* input=malloc(size);
	memcpy(input, data, size);
	fuzz(input, size);
	return 0;
}
