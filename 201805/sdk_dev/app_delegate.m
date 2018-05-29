#import "app_delegate.h"
#import "main_menu.h"
#import <Rqd/CrashReporter.h>
#include "xcast.h"
@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;

@property (nonatomic, strong) IBOutlet LoginViewController *loginViewController;

@end

@implementation AppDelegate


-(void) applicationDidFinishLaunching:(NSNotification *)notification {
  
    NSApplication *app = [NSApplication sharedApplication];

  
  _loginViewController = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:nil];
  [_loginViewController initUI];
  
  [[CrashReporter sharedInstance] setSceneValue:[NSString stringWithUTF8String: xcast_version()] forKey:@"XCastVersion"];
  [[CrashReporter sharedInstance] installWithAppkey:@"com.tencent.av.xcast"];
 
  [[CrashReporter sharedInstance] enableConsoleLogReport:YES];
   
  
  
  [_window.contentView addSubview:_loginViewController
   .view];
}

@end
