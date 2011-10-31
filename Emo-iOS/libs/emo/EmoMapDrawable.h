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
#import "Foundation/Foundation.h"
#import "EmoDrawable.h"

@interface EmoMapDrawable : EmoDrawable {
	EmoDrawable* child;
	NSInteger columns;
	NSInteger rows;
	NSMutableArray* tiles;
    
    BOOL useMesh;
    BOOL meshLoaded;
    short* meshIndices;
    float* meshPositions;
    float* meshTexCoords;
    GLuint mesh_vbos[3];
    NSInteger meshIndiceCount;
}
@property (readwrite) BOOL useMesh;

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
-(void)doUnload:(BOOL)doAll;
@end
