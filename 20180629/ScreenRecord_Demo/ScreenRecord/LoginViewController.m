//
//  LoginViewController.m
//  SreenRecord
//
//  Created by AlexiChen on 2018/5/29.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#import "LoginViewController.h"
#import "RoomNumViewController.h"
#import <ILiveSDK/ILiveLoginManager.h>

@interface LoginViewController ()
@property (weak, nonatomic) IBOutlet UITextField *identifier;
@property (weak, nonatomic) IBOutlet UITextField *usersig;
@property (weak, nonatomic) IBOutlet UILabel *tipinfo;

@end

@implementation LoginViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    if (![app isSupportScreenRecord]) {
        self.tipinfo.text = @"屏幕录制功能只支持iOS9，iOS10系统。";
    }
    
}
- (IBAction)onEditIdentifierEnd:(UITextField *)sender {
    
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)onTapBlank:(UITapGestureRecognizer *)sender {
    [[UIApplication sharedApplication] sendAction:@selector(resignFirstResponder) to:nil from:nil forEvent:nil];
}


- (IBAction)onLogin:(id)sender {
    
    if (self.identifier.text.length == 0 || self.usersig.text.length == 0)
    {
        self.tipinfo.text = @"请输入identifier或usersig";
        return;
    }
    
    
    
    __weak typeof(self) ws = self;
    [[ILiveLoginManager getInstance] iLiveLogin:self.identifier.text sig:self.usersig.text succ:^{
        NSMutableArray *controllers = [NSMutableArray arrayWithArray:ws.navigationController.viewControllers];
        
        UIStoryboard *msb = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
        controllers[0] = [msb instantiateViewControllerWithIdentifier:@"RoomNumViewController"];
        [ws.navigationController setViewControllers:controllers];
    } failed:^(NSString *module, int errId, NSString *errMsg) {
        ws.tipinfo.text = [NSString stringWithFormat:@"登录失败：%@， %d，%@", module, errId, errMsg];
    }];
}


@end
