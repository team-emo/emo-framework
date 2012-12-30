local stage = emo.Stage();

/*
 * This example shows one rectangular shape
 * that saves the latest position and restore it when loaded.
 */
 
const KEY_RECTANGLE_X = "RECTANGLE_X";
const KEY_RECTANGLE_Y = "RECTANGLE_Y";

class Main {

    rectangle   = emo.Rectangle();
    preference  = emo.Preference();
    database    = emo.Database();

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 

        local width  = 60;
        local height = 60;
        
        // move rectangle to the center of the screen
        local x = (stage.getWindowWidth()  - width)  / 2;
        local y = (stage.getWindowHeight() - height) / 2;
        
        rectangle.move(x, y);
        
        // set rectangle color(RGBA)
        rectangle.color(1, 0, 0, 1);

        // initial rectangle size is 1x1 so update the size to 60x60
        rectangle.setSize(width, height);

        // load rectangle to the screen
        rectangle.load();
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("onGainedFocus");
        
        // since we'are not sure whether the initial preference table is already created or not,
        // we use openOrCreate() that creates new preference table if the table does not exist.
        if (preference.openOrCreate() == EMO_NO_ERROR) {

            // detabase are stored at the following directory:
            print(format("STORED AT: %s", database.getPath(DEFAULT_DATABASE_NAME)));
        
            // retrieve the latest position.
            local x = preference.get(KEY_RECTANGLE_X);
            local y = preference.get(KEY_RECTANGLE_Y);
            
            // we have to close the database after retrieving values.
            preference.close();
            
            // NOTE: all preference values are 'string'
            //       so we have to restore numeric values now.
            if (x.len() > 0 && y.len() > 0) {
                print(format("LOAD: RECTANGLE_X = %s", x));
                print(format("LOAD: RECTANGLE_Y = %s", y));
            
                print("RESTORE THE POSITION");
                rectangle.move(x.tointeger(), y.tointeger());
            }
        } else {
            print("failed to store the position");
            print(format("ERROR CODE:    %d", database.getLastError()));
            print(format("ERROR MESSAGE: %s", database.getLastErrorMessage()));
        }
        
    }

    /*
     * Called when the app has lost focus
     */
    function onLostFocus() {
        print("onLostFocus"); 
        // now we are sure that the preference table is created,
        // we use preference.open().
        if (preference.open() == EMO_NO_ERROR) {
            // store the latest position to database.
            preference.set(KEY_RECTANGLE_X, rectangle.getX());
            preference.set(KEY_RECTANGLE_Y, rectangle.getY());
            
            // list all of the preference keys.
            local keys = preference.keys();
            for (local i = 0; i < keys.len(); i++) {
                print(format("SAVED: %s = %s", keys[i], preference.get(keys[i])));
            }
            
            // if you want to delete the preference value, uncomment below.
            //preference.del(KEY_RECTANGLE_X);
            //preference.del(KEY_RECTANGLE_Y);
            
            preference.close();
    
            // if you want to delete emo framework database completely, uncomment below.
            // NOTE: by using deleteDatabase, all of the tables used by emo framework are deleted.
            //       this function might be useful for developing or testing your program.
            // database.deleteDatabase(DEFAULT_DATABASE_NAME);
        } else {
            print("failed to store the latest position");
        }
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
        
        // remove rectangle from the screen
        rectangle.remove();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        // move rectangle to the position on the motion event 
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            local x = mevent.getX() - (rectangle.getWidth()  / 2);
            local y = mevent.getY() - (rectangle.getHeight() / 2);
            rectangle.move(x, y);
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
