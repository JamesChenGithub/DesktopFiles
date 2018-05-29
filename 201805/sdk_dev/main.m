//
//  main.m
//
//  Created by 刘磊 on 2017/10/27.
//

#import <Cocoa/Cocoa.h>
#import "app_delegate.h"

int main(int argc, const char *argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        id delegate = [[AppDelegate alloc] init];
        app.delegate = delegate;

        return NSApplicationMain(argc, (const char**)argv);
    }
}

