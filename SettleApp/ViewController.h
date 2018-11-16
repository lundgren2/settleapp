//
//  ViewController.h
//  SettleApp
//
//  Created by Tobias Lundgren on 2014-09-08.
//  Copyright (c) 2014 Tobias Lundgren. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController <UITableViewDataSource, UITableViewDelegate, UIGestureRecognizerDelegate>
@property(strong, nonatomic) IBOutlet UITableView *tableView;
@property(strong, nonatomic) NSMutableArray *data;

// Main View
@property(weak, nonatomic) IBOutlet UILabel *totalDebts;
//@property (strong, nonatomic) IBOutlet UILabel *messageLabel;

// Register
@property(weak, nonatomic) IBOutlet UITextField *txtUsername;
@property(weak, nonatomic) IBOutlet UITextField *txtEmail;
@property(weak, nonatomic) IBOutlet UITextField *txtSurname;
@property(weak, nonatomic) IBOutlet UITextField *txtName;
@property(weak, nonatomic) IBOutlet UITextField *txtPassword;
@property(weak, nonatomic) IBOutlet UITextField *txtConfirmpassword;

// create ibaction for insert button:
- (IBAction)registerUser:(id)sender;

// Login
- (IBAction)loginAction:(id)sender;
@property(nonatomic, retain) NSArray *arrayLogin;
@property(nonatomic, retain) IBOutlet UITextField *userNameTextField;
@property(nonatomic, retain) IBOutlet UITextField *passwordTextField;

@property(nonatomic, readwrite, assign) IBOutlet UITextField *nextField;

// Add debt
@property(weak, nonatomic) IBOutlet UITextField *debtUsername;
@property(weak, nonatomic) IBOutlet UITextField *debtDebt;
- (IBAction)createDebt:(id)sender;

// Contact window
@property(strong, nonatomic) IBOutlet UILabel *contactName;
@property(strong, nonatomic) IBOutlet UILabel *contactDebt;

// Main window
@property(weak, nonatomic) IBOutlet UIBarButtonItem *menuButton;
//@property (weak,nonatomic) IBOutlet UIBarButtonItem *barButton;

@end
