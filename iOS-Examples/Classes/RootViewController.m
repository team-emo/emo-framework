#import "RootViewController.h"
#import "EmoEngine_glue.h"

@implementation RootViewController


#pragma mark -
#pragma mark View lifecycle

- (void)viewDidLoad {
    [super viewDidLoad];

	self.title = @"emo examples";

	// subtract navigationBar height from the view
	CGRect frame = [self.tableView bounds];
	frame.size.height = frame.size.height - [self.navigationController.navigationBar frame].size.height;
	
	// initialize emo framework
	viewController = [[EmoViewController alloc] init];
	viewController.view = [[EmoView alloc] initWithFrame:frame];
	[viewController awakeFromNib];
	didReturnRootView = FALSE;
	
	sections = [[NSArray alloc] initWithObjects:@"Basic", @"Animation", 
					@"Event", @"On-Screen Controller", @"Audio",
					@"Database", @"Physics", @"Physics with Joints",
					@"Scene Transition", @"Misc",
					nil];
	examples = [[NSMutableArray alloc] init];
	
	// Basic
	[examples addObject: [NSArray arrayWithObjects:
		[NSArray arrayWithObjects:@"Drawing a Shape",  @"shape_example.nut", nil],
		[NSArray arrayWithObjects:@"Drawing a Sprite", @"sprite_example.nut", nil],
		[NSArray arrayWithObjects:@"Drawing Texts",    @"textsprite_example.nut", nil],
		[NSArray arrayWithObjects:@"Drawing Tiles",    @"tiles_example.nut", nil],
		[NSArray arrayWithObjects:@"Drawing TiledMap", @"mapsprite_example.nut", nil],
        [NSArray arrayWithObjects:@"Using a Texture Atlas",   @"atlas_example.nut", nil],
        [NSArray arrayWithObjects:@"Using Unicode Characters",   @"fontsprite_example.nut", nil],
		nil]];
	
	// Amimation
	[examples addObject: [NSArray arrayWithObjects:
		[NSArray arrayWithObjects:@"Sprite Animation", @"animation_example.nut", nil],
		[NSArray arrayWithObjects:@"Periodic Update",  @"periodic_update_example.nut", nil],
		[NSArray arrayWithObjects:@"Modifiers with Easing", @"modifier_move_example.nut", nil],
		[NSArray arrayWithObjects:@"Multiple Modifiers", @"modifier_multiple.nut", nil],
        [NSArray arrayWithObjects:@"Using Point Sprite",  @"pointsprite_example.nut", nil],
		nil]];

	// Event
	[examples addObject: [NSArray arrayWithObjects:
						  [NSArray arrayWithObjects:@"Dragging a Sprite",  @"motion_event.nut", nil],
						  [NSArray arrayWithObjects:@"Handling Multi-Touch",  @"motion_event_multi.nut", nil],
						  [NSArray arrayWithObjects:@"Using the Accelerometer",  @"sensor_event.nut", nil],
						  nil]];

	// On-Screen Controller
	[examples addObject: [NSArray arrayWithObjects:
						  [NSArray arrayWithObjects:@"Analog Controller",     @"analog_controller_example.nut", nil],
						  [NSArray arrayWithObjects:@"Digital Controller",    @"digital_controller_example.nut", nil],
						  [NSArray arrayWithObjects:@"Multiple Controllers",  @"multiple_controllers_example.nut", nil],
						  nil]];
	// Audio
	[examples addObject: [NSArray arrayWithObjects:
						 [NSArray arrayWithObjects:@"Playing with Sound", @"audio_example.nut", nil],
						 [NSArray arrayWithObjects:@"Multiple Channels",  @"audio_channel_example.nut", nil],
						 [NSArray arrayWithObjects:@"Changing Volume",    @"audio_volume_example.nut", nil],
						 nil]];
	
	// Database
	[examples addObject: [NSArray arrayWithObjects:
						 [NSArray arrayWithObjects:@"Save and Restore",   @"database_example.nut", nil],
						 nil]];
	
	// Physics
	[examples addObject: [NSArray arrayWithObjects:
						  [NSArray arrayWithObjects:@"HelloWorld (No Graphics)", @"box2d_helloworld.nut", nil],
						  [NSArray arrayWithObjects:@"Using Box Shape",    @"physics_helloworld.nut", nil],
						  [NSArray arrayWithObjects:@"Using Circle Shape", @"physics_circle.nut", nil],
						  [NSArray arrayWithObjects:@"Using Soft Circle",  @"physics_softcircle.nut", nil],
						  [NSArray arrayWithObjects:@"Using Soft Polygon", @"physics_soft_polygon.nut", nil],
						  [NSArray arrayWithObjects:@"Using with Sensor",  @"physics_sensor.nut", nil],
						  [NSArray arrayWithObjects:@"Using ContactListener",  @"physics_contact.nut", nil],
						  [NSArray arrayWithObjects:@"Adding Physics",     @"physics_add_example.nut", nil],
						  [NSArray arrayWithObjects:@"Using Camera with Physics",     @"physics_camera_moving.nut", nil],
						  nil]];
	
	// Physics with Joints
	[examples addObject: [NSArray arrayWithObjects:
						  [NSArray arrayWithObjects:@"Using DistanceJoint", @"physics_distancejoint.nut", nil],
						  [NSArray arrayWithObjects:@"Using RevoluteJoint", @"physics_revolutejoint.nut", nil],
						  [NSArray arrayWithObjects:@"Using PrismaticJoint", @"physics_prismaticjoint.nut", nil],
						  [NSArray arrayWithObjects:@"Using PulleyJoint",   @"physics_pulleyjoint.nut", nil],
						  [NSArray arrayWithObjects:@"Using GearJoint",     @"physics_gearjoint.nut", nil],
						  [NSArray arrayWithObjects:@"Using LineJoint",     @"physics_linejoint.nut", nil],
						  [NSArray arrayWithObjects:@"Using WeldJoint",     @"physics_weldjoint.nut", nil],
						  [NSArray arrayWithObjects:@"Using MouseJoint",    @"physics_mousejoint.nut", nil],
						  nil]];
	// Using Transitions
	[examples addObject: [NSArray arrayWithObjects:
                          [NSArray arrayWithObjects:@"Fade In/Out",        @"transition_fade_example.nut",  nil],
                          [NSArray arrayWithObjects:@"Move In/Out",        @"transition_move_example.nut",  nil],
                          [NSArray arrayWithObjects:@"Rotate and Scale",   @"transition_scale_example.nut", nil],
                          nil]];
	// Misc
	[examples addObject: [NSArray arrayWithObjects:
						 [NSArray arrayWithObjects:@"Loading Screen",  @"modifier_rotate_example.nut", nil],
						 [NSArray arrayWithObjects:@"Splash Screen",   @"modifier_event.nut", nil],
						 [NSArray arrayWithObjects:@"Accessing via HTTP", @"http_access_example.nut", nil],
                         [NSArray arrayWithObjects:@"Compiling a Script", @"compile_script_example.nut", nil],
                         [NSArray arrayWithObjects:@"Using Blendfunc", @"blendfunc_example.nut", nil],
						 nil]];
}

