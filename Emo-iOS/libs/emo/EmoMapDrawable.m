// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#import "EmoMapDrawable.h"
#import "Util.h"
#import "Constants.h"
#include <EmoEngine_glue.h>

@interface EmoMapDrawable (PrivateMethods)
-(BOOL)isInRange:(EmoStage*)stage;
-(void)createMeshIndiceBuffer;
-(void)createMeshPositionBuffer;
-(void)createMeshTextureBuffer;
-(void)unbindMeshVertex;
-(NSInteger)getMeshIndiceCount;
@end

@implementation EmoMapDrawable
@synthesize useMesh;

- (id)init {
    self = [super init];
    if (self != nil) {
		tiles = [[NSMutableArray alloc] init];
        loaded = FALSE;
        useMesh = FALSE;
        meshLoaded = FALSE;
        meshIndiceCount = 0;
    }
    return self;
}

-(void)dealloc {
	[tiles release];
    [self unbindMeshVertex];
	[super dealloc];
}

-(void)doUnload {
    [self unbindMeshVertex];
    [super doUnload];
}

-(void)unbindMeshVertex {
    if (useMesh && meshLoaded) {
        free(meshIndices);
        free(meshPositions);
        free(meshTexCoords);
        
        glDeleteBuffers(3, mesh_vbos);
        
        meshLoaded = FALSE;
    }
}

-(BOOL)bindVertex {
    if (useMesh && !meshLoaded) {
        [self unbindMeshVertex];
        
        glGenBuffers(3, mesh_vbos);
        
        [self createMeshIndiceBuffer];
        [self createMeshPositionBuffer];
        [self createMeshTextureBuffer];
            
        meshLoaded = TRUE;
        loaded = [child bindVertex];
    } else if (!useMesh) {
        loaded = [child bindVertex];
    }
	return loaded;
}

-(void)createMeshPositionBuffer {
    int vertexCount = rows * columns * POINTS_3D_SIZE * POINTS_RECTANGLE;
    
    meshPositions = (float *)malloc(sizeof(float) * vertexCount);
    
    int childWidth  = [child getScaledWidth];
    int childHeight = [child getScaledHeight];

    int index = 0;
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            int pX = childWidth  * column;
            int pY = childHeight * row;
            
            meshPositions[index++] = pX;
            meshPositions[index++] = pY;
            meshPositions[index++] = 0;
            
            meshPositions[index++] = pX;
            meshPositions[index++] = pY + childHeight;
            meshPositions[index++] = 0;
            
            meshPositions[index++] = pX + childWidth;
            meshPositions[index++] = pY;
            meshPositions[index++] = 0;
            
            meshPositions[index++] = pX + childWidth;
            meshPositions[index++] = pY + childHeight;
            meshPositions[index++] = 0;
        }
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh_vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount, meshPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    
	printGLErrors("Could not create position buffer");
}

-(NSInteger)getMeshIndiceCount {
    return rows * columns * POINTS_2D_SIZE * POINTS_TRIANGLE;
}

-(void)createMeshIndiceBuffer {
    meshIndiceCount = [self getMeshIndiceCount];
    
    meshIndices = (short *)malloc(sizeof(short) * meshIndiceCount);
    
    int index = 0;
    int trIndex = 0;
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            meshIndices[index++] = (short)(trIndex + 0);
            meshIndices[index++] = (short)(trIndex + 1);
            meshIndices[index++] = (short)(trIndex + 2);
            
            meshIndices[index++] = (short)(trIndex + 2);
            meshIndices[index++] = (short)(trIndex + 1);
            meshIndices[index++] = (short)(trIndex + 3);
            
            trIndex += 4;
        }
    }
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * meshIndiceCount, meshIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
	printGLErrors("Could not create indice buffer");
}

