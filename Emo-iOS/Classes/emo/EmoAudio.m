#import "EmoAudio.h"
#import "EmoEngine.h"

extern EmoEngine* engine;

@implementation EmoAudioManager

@end

/*
 * audio interfaces
 */
SQInteger emoLoadAudio(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoCreateAudioEngine(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoPlayAudioChannel(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoPauseAudioChannel(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoStopAudioChannel(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoSeekAudioChannel(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoCloseAudioChannel(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoCloseAudioEngine(HSQUIRRELVM v) {
	return 0;
}

SQInteger emoGetAudioChannelCount(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoGetAudioChannelVolume(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoSetAudioChannelVolume(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoGetAudioChannelMaxVolume(HSQUIRRELVM v) {
	return 0;
}
SQInteger emoGetAudioChannelMinVolume(HSQUIRRELVM v) {
	return 0;
}
