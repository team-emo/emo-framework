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
void getVec2Instance(HSQUIRRELVM v, int idx, b2Vec2* vec2);
void getVec2InstanceFromMember(HSQUIRRELVM v, int idx, const char* member, b2Vec2* vec2);
void pushVec2(HSQUIRRELVM v, b2Vec2 vec2);
void getBodyDefInstance(HSQUIRRELVM v, int idx, b2BodyDef* def);
void getFixtureDefInstance(HSQUIRRELVM v, int idx, b2FixtureDef* def);
void getDistanceJointDef(HSQUIRRELVM v, int idx, b2DistanceJointDef* def);
void getFrictionJointDef(HSQUIRRELVM v, int idx, b2FrictionJointDef* def);
void getGearJointDef(HSQUIRRELVM v, int idx, b2GearJointDef* def);
void getLineJointDef(HSQUIRRELVM v, int idx, b2LineJointDef* def);
void getPrismaticJointDef(HSQUIRRELVM v, int idx, b2PrismaticJointDef* def);
void getPulleyJointDef(HSQUIRRELVM v, int idx, b2PulleyJointDef* def);
void getRevoluteJointDef(HSQUIRRELVM v, int idx, b2RevoluteJointDef* def);
void getWeldJointDef(HSQUIRRELVM v, int idx, b2WeldJointDef* def);
