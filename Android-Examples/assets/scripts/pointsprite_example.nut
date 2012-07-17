local stage = emo.Stage();
local event = emo.Event();

const NUMBER_OF_SPARKS = 18;
const NUMBER_OF_RINGS  = 6;

/*
 * Bullet class that has angle, radius and radius step.
 */
class Bullet extends emo.Vec2 {
    angle  = null;
    radius = null;
    rstep  = null;
    function constructor(_x, _y, _angle = 0, _radius = 1, _rstep = 2) {
        base.constructor(_x, _y);
        angle  = emo.toRadian(_angle);
        radius = _radius;
        rstep  = _rstep;
    }
}

/*
 * This example shows single point sprite.
 * 
 * The PointSprite is a set of points that have same texture.
 * Each points can not be rotated and scaled. The texture should be square.
 * This sprite is much faster than normal sprite if you need a lot of points
 * with same texture (i.e. bullets on bullet-hell like shooter games)
 */
class Main {

    spark       = null;
    sparkCenter = null;
    points      = [];

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 
        
        local centerX = stage.getWindowWidth()  * 0.5;
        local centerY = stage.getWindowHeight() * 0.5;
        
        sparkCenter = emo.Vec2(centerX, centerY);
        
        // initialize points
        for (local ring = 0; ring < NUMBER_OF_RINGS; ring++) {
            for (local i = 0; i < NUMBER_OF_SPARKS; i++) {
                points.append(Bullet(centerX, centerY,
                        i * 360 / NUMBER_OF_SPARKS, ring + 2, ring + 2));
            }
        }

        // create point sprite with texture
        spark = emo.PointSprite("flare.png", points)
        spark.color(1, 1, 0);
        spark.load();
        
        // print frame per second on every 5 seconds
        event.enableOnFpsCallback(5000);
        
        // onDrawFrame(dt) will be called on every 33 milliseconds
        event.enableOnDrawCallback(33);
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("onGainedFocus");
    }

    /*
     * Called when the app has lost focus
     */
    function onLostFocus() {
        print("onLostFocus"); 
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
        
        // remove sprite from the screen
        spark.remove();
    }

    /*
     * Enabled after onDrawCalleback event is enabled by enableOnDrawCallback
     * dt parameter is a delta time (millisecond)
     */
    function onDrawFrame(dt) {
        for (local i = 0; i < NUMBER_OF_RINGS * NUMBER_OF_SPARKS; i++) {
            points[i] = moveBullet(points[i]); 
        }
        spark.setPoints(points);
    }
    
    function moveBullet(bullet) {
        bullet.angle = bullet.angle + emo.toRadian(5);
        bullet.x = sparkCenter.x + (bullet.radius * cos(bullet.angle));
        bullet.y = sparkCenter.y + (bullet.radius * sin(bullet.angle));
        bullet.radius = bullet.radius + bullet.rstep;
        
        if (bullet.radius > sparkCenter.x || bullet.radius < 0) bullet.rstep = -bullet.rstep;
        
        return bullet;
    }
    
    function onFps(fps) {
        print(format("FPS: %4.2f", fps));
    }
}

function emo::onLoad() {
    stage.load(Main());
}
