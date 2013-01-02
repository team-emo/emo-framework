class Main {	
    // Create 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    
    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 
        
        text.setText("CHECK PROCESS ON THE LOG");
        text.setZ(1);
        text.moveCenter(emo.Stage.getWindowWidth() * 0.5, emo.Stage.getWindowHeight() * 0.5);
        text.load();
        
        local database = emo.Database;
        
        print("*************** OPEN *****************");
        // Open database with encryption flag
        local result = database.openOrCreate(true);
        if( result != EMO_NO_ERROR){
            print("failed to open database");
        }else{
            print("OK");
        }
        
        print("*********** CREATE TABLE *************");
        local tableName = "tableA";
        local columns = ["name", "age", "description"];
        // Set true for the 3rd argument when you use primary key
        result = database.createTable(tableName, columns, true);
        if(result != EMO_NO_ERROR){
            print("failed to create table");
        }else{
            print("OK");
        }
        
        print("************** INSERT ****************");
        local record1 = ["Maria", "25", "a woman"];
        local record2 = ["Ken", "44", "a man"];
        local record3 = ["Mickey", "3", "a mouse"];
        
        result = database.insert(tableName, record1);
        if(result != EMO_NO_ERROR){
            print("failed to insert record");
        }else{
            print("OK");
        }
        result = database.insert(tableName, record2);
        if(result != EMO_NO_ERROR){
            print("failed to insert record");
        }else{
            print("OK");
        }
        result = database.insert(tableName, record3);
        if(result != EMO_NO_ERROR){
            print("failed to insert record");
        }else{
            print("OK");
        }
        
        print("************ SELECT ALL **************");
        local recordArray = database.selectAll(tableName);
        for(local i = 0; i < recordArray.len() ; i++){
            local str = "";
            for(local j = 0; j < recordArray[i].len() ; j++){
                str += recordArray[i][j] + "; ";
            }
            print(str);
        }
        
        print("******* UPDATE WITH CONDITION ********");
        local dataSets = [ ["age", "7"], ["description", "a dog"] ]; // column name & value sets to update
        local condition = [["name", "==", "Mickey"]];
        result = database.updateWhere(tableName, dataSets, condition);
        if(result != EMO_NO_ERROR){
            print("failed to update record");
        }else{
            print("OK");
        }

        print("******* SELECT WITH CONDITION ********");
        // Select records to verify the result
        local condition = [["name", "==", "Mickey"]];
        recordArray = database.selectAllWhere(tableName, condition);
         for(local i = 0; i < recordArray.len() ; i++){
            local str = "";
            for(local j = 0; j < recordArray[i].len() ; j++){
                str += recordArray[i][j] + "; ";
            }
            print(str);
        }
        
        print("******* DELETE WITH CONDITION ********");
        condition = [["description", "==", "a woman"]];
        result = database.deleteWhere(tableName, condition);
        if(result != EMO_NO_ERROR){
            print("failed to delete record");
        }else{
            print("OK");
        }
        
        recordArray = database.selectAll(tableName);
        print("************ SELECT ALL **************");
        for(local i = 0; i < recordArray.len() ; i++){
            local str = "";
            for(local j = 0; j < recordArray[i].len() ; j++){
                str += recordArray[i][j] + "; ";
            }
            print(str);
        }
        
        print("*********** TRUNCATE TABLE ***********");
        result = database.truncateTable(tableName);
        if(result != EMO_NO_ERROR){
            print("failed to truncate table");
        }else{
            print("OK");
        }
        
        print("*************** COUNT ****************");
        // Count remaining records
        local count = database.count(tableName);
        print( count );
        
        print("************ DROP TABLE **************");
        result = database.dropTable(tableName);
        if(result != EMO_NO_ERROR){
            print("failed to drop table");
        }else{
            print("OK");
        }
        
        print("************ ERROR CHECK *************");
        // Printing errors if any
        print( "last error : " + database.getLastError() );
        print( "last error message : " + database.getLastErrorMessage() );
        
        print("*********** CLOSE DATABASE ***********");
        result = database.close();
        if(result != EMO_NO_ERROR){
            print("failed to close database");
        }else{
            print("OK");
        }
        
        print("*********** REMOVE DATABASE **********");
        result = database.remove();
        if(result != EMO_NO_ERROR){
            print("failed to remove database");
        }else{
            print("OK");
        }
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
    }
}

function emo::onLoad() {
    emo.Stage().load(Main());
}
