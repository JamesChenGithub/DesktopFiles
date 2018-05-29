//
//  MainSplitViewController.m
//  LiveTree
//
//  Created by AlexiChen on 2018/4/23.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#import "MainSplitViewController.h"

@interface MainSplitViewController ()
@property (weak) IBOutlet NSSplitViewItem *treeItem;
@property (weak) IBOutlet NSSplitViewItem *contentItem;
@property (weak) IBOutlet NSSplitViewItem *infoItem;

@end

@implementation MainSplitViewController

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)viewDidAppear
{
    [super viewDidAppear];
    
    CGRect frame = self.view.window.frame;
    frame.origin.x += (frame.size.width - 1600)/2;
    frame.origin.y += (frame.size.height - 1200)/2;
    frame.size.width = 1600;
    frame.size.height = 1200;
    [self.view.window setFrame:frame display:YES];
    [self.view.window update];
    
    
    NSViewController *vc0 = self.childViewControllers[0];
    NSViewController *vc1 = self.childViewControllers[1];
    NSViewController *vc2 = self.childViewControllers[2];
    
    
    CGRect rect0 = vc0.view.frame;
    CGRect rect00 = rect0;
    rect00.size.width = 300;
    [vc0.view setFrame:rect00];
    
    
    
    CGRect rect2 = vc2.view.frame;
    CGRect rect22 = rect2;
    rect22.size.width = 300;
    [vc2.view setFrame:rect22];
    
    
    CGRect rect1 = vc1.view.frame;
    rect1.size.width = rect0.size.width + rect1.size.width + rect2.size.width - rect00.size.width - rect22.size.width;
    [vc2.view setFrame:rect1];
    
    [self updateViewConstraints];
}

//- (BOOL)splitView:(NSSplitView *)splitView canCollapseSubview:(NSView *)subview
//{
//    [super splitView:splitView canCollapseSubview:subview];
//    return NO;
//}
//
//
///* Return YES if the subview should be collapsed because the user has double-clicked on an adjacent divider. If a split view has a delegate, and the delegate responds to this message, it will be sent once for the subview before a divider when the user double-clicks on that divider, and again for the subview after the divider, but only if the delegate returned YES when sent -splitView:canCollapseSubview: for the subview in question. When the delegate indicates that both subviews should be collapsed NSSplitView's behavior is undefined.
// */
//- (BOOL)splitView:(NSSplitView *)splitView shouldCollapseSubview:(NSView *)subview forDoubleClickOnDividerAtIndex:(NSInteger)dividerIndex NS_AVAILABLE_MAC(10_5)
//{
//    [super splitView:splitView shouldCollapseSubview:subview forDoubleClickOnDividerAtIndex:dividerIndex];
//    return NO;
//}


