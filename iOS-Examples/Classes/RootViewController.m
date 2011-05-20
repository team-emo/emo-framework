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
	
	sections = [[NSArray alloc] initWithObjects:@"Basic", @"Animation", @"Physics", nil];
	examples = [[NSMutableArray alloc] init];
	
	// Basic
	[examples addObject: [NSArray arrayWithObjects:
		[NSArray arrayWithObjects:@"Drawing a Shape",  @"shape_example.nut", nil],
		[NSArray arrayWithObjects:@"Drawing a Sprite", @"sprite_example.nut", nil],
		[NSArray arrayWithObjects:@"Drawing Tiles",    @"tiles_example.nut", nil],
		nil]];
	
	// Event
	[examples addObject: [NSArray arrayWithObjects:
		[NSArray arrayWithObjects:@"Periodic Update",   @"periodic_update_example.nut", nil],
		[NSArray arrayWithObjects:@"Using Modifier",   @"modifier_rotate_example.nut", nil],
		[NSArray arrayWithObjects:@"Modifiers with Easing", @"modifier_move_example.nut", nil],
		nil]];

	// Physics
	[examples addObject: [NSArray arrayWithObjects:
						  [NSArray arrayWithObjects:@"Hello, world (No Graphics)", @"box2d_helloworld.nut", nil],
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
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
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

- (void)viewDidUnload {
	[examples release];
	[viewController release];
}


- (void)dealloc {
    [super dealloc];
}


@end