-(void)createMeshTextureBuffer {
    int coordCount = rows * columns * POINTS_2D_SIZE * POINTS_RECTANGLE;
    
    meshTexCoords = (float *)malloc(sizeof(float) * coordCount);
    
    int index = 0;
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            NSInteger frameIndex = [self getTileAt:row column:column];
            if ([child setFrameIndex:frameIndex force:TRUE]) {
                meshTexCoords[index++] = [child getTexCoordStartX];
                meshTexCoords[index++] = [child getTexCoordStartY];
            
                meshTexCoords[index++] = [child getTexCoordStartX];
                meshTexCoords[index++] = [child getTexCoordEndY];
            
                meshTexCoords[index++] = [child getTexCoordEndX];
                meshTexCoords[index++] = [child getTexCoordStartY];
            
                meshTexCoords[index++] = [child getTexCoordEndX];
                meshTexCoords[index++] = [child getTexCoordEndY];
            } else {
                // no texture for this cell
                meshTexCoords[index++] = 0;
                meshTexCoords[index++] = 0;
                
                meshTexCoords[index++] = 0;
                meshTexCoords[index++] = 0;
                
                meshTexCoords[index++] = 0;
                meshTexCoords[index++] = 0;
                
                meshTexCoords[index++] = 0;
                meshTexCoords[index++] = 0;
            }
        }
    }
    
	glEnable(GL_TEXTURE_2D);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coordCount, meshTexCoords, GL_STATIC_DRAW);
    
	printGLErrors("Could not create texture buffer");
}

-(BOOL) isInRange:(EmoStage*)stage {
	if (self.x > 0 && self.x  > stage.width)  return FALSE;
	if (self.y > 0 && self.y  > stage.height) return FALSE;
	if (self.x < 0 && -self.x > [child getScaledWidth]  * columns) return FALSE;
	if (self.y < 0 && -self.y > [child getScaledHeight] * rows) return FALSE;
    
    return TRUE;
}

-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage {
	
    if (![self isInRange:stage]) return FALSE;
    
    if (useMesh) {
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity (); 
        
        // update colors
        glColor4f(param_color[0], param_color[1], param_color[2], param_color[3]);
        
        // update position
        glTranslatef(x, y, 0);
        
        // bind vertex positions
        glBindBuffer(GL_ARRAY_BUFFER, mesh_vbos[0]);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        
        // bind a texture
        if (child.hasTexture) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, child.texture.textureId);
            
            // bind texture coords
            glBindBuffer(GL_ARRAY_BUFFER, mesh_vbos[2]);
            glTexCoordPointer(2, GL_FLOAT, 0, 0);
        } else {
            glDisable(GL_TEXTURE_2D);
        }
        
        // bind indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vbos[1]);
        
        // draw sprite
        glDrawElements(GL_TRIANGLES, meshIndiceCount, GL_UNSIGNED_SHORT, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        return TRUE;
    }
	
    int rowCount = (int)ceil([self getScaledHeight] / (double)[child getScaledHeight]);
    int columnCount = (int)ceil([self getScaledWidth]  / (double)[child getScaledWidth]);
    
	int firstColumn = max(0, min(columns, (-self.x / [child getScaledWidth])));
	int lastColumn  = min(firstColumn + columnCount + 1, columns);
	
	int firstRow = max(0, min(rows, (-self.y / [child getScaledHeight])));
	int lastRow  = min(firstRow + rowCount + 1, rows);
	
	for (int i = firstRow; i < lastRow; i++) {
		for (int j = firstColumn; j < lastColumn; j++) {
			if (([tiles count]) <= i || [[tiles objectAtIndex:i] count] <= j) break;
			child.x = j * [child getScaledWidth]  - -self.x;
			child.y = i * [child getScaledHeight] - -self.y;
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
	
	width  = child.width  * columns;
	height = child.height * rows;
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
	int row = (int)floor((_y - self.y) / (double)[child getScaledHeight]);
	int col = (int)floor((_x - self.x) / (double)[child getScaledWidth]);
	return [NSArray arrayWithObjects: [NSNumber numberWithInt:row], [NSNumber numberWithInt:col], nil];
}

-(NSArray*)getTilePositionAtCoord:(float)_x y:(float)_y {
	NSArray* tile = [self getTileIndexAtCoord:_x y:_y];
	int px = [[tile objectAtIndex:1] intValue] * [child getScaledWidth]  + x;
	int py = [[tile objectAtIndex:0] intValue] * [child getScaledHeight] + y;
	return [NSArray arrayWithObjects: [NSNumber numberWithInt:px], [NSNumber numberWithInt:py], nil];
}

@end