///* Given a proposed minimum allowable position for one of the dividers of a split view, return the minimum allowable position for the divider. If a split view has no delegate, or if its delegate does not respond to this message, the split view behaves as if it has a delegate that responds to this message by merely returning the proposed minimum. If a split view has a delegate, and the delegate responds to this message, it will be sent at least once when the user begins dragging one of the split view's dividers, and may be resent as the user continues to drag the divider.
// 
// Delegates that respond to this message and return a number larger than the proposed minimum position effectively declare a minimum size for the subview above or to the left of the divider in question, the minimum size being the difference between the proposed and returned minimum positions. This minimum size is only effective for the divider-dragging operation during which the -splitView:constrainMinCoordinate:ofSubviewAt: message is sent. NSSplitView's behavior is undefined when a delegate responds to this message by returning a number smaller than the proposed minimum.
// */
//- (CGFloat)splitView:(NSSplitView *)splitView constrainMinCoordinate:(CGFloat)proposedMinimumPosition ofSubviewAt:(NSInteger)dividerIndex;
//
///* Given a proposed maximum allowable position for one of the dividers of a split view, return the maximum allowable position for the divider. If a split view has no delegate, or if its delegate does not respond to this message, the split view behaves as if it has a delegate that responds to this message by merely returning the proposed maximum. If a split view has a delegate, and the delegate responds to this message, it will be sent at least once when the user begins dragging one of the split view's dividers, and may be resent as the user continues to drag the divider.
// 
// Delegates that respond to this message and return a number smaller than the proposed maximum position effectively declare a minimum size for the subview below or to the right of the divider in question, the minimum size being the difference between the proposed and returned maximum positions. This minimum size is only effective for the divider-dragging operation during which the -splitView:constrainMaxCoordinate:ofSubviewAt: message is sent. NSSplitView's behavior is undefined when a delegate responds to this message by returning a number larger than the proposed maximum.
// */
//- (CGFloat)splitView:(NSSplitView *)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex;
//
///* Given a proposed position for one of the dividers of a split view, return a position at which the divider should be placed as the user drags it. If a split view has no delegate, or if its delegate does not respond to this message, the split view behaves as if it has a delegate that responds to this message by merely returning the proposed position. If a split view has a delegate, and the delegate responds to this message, it will be sent repeatedly as the user drags one of the split view's dividers.
// */
//- (CGFloat)splitView:(NSSplitView *)splitView constrainSplitPosition:(CGFloat)proposedPosition ofSubviewAt:(NSInteger)dividerIndex;
//
///* Given that a split view has been resized but has not yet adjusted its subviews to accomodate the new size, and given the former size of the split view, adjust the subviews to accomodate the new size of the split view. If a split view has no delegate, or if its delegate does not respond to this message, the split view behaves as if it has a delegate that responds to this message by merely sending the split view an -adjustSubviews message.
// 
// Delegates that respond to this message should adjust the frames of the uncollapsed subviews so that they exactly fill the split view with room for dividers in between, taking its new size into consideration. The thickness of dividers can be determined by sending the split view a -dividerThickness message.
// */
//- (void)splitView:(NSSplitView *)splitView resizeSubviewsWithOldSize:(NSSize)oldSize;
//
//
///* Given that a split view has been resized and is adjusting its subviews to accomodate the new size, return YES if -adjustSubviews can change the size of the indexed subview, NO otherwise. -adjustSubviews may change the origin of the indexed subview regardless of what this method returns. -adjustSubviews may also resize otherwise nonresizable subviews to prevent an invalid subview layout. If a split view has no delegate, or if its delegate does not respond to this message, the split view behaves as if it has a delegate that returns YES when sent this message.
// */
//- (BOOL)splitView:(NSSplitView *)splitView shouldAdjustSizeOfSubview:(NSView *)view NS_AVAILABLE_MAC(10_6);
//
//
//
///* Given that a split view has been resized and is adjusting its subviews to accomodate the new size, or that the user is dragging a divider, return YES to allow the divider to be dragged or adjusted off the edge of the split view where it will not be visible. If a split view has no delegate, or if its delegate does not respond to this message, the split view behaves as if it has a delegate that returns NO when sent this message.
// */
//- (BOOL)splitView:(NSSplitView *)splitView shouldHideDividerAtIndex:(NSInteger)dividerIndex NS_AVAILABLE_MAC(10_5);
//
///* Given the drawn frame of a divider (in the coordinate system established by the split view's bounds), return the frame in which mouse clicks should initiate divider dragging. If a split view has no delegate, or if its delegate does not respond to this message, the split view behaves as if it has a delegate that returns proposedEffectiveRect when sent this message. A split view with thick dividers proposes the drawn frame as the effective frame. A split view with thin dividers proposes an effective frame that's a litte larger than the drawn frame, to make it easier for the user to actually grab the divider.
// */
//- (NSRect)splitView:(NSSplitView *)splitView effectiveRect:(NSRect)proposedEffectiveRect forDrawnRect:(NSRect)drawnRect ofDividerAtIndex:(NSInteger)dividerIndex NS_AVAILABLE_MAC(10_5);
//
///* Given a divider index, return an additional rectangular area (in the coordinate system established by the split view's bounds) in which mouse clicks should also initiate divider dragging, or NSZeroRect to not add one. If a split view has no delegate, or if its delegate does not respond to this message, only mouse clicks within the effective frame of a divider initiate divider dragging.
// */
//- (NSRect)splitView:(NSSplitView *)splitView additionalEffectiveRectOfDividerAtIndex:(NSInteger)dividerIndex NS_AVAILABLE_MAC(10_5);
//
//
///* Respond as if the delegate had registered for the NSSplitViewDidResizeSubviewsNotification or NSSplitViewWillResizeSubviewsNotification notification, described below. A split view's behavior is not explicitly affected by a delegate's ability or inability to respond to these messages, though the delegate may send messages to the split view in response to these messages.
// */
//- (void)splitViewWillResizeSubviews:(NSNotification *)notification;
//- (void)splitViewDidResizeSubviews:(NSNotification *)notification;

@end
