#include <squirrel.h>
#include <EmoImage.h>

@interface EmoDrawable : NSObject {
	unsigned int vbo[1];
	NSString* name;
	float x;
	float y;
	float z;
	float width;
	float height;
	BOOL hasTexture;
	
    float      vertex_tex_coords[8];
	
    float      param_rotate[4];
    float      param_scale[4];
    float      param_color[4];
	
	EmoImage*  texture;
}
@property (copy, readwrite) NSString* name;
@property (readwrite) float x;
@property (readwrite) float y;
@property (readwrite) float z;
@property (readwrite) float width;
@property (readwrite) float height;
@property (readwrite) BOOL hasTexture;
@property (assign, readwrite, nonatomic) EmoImage* texture;

-(void)doUnload;
-(void)initDrawable;
-(BOOL)bindVertex;
-(void)updateKey:(char*)key;
-(void)setScale:(NSInteger)index withValue:(float)value;
-(void)setRotate:(NSInteger)index withValue:(float)value;
-(void)setColor:(NSInteger)index withValue:(float)value;
-(float)getTexCoordStartX;
-(float)getTexCoordStartY;
-(float)getTexCoordEndX;
-(float)getTexCoordEndY;
-(BOOL)onDrawFrame:(NSTimeInterval)dt;
@end

SQInteger emoDrawableCreateSprite(HSQUIRRELVM v);
SQInteger emoDrawableLoad(HSQUIRRELVM v);
SQInteger emoDrawableMove(HSQUIRRELVM v);
SQInteger emoDrawableScale(HSQUIRRELVM v);
SQInteger emoDrawableRotate(HSQUIRRELVM v);
SQInteger emoDrawableColor(HSQUIRRELVM v);
SQInteger emoDrawableUnload(HSQUIRRELVM v);
SQInteger emoSetOnDrawInterval(HSQUIRRELVM v);