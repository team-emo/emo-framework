#include <stdio.h>
#include <assert.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>

#include <SLES/OpenSLES.h>
#include "SLES/OpenSLES_Android.h"

#include <squirrel.h>
#include <common.h>
#include <main.h>
#include <emo_engine_func.h>

struct AudioChannel {
    SLObjectItf   playerObject;
    SLPlayItf     playerPlay;
    SLSeekItf     playerSeek;
    SLVolumeItf   playerVolume;
};

extern struct engine *g_engine;

struct AudioChannel *audioChannels;

static int  audioChannelCount  = 0;
static bool audioEngineCreated = false;

static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;
static SLObjectItf outputMixObject = NULL;

static SLresult checkOpenSLresult(const char* message, SLresult result) {
    if (SL_RESULT_SUCCESS != result) {
        LOGE(message);
    }
    return result;
}

bool createAudioEngine(int channelCount) {

    audioChannelCount = channelCount;

    if (audioEngineCreated) {
        g_engine->lastError = ERR_AUDIO_ENGINE_CREATED;
        LOGE("emo_audio: audio engine is already created.");
        return false;
    }

    SLresult result;
    audioChannels = (AudioChannel *)malloc(sizeof(AudioChannel) * audioChannelCount);

    // create engine
    const SLInterfaceID engine_ids[] = {SL_IID_ENGINE};
    const SLboolean engine_req[] = {SL_BOOLEAN_TRUE};
    result = slCreateEngine(&engineObject, 0, NULL, 1, engine_ids, engine_req);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ENGINE_INIT;
        LOGE("emo_audio: failed to create audio engine");
        return false;
    }

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ENGINE_INIT;
        LOGE("emo_audio: failed to realize audio engine");
        return false;
    }

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ENGINE_INIT;
        LOGE("emo_audio: failed to get audio engine interface");
        return false;
    }

    // create output mix
    const SLInterfaceID mix_ids[1] = {SL_IID_VOLUME};
    const SLboolean mix_req[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, mix_ids, mix_req);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ENGINE_INIT;
        LOGE("emo_audio: failed to create output mix object");
        return false;
    }

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ENGINE_INIT;
        LOGE("emo_audio: failed to realize output mix object");
        return false;
    }

    audioEngineCreated = true;

    return true;
}

bool createAudioChannelFromAsset(const char* fname, struct AudioChannel* channel) {
    SLresult result;

    AAssetManager* mgr = g_engine->app->activity->assetManager;
    if (mgr == NULL) {
    	g_engine->lastError = ERR_ASSET_LOAD;
    	LOGE("emo_audio: failed to load AAssetManager");
    	return false;
    }

    AAsset* asset = AAssetManager_open(mgr, fname, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
    	g_engine->lastError = ERR_ASSET_OPEN;
    	LOGE("emo_audio: failed to open an audio file");
        LOGE(fname);
    	return false;
    }

    // open asset as file descriptor
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    assert(0 <= fd);
    AAsset_close(asset);

    // configure audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID player_ids[3] = {SL_IID_PLAY, SL_IID_VOLUME, SL_IID_SEEK};
    const SLboolean player_req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &channel->playerObject, &audioSrc, &audioSnk,
            3, player_ids, player_req);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ASSET_INIT;
        LOGE("emo_audio: failed to create an audio player");
        return false;
    }

    // realize the player
    result = (*channel->playerObject)->Realize(channel->playerObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ASSET_INIT;
        LOGE("emo_audio: failed to realize an audio player");
        return false;
    }

    // get the play interface
    result = (*channel->playerObject)->GetInterface(channel->playerObject, SL_IID_PLAY, &channel->playerPlay);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ASSET_INIT;
        LOGE("emo_audio: failed to get an audio player interface");
        return false;
    }

    // get the seek interface
    result = (*channel->playerObject)->GetInterface(channel->playerObject, SL_IID_SEEK, &channel->playerSeek);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ASSET_INIT;
        LOGE("emo_audio: failed to get an audio seek interface");
        return false;
    }

    // the volume interface
    result = (*channel->playerObject)->GetInterface(channel->playerObject, SL_IID_VOLUME, &channel->playerVolume);
    if (SL_RESULT_SUCCESS != result) {
        g_engine->lastError = ERR_AUDIO_ASSET_INIT;
        LOGE("emo_audio: failed to create an audio volume interface");
        return false;
    }

    return true;
}

SLuint32 getAudioChannelState(struct AudioChannel* channel) {
    SLuint32 state;
    SLresult result = (*channel->playerPlay)->GetPlayState(channel->playerPlay, &state);
    checkOpenSLresult("emo_audio: failed to get play state", result);
    return state;
}

