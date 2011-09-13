local stage = emo.Stage();

local text = null;

/*
 * This example shows moving in/out scene transition
 */
class SceneA {

    panel = null;

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad");

        panel = emo.Rectangle();
        panel.setSize(stage.getWindowWidth(), stage.getWindowHeight());
        panel.color(1, 0, 0);
        panel.setZ(0);
        panel.load();

        text.setText("SCENE A - TAP");
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
        panel.remove();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            // move out current scene then fade in next scene
            local currentSceneModifier = emo.MoveModifier(
                    [0, 0],
                    [0, stage.getWindowHeight()],
                    1000, emo.easing.BackOut);
            local nextSceneModifier    = emo.AlphaModifier(0, 1, 1000, emo.easing.CubicOut);
            stage.load(SceneB(), currentSceneModifier, nextSceneModifier);
        }
    }
}
class SceneB {

    panel = null;

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad");

        panel = emo.Rectangle();
        panel.setSize(stage.getWindowWidth(), stage.getWindowHeight());
        panel.color(0, 0, 1);
        panel.setZ(0);
        panel.load();

        text.setText("SCENE B - TAP");
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
        panel.remove();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            // fade out current scene and move in next scene
            local currentSceneModifier = emo.AlphaModifier(1, 0, 1000, emo.easing.CubicOut);
            local nextSceneModifier = emo.MoveModifier(
                    [stage.getWindowWidth(), 0],
                    [0, 0],
                    1000, emo.easing.BackOut);
            stage.load(SceneC(), currentSceneModifier, nextSceneModifier);
        }
    }
}
class SceneC {

    panel = null;

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad");

        panel = emo.Rectangle();
        panel.setSize(stage.getWindowWidth(), stage.getWindowHeight());
        panel.color(0, 1, 0);
        panel.setZ(0);
        panel.load();

        text.setText("SCENE C - TAP");
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
        panel.remove();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            // fade out current scene and move in next scene
            local currentSceneModifier = emo.AlphaModifier(1, 0, 1000, emo.easing.CubicOut);
            local nextSceneModifier = emo.MoveModifier(
                    [0, stage.getWindowHeight()],
                    [0, 0],
                    1000, emo.easing.BackOut);
            stage.load(SceneA(), currentSceneModifier, nextSceneModifier);
        }
    }
}

function emo::loadText() {
    // Below statements is an example of multiple screen density support.
    // (i.e. Retina vs non-Retina, cellular phone vs tablet device).
    if (stage.getWindowWidth() > 320) {
        // if the screen has large display, scale contents twice
        // that makes the stage size by half.
        // This examples shows how to display similar-scale images
        // on Retina and non-Retina display.
        stage.setContentScale(stage.getWindowWidth() / 320.0);
    }
        
    // 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    text.setText("SCENE X - TAP");
    text.setZ(10);
        
    // move sprite to the center of the screen
    local x = (stage.getWindowWidth()  - text.getScaledWidth())  / 2;
    local y = (stage.getWindowHeight() - text.getScaledHeight()) / 2;

    text.move(x, y);

    // load sprite to the screen
    text.load();
}

function emo::onLoad() {
    loadText();
    stage.load(SceneA());
}
