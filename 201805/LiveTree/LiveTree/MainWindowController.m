//
//  MainWindowController.m
//  LiveTree
//
//  Created by AlexiChen on 2018/4/24.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#import "MainWindowController.h"

@interface MainWindowController ()
@property (weak) IBOutlet NSToolbarItem *startupItem;
@property (weak) IBOutlet NSToolbarItem *roomConfigItem;
@property (weak) IBOutlet NSToolbarItem *enterRoomItem;
@property (weak) IBOutlet NSToolbarItem *appConfig;

@end

@implementation MainWindowController

- (void)windowDidLoad {
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}
- (IBAction)onStartContext:(id)sender {
}
- (IBAction)onRoomConfig:(id)sender {
}
- (IBAction)onEnterRoom:(id)sender {
}

@end
