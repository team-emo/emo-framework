@interface EmoStage : NSObject {
	unsigned int vbo[2];
	BOOL loaded;
	float indices[4];
	float positions[12];
}

-(BOOL)loadBuffer;
-(void)unloadBuffer;
@end
