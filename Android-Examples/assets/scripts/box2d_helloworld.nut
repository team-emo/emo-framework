/*
 * This example is a Squirrel port of Box2D HelloWorld example.
 * There are no graphics for this example.
 *
 * FYI: Box2D HelloWorld example by C++
 * http://code.google.com/p/box2d/source/browse/trunk/Box2D/HelloWorld/HelloWorld.cpp
 */

/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

emo.Runtime.import("physics.nut");

// This is a simple example of building and running a simulation
// using Box2D. Here we create a large ground box and a small dynamic
// box.
// There are no graphics for this example. Box2D is meant to be used
// with your rendering engine in your game engine.
function emo::onLoad() {
    local gravity = emo.Vec2(0, -10);
    local world = emo.physics.World(gravity, true);
    
    local groundBodyDef = emo.physics.BodyDef();
    groundBodyDef.position = emo.Vec2(0, -10);
    
    local groundBody = world.createBody(groundBodyDef);
    
    local groundBox = emo.physics.PolygonShape();
    groundBox.setAsBox(50, 10);
    groundBody.createFixture(groundBox, 0);

    local bodyDef = emo.physics.BodyDef();
    bodyDef.type = PHYSICS_BODY_TYPE_DYNAMIC;
    bodyDef.position = emo.Vec2(0, 4);
    local body = world.createBody(bodyDef);
    
    local dynamicBox = emo.physics.PolygonShape();
    dynamicBox.setAsBox(1, 1);
    
    local fixtureDef = emo.physics.FixtureDef();
    fixtureDef.shape = dynamicBox;
    fixtureDef.density  = 1;
    fixtureDef.friction = 0.3;
    body.createFixture(fixtureDef);
    
    local timeStep = 1.0 / 60.0;
    local velocityIterations = 6;
    local positionIterations  = 2;
    
    for (local i = 0; i < 60; ++i) {
        world.step(timeStep, velocityIterations, positionIterations);
        world.clearForces();
        
        local position = body.getPosition();
        local angle    = body.getAngle();
        
        print(format("%4.2f %4.2f %4.2f", position.x, position.y, angle));
    }
}
