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
#import "squirrel.h"

void initDrawableFunctions();

SQInteger emoDrawableCreateSprite(HSQUIRRELVM v);
SQInteger emoDrawableCreateLine(HSQUIRRELVM v);
SQInteger emoDrawableCreateSpriteSheet(HSQUIRRELVM v);
SQInteger emoDrawableCreateLiquidSprite(HSQUIRRELVM v);

SQInteger emoDrawableCreateSnapshot(HSQUIRRELVM v);
SQInteger emoDrawableCreateMapSprite(HSQUIRRELVM v);
SQInteger emoDrawableLoadMapSprite(HSQUIRRELVM v);
SQInteger emoDrawableAddTileRow(HSQUIRRELVM v);
SQInteger emoDrawableClearTiles(HSQUIRRELVM v);
SQInteger emoDrawableSetTileAt(HSQUIRRELVM v);
SQInteger emoDrawableGetTileAt(HSQUIRRELVM v);
SQInteger emoDrawableGetTileIndexAtCoord(HSQUIRRELVM v);
SQInteger emoDrawableGetTilePositionAtCoord(HSQUIRRELVM v);
SQInteger emoDrawableUseMeshMapSprite(HSQUIRRELVM v);

SQInteger emoDrawableCreateFontSprite(HSQUIRRELVM v);
SQInteger emoDrawableSetFontSpriteParam(HSQUIRRELVM v);
SQInteger emoDrawableReloadFontSprite(HSQUIRRELVM v);

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

SQInteger emoSetStageColor(HSQUIRRELVM v);
SQInteger emoSetStageSize(HSQUIRRELVM v);
SQInteger emoSetViewport(HSQUIRRELVM v);
SQInteger emoGetWindowWidth(HSQUIRRELVM v);
SQInteger emoGetWindowHeight(HSQUIRRELVM v);
SQInteger emoDrawablePauseAt(HSQUIRRELVM v);
SQInteger emoDrawablePause(HSQUIRRELVM v);
SQInteger emoDrawableStop(HSQUIRRELVM v);
SQInteger emoDrawableAnimate(HSQUIRRELVM v);
SQInteger emoDrawableIsAnimationFinished(HSQUIRRELVM v);

SQInteger emoDrawableSelectFrame(HSQUIRRELVM v);

SQInteger emoDrawableGetX(HSQUIRRELVM v);
SQInteger emoDrawableGetY(HSQUIRRELVM v);
SQInteger emoDrawableGetZ(HSQUIRRELVM v);
SQInteger emoDrawableGetWidth(HSQUIRRELVM v);
SQInteger emoDrawableGetHeight(HSQUIRRELVM v);

SQInteger emoDrawableGetScaleX(HSQUIRRELVM v);
SQInteger emoDrawableGetScaleY(HSQUIRRELVM v);
SQInteger emoDrawableGetAngle(HSQUIRRELVM v);

SQInteger emoDrawableSetX(HSQUIRRELVM v);
SQInteger emoDrawableSetY(HSQUIRRELVM v);
SQInteger emoDrawableSetZ(HSQUIRRELVM v);
SQInteger emoDrawableSetWidth(HSQUIRRELVM v);
SQInteger emoDrawableSetHeight(HSQUIRRELVM v);
SQInteger emoDrawableSetSize(HSQUIRRELVM v);
SQInteger emoDrawableGetFrameIndex(HSQUIRRELVM v);
SQInteger emoDrawableGetFrameCount(HSQUIRRELVM v);

SQInteger emoDrawableSetLinePosition(HSQUIRRELVM v);

SQInteger emoDrawableLoadSnapshot(HSQUIRRELVM v);
SQInteger emoDrawableRemoveSnapshot(HSQUIRRELVM v);
SQInteger emoDrawableDisableSnapshot(HSQUIRRELVM v);

SQInteger emoDrawableUpdateLiquidTextureCoords(HSQUIRRELVM v);
SQInteger emoDrawableUpdateLiquidSegmentCoords(HSQUIRRELVM v);
SQInteger emoDrawableUpdateLiquidSegmentCount(HSQUIRRELVM v);
SQInteger emoDrawableGetLiquidSegmentCount(HSQUIRRELVM v);
