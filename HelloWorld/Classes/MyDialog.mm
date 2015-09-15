#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "MyDialog.h"

@interface InternalDialog : NSObject
{
    DialogProtocol *callBackObject;
    UIAlertView *alertView;
}
-(void) initWithItems:(NSString*)title buttons:(NSMutableArray*)items deletate:(DialogProtocol*)delegate;
-(void) show;
-(void) alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;
@end

@implementation InternalDialog

-(void) alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    callBackObject->buttonClicked((int)buttonIndex);
}

-(void) initWithItems:(NSString*)title buttons:(NSMutableArray*)items deletate:(DialogProtocol*)delegate
{
    alertView = [[UIAlertView alloc] init];
    [alertView setTitle:title];
    [alertView setDelegate:self];
    
    // add buttons
    for (NSString *item in items)
    {
        [alertView addButtonWithTitle: item];
    }
    
    callBackObject = delegate;
}

-(void) show
{
    [alertView show];
}

-(void) dealloc
{
    [alertView release];
    
    [super dealloc];
}
@end

using namespace std;

MyDialog::MyDialog() 
: internalObject(NULL)
{}

MyDialog::~MyDialog()
{
    [(InternalDialog*)internalObject release];
}

void MyDialog::initWithItems(std::string title, std::vector<std::string> &buttons, DialogProtocol *dialogProtocol)
{
    // create objective-c object IntervalDialog
    internalObject = [InternalDialog alloc];
    NSMutableArray *itemStrings = [NSMutableArray arrayWithCapacity:2];
    
    vector<string>::iterator iter;
    for (iter = buttons.begin(); iter != buttons.end(); ++iter)
    {
        [itemStrings addObject:[NSString stringWithUTF8String:(*iter).c_str()]];  
    }
    
    NSString *titleNSString = [NSString stringWithUTF8String:title.c_str()];
    
    [(InternalDialog*)internalObject initWithItems:titleNSString buttons:itemStrings deletate:dialogProtocol];
}

void MyDialog::popUp()
{
    [(InternalDialog*)internalObject show];
}