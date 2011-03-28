void createAudioChannel(int playerCount);
bool createAudioChannelFromAsset(const char* fname, struct AudioChannel* channel);
bool setAudioChannelState(struct AudioChannel* channel, SLuint32 state);
SLuint32 getAudioChannelState(struct AudioChannel* channel);
struct AudioChannel* getAudioChannel(int index);
void closeAudioChannel(struct AudioChannel* channel);
void closeAudioChannels();

bool playAudioChannel(struct AudioChannel* channel);
bool pauseAudioChannel(struct AudioChannel* channel);
bool stopAudioChannel(struct AudioChannel* channel);
struct AudioChannel* loadAudio(int channelIndex, const char* name);
