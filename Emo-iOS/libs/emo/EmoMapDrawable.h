#import <Foundation/Foundation.h>
#import "EmoDrawable.h"

@interface EmoMapDrawable : EmoDrawable {
	EmoDrawable* child;
	NSInteger columns;
	NSInteger rows;
	NSMutableArray* tiles;
}
-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage;
-(BOOL)bindVertex;

-(void)setChild:(EmoDrawable*)child;
-(EmoDrawable*)getChild;

-(void)addRow:(NSMutableArray*)rowdata;
-(BOOL)clearTiles;
-(BOOL)setTileAt:(NSInteger)row column:(NSInteger)column value:(NSInteger)value;
-(NSInteger)getTileAt:(NSInteger) row column:(NSInteger)column;
-(NSArray*)getTileIndexAtCoord:(float) x y:(float)y;
-(NSArray*)getTilePositionAtCoord:(float)x y:(float)y;

@end
