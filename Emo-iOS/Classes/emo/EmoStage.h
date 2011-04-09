@interface EmoStage : NSObject {
	unsigned int vbo[2];
	BOOL loaded;
	float indices[4];
	float positions[12];
}

-(BOOL)loadBuffer;
-(BOOL)onDrawFrame:(NSTimeInterval)dt;
-(void)unloadBuffer;
@end
