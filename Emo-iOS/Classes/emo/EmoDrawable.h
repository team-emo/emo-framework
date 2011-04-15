#include <squirrel.h>
#include <EmoImage.h>
#include <EmoStage.h>

@interface EmoDrawable : NSObject {
	GLuint* frames_vbos;
	NSString* name;
	float x;
	float y;
	float z;
	NSInteger width;
	NSInteger height;
	
	BOOL hasTexture;
	BOOL loaded;
	BOOL hasSheet;
	
    float      vertex_tex_coords[8];
	
    float      param_rotate[4];
    float      param_scale[4];
    float      param_color[4];
	
	NSInteger frameCount;
	NSInteger frame_index;
	NSInteger border;
	NSInteger margin;
	
	EmoImage*  texture;
}
@property (copy, readwrite) NSString* name;
@property (readwrite) float x;
@property (readwrite) float y;
@property (readwrite) float z;
@property (readwrite) NSInteger width;
@property (readwrite) NSInteger height;
@property (readwrite) BOOL hasTexture;
@property (readwrite) BOOL hasSheet;
@property (readwrite) NSInteger frameCount;
@property (readwrite) NSInteger frame_index;
@property (readwrite) NSInteger border;
@property (readwrite) NSInteger margin;
@property (assign, readwrite, nonatomic) EmoImage* texture;

-(void)doUnload;
-(void)initDrawable;
-(void)createTextureBuffer;
-(BOOL)bindVertex;
-(void)updateKey:(char*)key;
-(void)setScale:(NSInteger)index withValue:(float)value;
-(void)setRotate:(NSInteger)index withValue:(float)value;
-(void)setColor:(NSInteger)index withValue:(float)value;
-(float)getColor:(NSInteger)index;
-(float)getTexCoordStartX;
-(float)getTexCoordStartY;
-(float)getTexCoordEndX;
-(float)getTexCoordEndY;
-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage;
@end

SQInteger emoDrawableCreateSprite(HSQUIRRELVM v);
SQInteger emoDrawableCreateSpriteSheet(HSQUIRRELVM v);
SQInteger emoDrawableLoad(HSQUIRRELVM v);
SQInteger emoDrawableMove(HSQUIRRELVM v);
SQInteger emoDrawableScale(HSQUIRRELVM v);
SQInteger emoDrawableRotate(HSQUIRRELVM v);
SQInteger emoDrawableColor(HSQUIRRELVM v);
SQInteger emoDrawableRemove(HSQUIRRELVM v);
SQInteger emoSetOnDrawInterval(HSQUIRRELVM v);
SQInteger emoDrawableShow(HSQUIRRELVM v);
SQInteger emoDrawableHide(HSQUIRRELVM v);
SQInteger emoDrawableColorRed(HSQUIRRELVM v);
SQInteger emoDrawableColorGreen(HSQUIRRELVM v);
SQInteger emoDrawableColorBlue(HSQUIRRELVM v);
SQInteger emoDrawableColorAlpha(HSQUIRRELVM v);

SQInteger emoSetStageSize(HSQUIRRELVM v);
SQInteger emoSetViewport(HSQUIRRELVM v);
SQInteger emoGetWindowWidth(HSQUIRRELVM v);
SQInteger emoGetWindowHeight(HSQUIRRELVM v);
