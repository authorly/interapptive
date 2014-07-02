#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "MyDialog.h"
#import "EAGLView.h"

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
    callBackObject->buttonClicked(buttonIndex);
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

// implementatioin of Login

@interface LoginViewController : UIViewController<UITextFieldDelegate, UITableViewDataSource> {
    
    UITableView *mainLoginInfo;
    UITextField *userNameFeild;
    UITextField *passWordFeild;
    NSString *initUserName;
    NSString *initPassword;
}
@property(nonatomic, retain) UITextField *userNameFeild;
@property(nonatomic, retain) UITextField *passWordFeild;

- (void) buttonClicked;
- (id) initWithUsername:(NSString *)userName password:(NSString *)password;

@end


@implementation LoginViewController

@synthesize userNameFeild;
@synthesize passWordFeild;


-(void)dealloc{
    
    [userNameFeild release];
    [passWordFeild release];
    [initUserName release];
    [initPassword release];
    [super dealloc];
    
}

- (id) initWithUsername:(NSString *)userName password:(NSString *)password {
    self = [super init];
    initUserName = userName;
    [initUserName retain];
    initPassword = password;
    [initPassword retain];
    
    return self;
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    
    self.userNameFeild = nil;
    self.passWordFeild = nil;
    
}

- (void)buttonClicked
{
    // Bug: must call this two times to hide keyboard. 
    [[mainLoginInfo window] endEditing:YES];
    [[mainLoginInfo window] endEditing:YES];
    
    [self.view removeFromSuperview];
    [self release];
    
    LoginProtocol *loginDelegate = Login::sharedLogin()->getDelegate();
    if (loginDelegate)
    {
        loginDelegate->buttonClicked([userNameFeild.text UTF8String], [passWordFeild.text UTF8String]);
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    mainLoginInfo.backgroundColor = [UIColor clearColor];
    userNameFeild.text = @"";
    passWordFeild.text = @"";
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    CGFloat screenHeight = screenRect.size.height;
    CGFloat screenWidth = screenRect.size.width;
    
    mainLoginInfo  = [[UITableView alloc] initWithFrame:CGRectMake((screenHeight - 320)/2,screenWidth/7, 320,150) style:UITableViewStyleGrouped];
    mainLoginInfo.dataSource = self;
    [self.view addSubview:mainLoginInfo];
    
    // BookFair logo above UITableView
    UIImageView *imgView = [[UIImageView alloc] initWithFrame:CGRectMake((screenHeight - 300)/2,  mainLoginInfo.frame.origin.y - 27, 300, 54)];
    [imgView setImage:[UIImage imageNamed:@"logo.png"]];
    [self.view  addSubview:imgView];
    
    // Login button
    UIButton *button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [button addTarget:self
               action:@selector(buttonClicked)
     forControlEvents:UIControlEventTouchUpInside];
    [button setTitle:@"Login" forState:UIControlStateNormal];
    
    button.frame = CGRectMake(screenHeight/2 - 80, mainLoginInfo.frame.origin.y + 125, 160.0, 40.0);
    [self.view addSubview:button];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [[event allTouches] anyObject];
    
    if ([userNameFeild isFirstResponder] && [touch view] != userNameFeild) {
        [userNameFeild resignFirstResponder];
        
        [[mainLoginInfo window] endEditing:YES];
    }
    
    if ([passWordFeild isFirstResponder] && [touch view] != passWordFeild) {
        [passWordFeild resignFirstResponder];
        
        [[mainLoginInfo window] endEditing:YES];
    }
    
    [super touchesBegan:touches withEvent:event];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    [[mainLoginInfo window] endEditing:YES];
    return YES;
}


//- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
//{
//    // Return YES for supported orientations
//    return YES;
//}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    
    // Return the number of sections.
    return 1;
}
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    return 44.0;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    
    return 2;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    // Configure the cell...
    
    tableView.scrollEnabled = NO;
    tableView.separatorStyle= UITableViewCellSeparatorStyleNone;
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    
    switch (indexPath.row) {
        case 0:
            cell.textLabel.text = @"Email";
            userNameFeild = [[UITextField alloc] initWithFrame:CGRectMake(115, 7, 185, 31)];
            userNameFeild.textAlignment = UITextAlignmentLeft;
            userNameFeild.textColor = [UIColor blueColor];
            userNameFeild.clearButtonMode  = UITextFieldViewModeAlways;
            userNameFeild.delegate = self;
            userNameFeild.borderStyle = UITextBorderStyleBezel;
            userNameFeild.text = initUserName;
            userNameFeild.font = [UIFont fontWithName:@"Helvetica" size:18.0];
            userNameFeild.autocapitalizationType = UITextAutocapitalizationTypeNone;
            userNameFeild.keyboardType = UIKeyboardTypeEmailAddress;
            userNameFeild.adjustsFontSizeToFitWidth = YES;
            userNameFeild.minimumFontSize = 4.0;
            [cell.contentView addSubview:userNameFeild];
            break;
        case 1:
            cell.textLabel.text = @"Password";
            passWordFeild = [[UITextField alloc] initWithFrame:CGRectMake(115, 7, 185, 31)];
            passWordFeild.textAlignment = UITextAlignmentLeft;
            passWordFeild.textColor = [UIColor blueColor];
            passWordFeild.clearButtonMode = UITextFieldViewModeAlways;
            passWordFeild.secureTextEntry = YES;
            passWordFeild.delegate = self;
            passWordFeild.borderStyle = UITextBorderStyleBezel;
            passWordFeild.text = initPassword;
            passWordFeild.font = [UIFont fontWithName:@"Helvetica" size:18.0];
            passWordFeild.autocapitalizationType = UITextAutocapitalizationTypeNone;
            userNameFeild.adjustsFontSizeToFitWidth = YES;
            userNameFeild.minimumFontSize = 4.0;
            [cell.contentView addSubview:passWordFeild];
            
            break;
        default:
            break;
    }
    
    return cell;
}

