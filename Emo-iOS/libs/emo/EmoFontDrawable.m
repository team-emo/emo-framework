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
#import "EmoFontDrawable.h"
#import "Util.h"

@implementation EmoFontDrawable
@synthesize fontSize, fontFace, isBold, isItalic;
@synthesize param1, param2, param3, param4, param5, param6;

-(void)initDrawable {
	[super initDrawable];
    
    fontSize = 0;
    isBold   = FALSE;
    isItalic = FALSE;
}

-(void)dealloc {
    [fontFace release];
    [param1 release];
    [param2 release];
    [param3 release];
    [param4 release];
    [param5 release];
    [param6 release];
    
    [super dealloc];
}

-(void)loadTextBitmap {
    [texture freeData];
    
    UIFont* font = [UIFont systemFontOfSize:[UIFont systemFontSize]];
    
    if ([fontFace length] > 0) {
        NSInteger size = fontSize > 0 ? fontSize : [UIFont systemFontSize];
        font = [UIFont fontWithName:fontFace size:size];
    } else if (fontSize > 0) {
        font = [UIFont systemFontOfSize:fontSize];
    }
    
    NSString* text = @" ";
    
    // extract property name
    NSString* propName = 
    [name substringFromIndex:[name rangeOfString:@"::"].location+2];
    
    // retrieve property value
    NSDictionary* plist = [NSDictionary dictionaryWithContentsOfFile:[
                                                                      [NSBundle mainBundle] pathForResource:@"strings" ofType:@"plist"]];  
    if ([plist objectForKey:propName] != nil) {
        NSString* formatStr = [[plist objectForKey:propName] 
                               stringByReplacingOccurrencesOfString:@"%s" withString:@"%@"];
        
        text = [NSString stringWithFormat:formatStr,
                param1, param2, param3, param4, param5, param6];
    }
    
    CGSize textSize = [text sizeWithFont:font]; 
    
    int textWidth  = textSize.width;
    int textHeight = textSize.height;
    
    GLubyte *bitmap = (GLubyte *)malloc(textWidth * textHeight * 4);
    memset(bitmap, 0, textWidth * textHeight * 4);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(
                                                 bitmap, textWidth, textHeight,
                                                 8, textWidth * 4, colorSpace,
                                                 kCGImageAlphaPremultipliedLast);
    
    UIGraphicsPushContext(context);
    
    [[UIColor whiteColor] set];
    [text drawAtPoint:CGPointMake(0, 0) withFont:font];
    
    UIGraphicsPopContext();
    
    CGColorSpaceRelease(colorSpace);
    CGContextRelease(context);
    
    texture.width  = textWidth;
    texture.height = textHeight;
    texture.glWidth  = nextPowerOfTwo(texture.width);
    texture.glHeight = nextPowerOfTwo(texture.height);
    texture.data   = bitmap;
    texture.hasAlpha = TRUE;
    texture.freed = FALSE;
    
}
@end
