#import "IosAppDelegate.h"

@implementation IosAppDelegate

@synthesize window=_window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	// Override point for customization after application launch.
	
	CGRect screenBounds = [[UIScreen mainScreen] bounds];
	self.window = [[UIWindow alloc] initWithFrame: screenBounds];

	m_mainViewController = [[IphoneViewController alloc] init];
	self.window.rootViewController = m_mainViewController;
	
	[self.window makeKeyAndVisible];

	return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	[m_mainViewController stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	[m_mainViewController startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	
}

- (void)dealloc
{
	[_window release];
	[super dealloc];
}

@end

int main(int argc, char *argv[])
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	int retVal = UIApplicationMain(argc, argv, nil, @"IosAppDelegate");
	[pool release];
	return retVal;
}
