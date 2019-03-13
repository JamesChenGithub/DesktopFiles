//
//  HostViewController.m
//  SreenRecord
//
//  Created by AlexiChen on 2018/5/29.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#import "HostViewController.h"

#import <ILiveSDK/ILiveRoomManager.h>
#import <QAVScreenRecordKit/QAVScreenRecord.h>

@interface HostViewController ()
@property (weak, nonatomic) IBOutlet UIView *testView;

@end

@implementation HostViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    if (![app isScreenRecording])
    {
        ILiveRoomOption *opt = [ILiveRoomOption defaultHostLiveOption];
        opt.avOption.autoCamera = NO;
        opt.controlRole = @"LiveMaster";
        __weak typeof(self) ws = self;
        [[ILiveRoomManager getInstance] createRoom:self.roomNum option:opt succ:^{
            
        } failed:^(NSString *module, int errId, NSString *errMsg) {
            NSString *msg = [NSString stringWithFormat:@"主播进房失败，%@, %d, %@", module, errId, errMsg];
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
            [alert show];
        }];
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    if ([app isScreenRecording])
    {
        self.navigationItem.leftBarButtonItem = nil;
        // Do any additional setup after loading the view.
        self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"停止录制" style:UIBarButtonItemStylePlain target:self action:@selector(onStopRecord)];
    }
    else
    {
        self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"退房" style:UIBarButtonItemStylePlain target:self action:@selector(onExitRoom)];
        // Do any additional setup after loading the view.
        self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"开始录制" style:UIBarButtonItemStylePlain target:self action:@selector(onStartRecord)];
    }
    
}

- (void)onExitRoom
{
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    __weak typeof(self) ws = self;
    if (app.isScreenRecording)
    {
        [[[ILiveSDK getInstance] getAVContext].videoCtrl enableScreenRecord:NO withMode:QAVRecordPreset1280x720 complete:^(int result) {
            
            if ([app isScreenRecording])
            {
                [app stopScreenRecord];
            }
            
            
            [[ILiveRoomManager getInstance] quitRoom:^{
                [ws.navigationController popViewControllerAnimated:YES];
            } failed:^(NSString *module, int errId, NSString *errMsg) {
                [ws.navigationController popViewControllerAnimated:YES];
            }];
        }];
    }
    else
    {
        [[ILiveRoomManager getInstance] quitRoom:^{
            [ws.navigationController popViewControllerAnimated:YES];
        } failed:^(NSString *module, int errId, NSString *errMsg) {
            [ws.navigationController popViewControllerAnimated:YES];
        }];
    }
    
}

- (void)onStartRecord
{
    QAVRecordPreset rec = QAVRecordPreset1280x720;
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    if (![app isScreenRecording])
    {
        __weak typeof(self) ws = self;
        [[[ILiveSDK getInstance] getAVContext].videoCtrl enableScreenRecord:YES withMode:rec complete:^(int result) {
            [app startScreenRecord:rec];
            ws.navigationItem.leftBarButtonItem = nil;
            ws.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"停止录制" style:UIBarButtonItemStylePlain target:self action:@selector(onStopRecord)];
        }];
    }
}
- (IBAction)onPanTestView:(UIPanGestureRecognizer *)recognizer {
    if ([recognizer state] == UIGestureRecognizerStateChanged) {
        CGPoint translatedPoint = [recognizer locationInView:self.view];
        self.testView.center = translatedPoint;
    }
}

- (void)onStopRecord
{
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    [app stopScreenRecord];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"退房" style:UIBarButtonItemStylePlain target:self action:@selector(onExitRoom)];
    // Do any additional setup after loading the view.
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"开始录制" style:UIBarButtonItemStylePlain target:self action:@selector(onStartRecord)];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
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
