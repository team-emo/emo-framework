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
	
	viewController = [[EmoViewController alloc] init];
	viewController.view = [[EmoView alloc] initWithFrame:frame];
	[viewController awakeFromNib];
	rootViewReLoaded = FALSE;
}

- (void)viewWillAppear:(BOOL)animated {
	if (rootViewReLoaded) {
		[viewController onLostFocus];
		[viewController stopAnimation];
		[viewController onDispose];
		rootViewReLoaded = FALSE;
	}
    [super viewWillAppear:animated];
}

#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    switch( section ) {
        case 0: return 3;
        case 1: return 1;
        default: return 0;
    }
}

- (NSString *) tableView:(UITableView *) tableView titleForHeaderInSection:(NSInteger) section {
	
    switch( section ) {
        case 0: return @"Basic";
        case 1: return @"Event";
    }
    return nil;
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    if( indexPath.section == 0 ) {
        if( indexPath.row == 0 ) {
            [[cell textLabel] setText:@"Drawing a Shape"];
        }else if( indexPath.row == 1 ) {
            [[cell textLabel] setText:@"Drawing a Sprite"];
        }else if( indexPath.row == 2 ) {
            [[cell textLabel] setText:@"Drawing a Map"];
        }
    }else if( indexPath.section == 1 ) {
        if( indexPath.row == 0 ) {
            [[cell textLabel] setText:@"Periodic Update"];
        }
    }
	
    return cell;
}


#pragma mark -
#pragma mark Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
	if (indexPath.section == 0) {
		viewController.runtimeScript = @"runtime.nut";
		viewController.mainScript = @"periodic_update_example.nut";
	}
	
	[viewController onLoad];
	[viewController onGainedFocus];
	[viewController startAnimation];
	
	[self.navigationController pushViewController:viewController animated:YES];			
	
	rootViewReLoaded = TRUE;
}


#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload {
	[viewController release];
}


- (void)dealloc {
    [super dealloc];
}


@end

