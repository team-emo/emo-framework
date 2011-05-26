#import "EmoMapDrawable.h"
#import "Util.h"

@implementation EmoMapDrawable
- (id)init {
    self = [super init];
    if (self != nil) {
		tiles = [[NSMutableArray alloc] init];
    }
    return self;
}

-(void)dealloc {
	[tiles release];
	[super dealloc];
}

-(BOOL)bindVertex {
	loaded = [child bindVertex];
	return loaded;
}

-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage {
	
	int invertX = -self.x;
	int invertY = -self.y;
	
	if (self.x > 0 && self.x  > [self getScaledWidth])  return FALSE;
	if (self.y > 0 && self.y  > [self getScaledHeight]) return FALSE;
	if (self.x < 0 && invertX > [child getScaledWidth]  * columns) return FALSE;
	if (self.y < 0 && invertY > [child getScaledHeight] * rows) return FALSE;
	
	int columnCount = (int)ceil([self getScaledWidth]  / (double)[child getScaledWidth]);
	int rowCount    = (int)ceil([self getScaledHeight] / (double)[child getScaledHeight]);
	
	int firstColumn = max(0, min(columns, (invertX / [child getScaledWidth])));
	int lastColumn  = min(firstColumn + columnCount + 1, columns);
	
	int firstRow = max(0, min(rows, (invertY / [child getScaledHeight])));
	int lastRow  = min(firstRow + rowCount + 1, rows);
	
	for (int i = firstRow; i < lastRow; i++) {
		for (int j = firstColumn; j < lastColumn; j++) {
			if (([tiles count]) <= i || [[tiles objectAtIndex:i] count] <= j) break;
			child.x = j * [child getScaledWidth]  - invertX;
			child.y = i * [child getScaledHeight] - invertY;
			if ([self getTileAt:i column:j] < 0) continue;
			
			[child setFrameIndex:[self getTileAt:i column:j]];
			[child onDrawFrame:dt withStage:stage];
		}
	}
	
	return TRUE;
}

-(void)setChild:(EmoDrawable*)_child {
	child = _child;
}

-(EmoDrawable*)getChild {
	return child;
}

-(void)addRow:(NSMutableArray*)rowdata {
	[tiles addObject:rowdata];
	columns = [rowdata count];
	rows    = [tiles count];
}

-(BOOL)clearTiles {
	[tiles removeAllObjects];
	return TRUE;
}
-(BOOL)setTileAt:(NSInteger)row column:(NSInteger)column value:(NSInteger)value {
	if (row    >= [tiles count]) return FALSE;
	if (column >= [[tiles objectAtIndex:row] count]) return FALSE;

	NSMutableArray* col_data = [tiles objectAtIndex:row];
	[col_data replaceObjectAtIndex:column withObject:[NSNumber numberWithInt:value]];
	
	return TRUE;
}

-(NSInteger)getTileAt:(NSInteger) row column:(NSInteger)column {
	if (row    >= [tiles count]) return -1;
	if (column >= [[tiles objectAtIndex:row] count]) return -1;
	return [[[tiles objectAtIndex:row] objectAtIndex:column] intValue];
}

-(NSArray*)getTileIndexAtCoord:(float)_x y:(float)_y {
	int col = (int)floor((_x - self.x) / (double)[child getScaledWidth]);
	int row = (int)floor((_y - self.y) / (double)[child getScaledHeight]);
	return [NSArray arrayWithObjects: [NSNumber numberWithInt:col], [NSNumber numberWithInt:row], nil];
}

-(NSArray*)getTilePositionAtCoord:(float)_x y:(float)_y {
	NSArray* tile = [self getTileIndexAtCoord:_x y:_y];
	int col = [[tile objectAtIndex:0] intValue] * [child getScaledWidth];
	int row = [[tile objectAtIndex:1] intValue] * [child getScaledHeight];
	return [NSArray arrayWithObjects: [NSNumber numberWithInt:col], [NSNumber numberWithInt:row], nil];
}

@end