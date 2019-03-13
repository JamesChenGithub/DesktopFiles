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

@end

@implementation RoomNumViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

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