//- (void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
//{
//    if (toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft ||
//        toInterfaceOrientation == UIInterfaceOrientationLandscapeRight)
//    {
//        
//        mainLoginInfo.frame = CGRectMake(30,60,420, 150);
//        userNameFeild.frame = CGRectMake(145,12,250,31);
//        passWordFeild.frame = CGRectMake(145, 12,250, 31);
//        
//    }
//    
//    else
//    {
//        
//        
//        mainLoginInfo.frame = CGRectMake(0,90, 320,150);
////        logIn.frame = CGRectMake(23, 246, 274, 46);
//        userNameFeild.frame = CGRectMake(115,12, 180, 31);
//        passWordFeild.frame = CGRectMake(115,12, 180, 31);
//        
//    }
//}

@end

static LoginViewController *g_loginViewController = NULL;

Login* Login::instance = NULL;

Login* Login::sharedLogin()
{
    if (!instance)
    {
        instance = new Login();
    }
    return instance;
}

Login::Login() : delegate(NULL)
{
    
}

void Login::setDelegate(LoginProtocol *delegate)
{
    this->delegate = delegate;
}

LoginProtocol* Login::getDelegate()
{
    return delegate;
}

void Login::popUp(const char* userName, const char* password)
{
    LoginViewController *viewContorller =  [[LoginViewController alloc] initWithUsername:[NSString stringWithUTF8String:userName ] password:[NSString stringWithUTF8String:password]];
    g_loginViewController = viewContorller;
    
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    CGFloat screenHeight = screenRect.size.height;
    CGFloat screenWidth = screenRect.size.width;
    
    viewContorller.view.frame = CGRectMake(0, 0, screenHeight, screenWidth);
    
    [[EAGLView sharedEGLView] addSubview:viewContorller.view];
}

void Login::hide()
{
    if (g_loginViewController)
    {
        [g_loginViewController.view removeFromSuperview];
        [g_loginViewController release];
        g_loginViewController = NULL;
    }
}

