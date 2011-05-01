local stage   = emo.Stage();

class Main {

    rectangle = emo.Rectangle();

    function onLoad() {
        print("onLoad"); 

		local width  = 60;
		local height = 60;
		
		local x = (stage.getWindowWidth()  - width)  / 2;
		local y = (stage.getWindowHeight() - height) / 2;
		
		rectangle.move(x, y);
		rectangle.color(1, 0, 0, 1);
        rectangle.setSize(width, height);

        rectangle.load();
    }

    function onGainedFocus() {
        print("onGainedFocus");
    }

    function onLostFocus() {
        print("onLostFocus"); 
    }

    function onDispose() {
        print("onDispose"); 
    }

}

function emo::onLoad() {
    stage.load(Main());
}
