package com.emo_framework.examples;

import java.util.Map;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;

import android.os.Bundle;
import android.content.ComponentName;
import android.content.Intent;
import android.view.View;
import android.widget.ExpandableListView;
import android.widget.ExpandableListAdapter;
import android.widget.SimpleExpandableListAdapter;
import android.app.ExpandableListActivity;

public class Launcher extends ExpandableListActivity {
	
    private static final String KEY = "KEY";
	private static final String PACKAGE_NAME = "com.emo_framework.examples";
    
	private ExpandableListAdapter adapter;
    private List<Map<String, String>> groupData = new ArrayList<Map<String, String>>();
    private List<List<Map<String, String>>> childData = new ArrayList<List<Map<String, String>>>();
    
    private static final String[] groupNames = {
    	"Basic", "Animation"
    };
    
    private static final String[][] childNames = {
    	{
    		"Drawing a Shape",
    		"Drawing a Sprite",
			"Drawing Tiles"
    	},
    	{
    		"Periodic Update",
    		"Using Modifiers",
    		"Modifier with Easing"
    	}
    };
    private static final String[][] activities = {
    	{
    		".ShapeExample",
    		".SpriteExample",
			".TileExample"
    	},
    	{
    		".PeriodicUpdateExample",
    		".RotateModifierExample",
    		".MoveModifierExample"
    	}
    };
    
	@Override
	public void onCreate(final Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		for (int i = 0; i < groupNames.length; i++) {
			String groupName = groupNames[i];
            Map<String, String> curGroupMap = new HashMap<String, String>();
            groupData.add(curGroupMap);
            curGroupMap.put(KEY, groupName);
            
        	List<Map<String, String>> children = new ArrayList<Map<String, String>>();
        	
        	String[] childName = childNames[i];
        	for (String name : childName) {
                Map<String, String> curChildMap = new HashMap<String, String>();
        		curChildMap.put(KEY, name);
        		children.add(curChildMap);
        	}
        	childData.add(children);
        }
				
		adapter = new SimpleExpandableListAdapter(
				this, 
                groupData,
                android.R.layout.simple_expandable_list_item_1,
                new String[] { KEY },
                new int[] { android.R.id.text1 },
                childData,
                android.R.layout.simple_expandable_list_item_2,
                new String[] { KEY },
                new int[] { android.R.id.text1 }
		);
		this.setListAdapter(adapter);
		this.setContentView(R.layout.main);
	}
	
	@Override
	public boolean onChildClick(ExpandableListView parent, View view, 
			int groupPosition, int childPosition, final long id) {
		Intent intent = new Intent(Intent.ACTION_MAIN);
		intent.setComponent(new ComponentName(PACKAGE_NAME, 
					     PACKAGE_NAME + activities[groupPosition][childPosition]));
        startActivity(intent);
		return super.onChildClick(parent, view, groupPosition, childPosition, id);
	}
}
