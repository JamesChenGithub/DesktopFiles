//
//  GuestViewController.m
//  SreenRecord
//
//  Created by AlexiChen on 2018/5/29.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#import "GuestViewController.h"

#import <ILiveSDK/ILiveRenderView.h>
#import <ILiveSDK/ILiveRoomManager.h>

@interface GuestViewController ()<ILiveMemStatusListener>
@property (weak, nonatomic) IBOutlet ILiveRenderView *renderView;

@end

@implementation GuestViewController

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"退房" style:UIBarButtonItemStylePlain target:self action:@selector(onExitRoom)];
}

- (void)onExitRoom
{
    __weak typeof(self) ws = self;
    
    [[ILiveRoomManager getInstance] quitRoom:^{
        [ws.navigationController popViewControllerAnimated:YES];
    } failed:^(NSString *module, int errId, NSString *errMsg) {
        [ws.navigationController popViewControllerAnimated:YES];
    }];

    
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    ILiveRoomOption *opt = [ILiveRoomOption defaultGuestLiveOption];
    opt.avOption.autoCamera = NO;
    opt.memberStatusListener = self;
    opt.controlRole = @"Guest";
    [[ILiveRoomManager getInstance] joinRoom:self.roomNum option:opt succ:^{
        ILiveFrameDispatcher *fd = [[ILiveRoomManager getInstance] getFrameDispatcher];
        [fd startDisplay];
    } failed:^(NSString *module, int errId, NSString *errMsg) {
        NSString *msg = [NSString stringWithFormat:@"观众进房失败，%@, %d, %@", module, errId, errMsg];
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alert show];
    }];
    

    //[[ILiveRoomManager getInstance] setScreenVideoDelegate:self];
    // Do any additional setup after loading the view.
    
}


/**
 房间成员状态变化通知的函数，房间成员发生状态变化(如是否发音频、是否发视频等)时，会通过该函数通知业务侧。
 
 @param event     状态变化id，详见QAVUpdateEvent的定义
 @param endpoints 发生状态变化的成员id列表。
 
 @return YES 执行成功
 */
- (BOOL)onEndpointsUpdateInfo:(QAVUpdateEvent)event updateList:(NSArray *)endpoints
{
    // 本示例子处理屏幕分享事件，公未展现功能，未作复杂处理
    if (event == QAV_EVENT_ID_ENDPOINT_HAS_SCREEN_VIDEO)
    {
        ILiveFrameDispatcher *fd = [[ILiveRoomManager getInstance] getFrameDispatcher];
        QAVEndpoint *end = endpoints[0];
        self.renderView.identifier = end.identifier;
        self.renderView.srcType = QAVVIDEO_SRC_TYPE_SCREEN;
        [fd addRenderView:self.renderView];
        [self.navigationController setNavigationBarHidden:YES animated:YES];
    }
    else if(event == QAV_EVENT_ID_ENDPOINT_NO_SCREEN_VIDEO)
    {
        ILiveFrameDispatcher *fd = [[ILiveRoomManager getInstance] getFrameDispatcher];
        QAVEndpoint *end = endpoints[0];
        self.renderView.identifier = nil;
        self.renderView.srcType = 0;
        [fd removeRenderViewFor:end.identifier srcType:QAVVIDEO_SRC_TYPE_SCREEN];
        
        [self.navigationController setNavigationBarHidden:NO animated:YES];
    }
    return true;
}
- (IBAction)onTapHideNavBar:(UITapGestureRecognizer *)sender {
    if (sender.state == UIGestureRecognizerStateEnded)
    {
        [self.navigationController setNavigationBarHidden:!self.navigationController.navigationBar.hidden animated:YES];
    }
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