bool setAudioChannelState(struct AudioChannel* channel, SLuint32 state) {
    SLresult result = (*channel->playerPlay)->SetPlayState(channel->playerPlay, state);
    checkOpenSLresult("emo_audio: failed to set play state", result);
    return (SL_RESULT_SUCCESS == result);
}

bool seekAudioChannel(struct AudioChannel* channel, int pos) {
    SLresult result = (*channel->playerSeek)->SetPosition(channel->playerSeek, pos, SL_SEEKMODE_FAST);
    checkOpenSLresult("emo_audio: failed to seek audio channel", result);
    return (SL_RESULT_SUCCESS == result);
}

bool playAudioChannel(struct AudioChannel* channel) {
    if (getAudioChannelState(channel) != SL_PLAYSTATE_STOPPED) {
        seekAudioChannel(channel, 0);
    }
    return setAudioChannelState(channel, SL_PLAYSTATE_PLAYING);
}

bool pauseAudioChannel(struct AudioChannel* channel) {
    return setAudioChannelState(channel, SL_PLAYSTATE_PAUSED);
}

bool stopAudioChannel(struct AudioChannel* channel) {
    return setAudioChannelState(channel, SL_PLAYSTATE_STOPPED);
}

struct AudioChannel* getAudioChannel(int index) {
    return &audioChannels[index];
}

void closeAudioChannel(struct AudioChannel* channel) {
    if (channel->playerObject != NULL) {
        (*channel->playerObject)->Destroy(channel->playerObject);
        channel->playerObject = NULL;
        channel->playerPlay   = NULL;
        channel->playerSeek   = NULL;
        channel->playerVolume = NULL;
    }
}

void closeAudioEngine() {
    for (int i = 0; i < audioChannelCount; i++) {
        closeAudioChannel(&audioChannels[i]);
    }
    free(audioChannels);

    (*outputMixObject)->Destroy(outputMixObject);
    (*engineObject)->Destroy(engineObject);

    engineObject    = NULL;
    outputMixObject = NULL;
    engineEngine    = NULL;

    audioEngineCreated = false;
}

/*
 * SQInteger loadAudio(SQInteger audioIndex, SQChar* filename);
 */
SQInteger emoLoadAudio(HSQUIRRELVM v) {
    if (!audioEngineCreated) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }

    SQInteger channelIndex;
    const SQChar* filename;

    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_STRING) {
        sq_getinteger(v, 2, &channelIndex);
        sq_tostring(v, 3);
        sq_getstring(v, -1, &filename);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    struct AudioChannel* channel = getAudioChannel(channelIndex);

    if (!createAudioChannelFromAsset(filename, channel)) {
        sq_pushinteger(v, g_engine->lastError);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

SQInteger emoCreateAudioEngine(HSQUIRRELVM v) {
    if (audioEngineCreated) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CREATED);
        return 1;
    }

    SQInteger channelCount;

    if (sq_gettype(v, 2) == OT_INTEGER) {
        sq_getinteger(v, 2, &channelCount);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    if (!createAudioEngine(channelCount)) {
        sq_pushinteger(v, g_engine->lastError);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

SQInteger emoPlayAudioChannel(HSQUIRRELVM v) {
    if (!audioEngineCreated) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }

    SQInteger channelIndex;

    if (sq_gettype(v, 2) == OT_INTEGER) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    if (!playAudioChannel(getAudioChannel(channelIndex))) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

SQInteger emoPauseAudioChannel(HSQUIRRELVM v) {
    if (!audioEngineCreated) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }

    SQInteger channelIndex;

    if (sq_gettype(v, 2) == OT_INTEGER) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    if (!pauseAudioChannel(getAudioChannel(channelIndex))) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

SQInteger emoStopAudioChannel(HSQUIRRELVM v) {
    if (!audioEngineCreated) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }

    SQInteger channelIndex;

    if (sq_gettype(v, 2) == OT_INTEGER) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    if (!stopAudioChannel(getAudioChannel(channelIndex))) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

SQInteger emoCloseAudioChannel(HSQUIRRELVM v) {
    if (!audioEngineCreated) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }

    SQInteger channelIndex;

    if (sq_gettype(v, 2) == OT_INTEGER) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    struct AudioChannel* channel = getAudioChannel(channelIndex);

    if (channel->playerObject == NULL) {
        sq_pushinteger(v, ERR_AUDIO_CHANNEL_CLOSED);
        return 1;
    }

    closeAudioChannel(channel);

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

SQInteger emoCloseAudioEngine(HSQUIRRELVM v) {
    if (!audioEngineCreated) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }

    closeAudioEngine();

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}
