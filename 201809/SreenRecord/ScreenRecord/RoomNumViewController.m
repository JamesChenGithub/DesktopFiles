//
//  RoomNumViewController.m
//  SreenRecord
//
//  Created by AlexiChen on 2018/5/29.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#import "RoomNumViewController.h"
#import <ILiveSDK/ILiveRoomOption.h>
#import <ILiveSDK/ILiveRoomManager.h>

#import "HostViewController.h"
#import "GuestViewController.h"

@interface RoomNumViewController ()
@property (weak, nonatomic) IBOutlet UITextField *roomNum;
@property (weak, nonatomic) IBOutlet UILabel *tipInfo;
@property (weak, nonatomic) IBOutlet UIButton *hostButton;
@property (weak, nonatomic) IBOutlet UIButton *guestButton;
@property (weak, nonatomic) IBOutlet UIButton *loginButton;
@property (weak, nonatomic) NSString *loginID;

@end

#define kAutoLogin 1

@implementation RoomNumViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
#if kAutoLogin
    [self onLogin];
#endif

}

- (IBAction)onLogin
{
    __weak typeof(self) ws = self;
    
    ws.view.userInteractionEnabled = NO;
    
    if (ws.loginID.length == 0)
    {
        //定义一个包含数字，大小写字母的字符串
        NSString * strAll = @"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        //定义一个结果
        NSString *result = [[NSMutableString alloc] initWithCapacity:24];
        for (int i = 0; i < 24; i++)
        {
            //获取随机数
            NSInteger index = arc4random() % (strAll.length-1);
            char tempStr = [strAll characterAtIndex:index];
            result = (NSMutableString *)[result stringByAppendingString:[NSString stringWithFormat:@"%c",tempStr]];
        }
        ws.loginID = result;
    }
    
    
    
    NSString *url = @"https://xzb.qcloud.com/webrtc/weapp/webrtc_room/get_login_info";
    NSDictionary *dic = @{@"userID" : ws.loginID};
    NSData *data = [NSJSONSerialization dataWithJSONObject:dic options:NSJSONWritingPrettyPrinted error:nil];
    
    NSURL *URL = [NSURL URLWithString:url];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:URL];
    
    if (data)
    {
        [request setValue:[NSString stringWithFormat:@"%ld",(long)[data length]] forHTTPHeaderField:@"Content-Length"];
        [request setHTTPMethod:@"POST"];
        [request setValue:@"application/json; charset=UTF-8" forHTTPHeaderField:@"Content-Type"];
        [request setValue:@"gzip" forHTTPHeaderField:@"Accept-Encoding"];
        
        [request setHTTPBody:data];
    }
    
    [request setTimeoutInterval:10];
    
#if TARGET_OS_IOS
    [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
#endif
    
    NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        
#if TARGET_OS_IOS
        [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
#endif
        if (error != nil)
        {
            
            dispatch_async(dispatch_get_main_queue(), ^{
                ws.view.userInteractionEnabled = YES;
                ws.tipInfo.text = @"获取sig失败";
                ws.loginButton.hidden = NO;
            });
        }
        else
        {
            NSObject *jsonObj = [NSJSONSerialization JSONObjectWithData:data options:0 error:NULL];//[responseString objectFromJSONString];
            if (jsonObj && [jsonObj isKindOfClass:[NSDictionary class]])
            {
                NSDictionary *jdic = (NSDictionary *)jsonObj;
                int code = [(NSNumber *)jdic[@"code"] intValue];
                if (code == 0) {
                    NSString *userid = jdic[@"userID"];
                    NSString *usersig = jdic[@"userSig"];
                    
                    dispatch_async(dispatch_get_main_queue(), ^{
                        ws.view.userInteractionEnabled = YES;
                        [ws loginWith:userid userSig:usersig];
                    });
                }
                else
                {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        ws.view.userInteractionEnabled = YES;
                        ws.tipInfo.text = @"获取sig失败";
                        ws.loginButton.hidden = NO;
                    });
                }
                
            }
            else
            {
                dispatch_async(dispatch_get_main_queue(), ^{
                    ws.view.userInteractionEnabled = YES;
                    ws.tipInfo.text = @"获取sig失败";
                    ws.loginButton.hidden = NO;
                });
            }
        }
    }];
    [task resume];
}