- (void)viewWillAppear:(BOOL)animated {
	// dispose emo framework before returning to the root menu
	if (didReturnRootView) {
		[viewController onLostFocus];
		[viewController stopAnimation];
		[viewController onDispose];
		didReturnRootView = FALSE;
	}
    [super viewWillAppear:animated];
}

#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return [examples count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	if (section >= [examples count]) return 0;
	return [[examples objectAtIndex:section] count];
}

- (NSString *) tableView:(UITableView *) tableView titleForHeaderInSection:(NSInteger) section {

	if (section >= [sections count]) return nil;
	return [sections objectAtIndex:section];
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
	if (indexPath.section >= [examples count]) return cell;
	if (indexPath.row >= [[examples objectAtIndex:indexPath.section] count]) return cell;
	[[cell textLabel] setText:[[[examples objectAtIndex:indexPath.section]
								objectAtIndex:indexPath.row] objectAtIndex:0]];
	
    return cell;
}


#pragma mark -
#pragma mark Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	if (indexPath.section >= [examples count]) return;
	if (indexPath.row >= [[examples objectAtIndex:indexPath.section] count]) return;
	
	// start emo framework with parameters
	viewController.runtimeScript = @"runtime.nut";
	viewController.mainScript = [[[examples objectAtIndex:indexPath.section] 
								  objectAtIndex:indexPath.row] objectAtIndex:1];
	
	[viewController onLoad];
	[viewController onGainedFocus];
	[viewController startAnimation];
	
	[self.navigationController pushViewController:viewController animated:YES];			
	
	didReturnRootView = TRUE;
}


#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end