- (void)loginWith:(NSString *)userid userSig:(NSString *)sig
{
    __weak typeof(self) ws = self;
    if (userid.length == 0 || sig.length == 0)
    {
        ws.tipInfo.text = @"获取sig失败";
        ws.loginButton.hidden = NO;
        return;
    }
    ws.view.userInteractionEnabled = NO;
    [[ILiveLoginManager getInstance] iLiveLogin:userid sig:sig succ:^{
        ws.view.userInteractionEnabled = YES;
        ws.tipInfo.text = @"登录成功";
        ws.loginButton.hidden = YES;
    } failed:^(NSString *module, int errId, NSString *errMsg) {
        ws.view.userInteractionEnabled = YES;
        ws.tipInfo.text = [NSString stringWithFormat:@"登录失败：%@， %d，%@", module, errId, errMsg];
        ws.loginButton.hidden = NO;
    }];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    if ([app isScreenRecording])
    {
        self.roomNum.enabled = NO;
        self.guestButton.enabled = NO;
        
        // Do any additional setup after loading the view.
        self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"停止录制" style:UIBarButtonItemStylePlain target:self action:@selector(onStopRecord)];
    }
}

- (void)onStopRecord
{
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    
    __weak typeof(self) ws = self;
    [[[ILiveSDK getInstance] getAVContext].videoCtrl enableScreenRecord:NO withMode:QAVRecordPreset1280x720 complete:^(int result) {
        [app stopScreenRecord];
        ws.roomNum.enabled = YES;
        ws.guestButton.enabled = YES;
        
        
        [[ILiveRoomManager getInstance] quitRoom:^{
            // Do any additional setup after loading the view.
            ws.navigationItem.rightBarButtonItem = nil;
        } failed:^(NSString *module, int errId, NSString *errMsg) {
             ws.navigationItem.rightBarButtonItem = nil;
        }];
    }];
    
}
- (IBAction)onTapBlank:(id)sender {
    [[UIApplication sharedApplication] sendAction:@selector(resignFirstResponder) to:nil from:nil forEvent:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
- (IBAction)onHostCreateRoom:(id)sender {
    [self onTapBlank:nil];
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    if ([app isScreenRecording])
    {
        [self performSegueWithIdentifier:@"HostCreateRoom" sender:nil];
    }
    else
    {
        int roomnum = self.roomNum.text.intValue;
        if (roomnum == 0)
        {
            self.tipInfo.text = @"房间号有误";
            return;
        }
        
        [self performSegueWithIdentifier:@"HostCreateRoom" sender:nil];
    }
}
- (IBAction)onGuestJoinRoom:(id)sender {
    [self onTapBlank:nil];
    
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    if ([app isScreenRecording])
    {
        [self performSegueWithIdentifier:@"GuestJoinRoom" sender:nil];
    }
    else
    {
        int roomnum = self.roomNum.text.intValue;
        if (roomnum == 0)
        {
            self.tipInfo.text = @"房间号有误";
            return;
        }
        
        [self performSegueWithIdentifier:@"GuestJoinRoom" sender:nil];
    }

}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    __weak typeof(self) ws = self;
    if ([segue.identifier isEqualToString:@"HostCreateRoom"])
    {
        AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
        if (![app isScreenRecording])
        {
            HostViewController *vc = (HostViewController *)segue.destinationViewController;
            vc.roomNum = self.roomNum.text.intValue;
        }
    }
    else if ([segue.identifier isEqualToString:@"GuestJoinRoom"])
    {
        
        AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
        if (![app isScreenRecording])
        {
            GuestViewController *vc = (GuestViewController *)segue.destinationViewController;
            vc.roomNum = self.roomNum.text.intValue;
        }
        
    }
}


/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
