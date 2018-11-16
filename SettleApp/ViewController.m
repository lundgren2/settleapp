//
//  ViewController.m
//  SettleApp
//
//  Created by Tobias Lundgren on 2014-09-08.
//  Copyright (c) 2014 Tobias Lundgren. All rights reserved.
//

#import "ViewController.h"
#import "AppDelegate.h"
#import "CustomTableCell.h"
#import "Debt.h"
#import "SWRevealViewController.h"

#ifndef USER_CLASS
#import "users.h"
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <cctype>
#endif

shared_ptr<Contact> string_to_contact(const std::string info, const double debt, unsigned char depth = 0);
@interface ViewController ()
{
    NSArray *_feedItems;
    
}

@property (nonatomic) std::shared_ptr<Self> SelfPtr;
@property (nonatomic) UIRefreshControl *refreshControl;

@end

static NSString *usernameSelf;
static NSString *contactNametxt, *contactDebttxt;
static std::shared_ptr<Self> SelfPtr;

@implementation ViewController
@synthesize userNameTextField, passwordTextField, data, arrayLogin, totalDebts, contactName;

- (NSString *)tableView:(UITableView *)tableView titleForDeleteConfirmationButtonForRowAtIndexPath:(NSIndexPath *)indexPath {
    return @"Ta bort";
}

-(BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    if ([NSStringFromClass([touch.view class]) isEqualToString:@"UITableViewCellContentView"])
        return NO;
    else
        return YES;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    //Instantiate NSMutableArray
    
    _feedItems = [[NSArray alloc] init];
    data = [[NSMutableArray alloc] initWithArray:_feedItems];
    
    self.contactName.text = contactNametxt;
    self.contactDebt.text = contactDebttxt;
    
    
    // Menu button
    _menuButton.target = self.revealViewController;
    _menuButton.action = @selector(revealToggle:);
   if (self.revealViewController) // Viktig if för swipe
    [self.view addGestureRecognizer:self.revealViewController.panGestureRecognizer];
    self.revealViewController.panGestureRecognizer.delegate = self;
    
    // Initialize the refresh control.
    self.refreshControl = [[UIRefreshControl alloc] init];
    self.refreshControl.backgroundColor = [UIColor lightGrayColor];
    self.refreshControl.tintColor = [UIColor whiteColor];
    [self.refreshControl addTarget:self action:@selector(uglyReload) forControlEvents:UIControlEventValueChanged];
    [_tableView addSubview:self.refreshControl];

    [self createSelf];
    
    // Set the Navigation Logo Image**
    UIImageView *logoImage = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 150, 25)];
    // if you need to resize the image to fit the UIImageView frame
    logoImage.contentMode = UIViewContentModeScaleAspectFit;
    // no extension name needed for image_name
    [logoImage setImage:[UIImage imageNamed:@"SettleApp.png"]];
    UIView *logoView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, logoImage.frame.size.width, logoImage.frame.size.height)];
    [logoView addSubview:logoImage];
    self.navigationItem.titleView = logoView;
    
    // Do any additional setup after loading the view, typically from a nib.
    
    // Show/Hide password
    CGSize hideShowSize = [@"VISAX" sizeWithAttributes:@{NSFontAttributeName:[UIFont systemFontOfSize:14.0f]}];
    UIButton *hideShow = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, hideShowSize.width, self.passwordTextField.frame.size.height)];
    [hideShow.titleLabel setFont:[UIFont systemFontOfSize:14.0f]];
    [hideShow setTitle:@"GÖM" forState:UIControlStateNormal];
    [hideShow setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    self.passwordTextField.rightView = hideShow;
    self.passwordTextField.rightViewMode = UITextFieldViewModeAlways;
    [hideShow addTarget:self action:@selector(hideShow:) forControlEvents:UIControlEventTouchUpInside];
    
    // Slide to delete function
    self.tableView.allowsMultipleSelectionDuringEditing = NO;
    
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

// Override to support conditional editing of the table view.
// This only needs to be implemented if you are going to be returning NO
// for some items. By default, all items are editable.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return YES if you want the specified item to be editable.
    return YES;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {

    Debt *contacts = [data objectAtIndex:indexPath.row];
    contactNametxt = contacts.fullname;
    contactDebttxt = [NSString stringWithFormat:@"%@kr", [contacts.amount stringValue]];
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        
        //[tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];

        Debt *contacts = [data objectAtIndex:indexPath.row];
        std::string usernamestd ([contacts.username UTF8String]);
        if ([contacts.amount doubleValue] < 0 )
            self.SelfPtr->change_debt(usernamestd, std::abs([contacts.amount doubleValue]));
        
        else if ([contacts.amount doubleValue] > 0) {
            double d = 0-[contacts.amount doubleValue];
            self.SelfPtr->change_debt(usernamestd, d);
        }
        [self updateDatabase];
        self.SelfPtr->deleteContact(usernamestd); // deleting contact after settle up
        
    }
    [self updateDatabase];
    
    [data removeObjectAtIndex:indexPath.row ];
    [self fillArray];
    [self uglyReload];
    [tableView reloadData];
    
}

- (void)hideShow:(id)sender
{
    UIButton *hideShow = (UIButton *)self.passwordTextField.rightView;
    if (!self.passwordTextField.secureTextEntry)
    {
        self.passwordTextField.secureTextEntry = YES;
        [hideShow setTitle:@"VISA" forState:UIControlStateNormal];
    }
    else
    {
        self.passwordTextField.secureTextEntry = NO;
        [hideShow setTitle:@"GÖM" forState:UIControlStateNormal];
    }
    [self.passwordTextField becomeFirstResponder];
}



-(BOOL)textFieldShouldReturn:(UITextField*)textField;
{
    
    if (textField == self.passwordTextField) {
        [textField resignFirstResponder];
        [self loginAction:0];
    }
    if (textField == self.txtConfirmpassword) {
        [textField resignFirstResponder];
        [self registerUser:0];
    }
    
    
    NSInteger nextTag = textField.tag + 1;
    // Try to find next responder
    UIResponder* nextResponder = [textField.superview viewWithTag:nextTag];
    if (nextResponder) {
        // Found next responder, so set it.
        [nextResponder becomeFirstResponder];
    } else {
        // Not found, so remove keyboard.
        [textField resignFirstResponder];
    }
    return NO; // We do not want UITextField to insert line-breaks.
    
}

- (void) updateDatabase {
    Self _self = *self.SelfPtr; // set self
    std::shared_ptr<vector<shared_ptr<User> > > debtUpd = _self.get_debts();
    NSString *userns = [NSString stringWithCString:_self.username().c_str()
                                          encoding:NSUTF8StringEncoding];
    
    
    NSString *selfdebt = [NSString stringWithCString:_self.debts_to_str().c_str()
                                            encoding:NSUTF8StringEncoding];
    
    
    NSString *strURL = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?debtUsername=%@&debtStr=%@", userns, selfdebt];
    [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL]];
    
   // NSLog(@"Uppdaterar self: %@ STRURL: %@", selfdebt, strURL);
    
    for (auto & u : *debtUpd) {
        std::shared_ptr<Contact> upd = std::dynamic_pointer_cast<Contact>(u);
        if (upd) {
            NSString *usernamens = [NSString stringWithCString:upd->username().c_str()
                                                      encoding:NSUTF8StringEncoding];
            NSString *debtns = [NSString stringWithCString:upd->debts_to_str().c_str()
                                                  encoding:NSUTF8StringEncoding];
            
           // NSLog(@"Uppdatera vän: %@", debtns);
            NSString *strURL2 = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?debtUsername=%@&debtStr=%@", usernamens, debtns];
            [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL2]];
            
        }
    }
    _self.refresh();
}

- (void) uglyReload {
    NSString *strURL = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?i=getUserbyName&id=%@",usernameSelf];
    
    NSLog(@"Användare: %@", usernameSelf);
    NSData *dataURL = [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL]];
    NSString *userResult = [[NSString alloc] initWithData:dataURL encoding:NSUTF8StringEncoding];
    
    std::string userResultstd ([userResult UTF8String]);
    if (userResultstd[0] != ' ') {
        self.SelfPtr = string_to_self(userResultstd);
        self.SelfPtr->refresh();
        
    } else {
        self.SelfPtr = make_shared<Self> ("", "", "", 0,"");
        return;
    }
    
    self.SelfPtr->refresh();
    
    if (self.SelfPtr->total() > 0)
        totalDebts.textColor = [UIColor colorWithRed:(57.0/255.0) green:(169.0/255.0) blue:(136.0/255.0) alpha:1];
    else if (self.SelfPtr->total() < 0)
        totalDebts.textColor = [UIColor colorWithRed:(231.0/255.0) green:(79.0/255.0) blue:(43.0/255.0) alpha:1];
    else totalDebts.textColor = [UIColor grayColor];
    totalDebts.text = [[NSString alloc] initWithFormat:@"%.0fkr", self.SelfPtr->total()]; // 0 decimals
    
    
    [self fillArray];

    // As this block of code is run in a background thread, we need to ensure the GUI
    // update is executed in the main thread
    [self performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
    
}

- (void) fillArray
{
    NSLog(@"fillArray");
    std::shared_ptr<vector<shared_ptr<User> > > fillVector = self.SelfPtr->get_debts();
    NSMutableArray *tableArray = [[NSMutableArray alloc] init];
    
    if (self.SelfPtr) {
        
        
        if (fillVector->size() != 0) {
            NSString *username, *name, *surname;
            NSNumber *debt;
            
            for (auto & cont : *fillVector) {
                std::shared_ptr<Contact> cnt = std::dynamic_pointer_cast<Contact>(cont);
                Debt *contacts = [[Debt alloc] init];
                if (cnt) {
                    username = [NSString stringWithUTF8String:cnt->username().c_str()];
                    name = [NSString stringWithUTF8String:cnt->name().c_str()];
                    surname = [NSString stringWithUTF8String:cnt->surname().c_str()];;
                    debt = [NSNumber numberWithDouble:cnt->debt()];
                    NSString *fullName = [NSString stringWithFormat:@"%@ %@", name, surname];
                    contacts.username = username;
                    contacts.fullname = fullName;
                    contacts.amount = debt;
                    
                    // Add Cell to tableArray
                    [tableArray addObject: contacts];
                }
                
                std::shared_ptr<Not_Complete> nc = std::dynamic_pointer_cast<Not_Complete>(cont);
                if (nc) {
                    username = [NSString stringWithUTF8String:nc->username().c_str()];
                    name = [NSString stringWithUTF8String:nc->name().c_str()];
                    surname = [NSString stringWithUTF8String:nc->surname().c_str()];;
                    debt = [NSNumber numberWithDouble:nc->debt()];
                    NSString *fullName = [NSString stringWithFormat:@"%@ %@", name, surname];
                    contacts.username = username;
                    contacts.fullname = fullName;
                    contacts.amount = debt;
                    
                    [tableArray addObject: contacts];
                }
            }
            // Sets array with Contact to tableView
            _feedItems = tableArray;
            data = [[NSMutableArray alloc] initWithArray:_feedItems];
            
        }
    }
}
- (void) createSelf
{
    if (!self.SelfPtr ) {
        NSLog(@"createSelf");
        std::shared_ptr<Self> SelfPtr;
        NSString *strURL = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?i=getUserbyName&id=%@",usernameSelf];
        
        NSLog(@"Användare: %@", usernameSelf);
        NSData *dataURL = [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL]];
        NSString *userResult = [[NSString alloc] initWithData:dataURL encoding:NSUTF8StringEncoding];
        
        std::string userResultstd ([userResult UTF8String]);
        if (userResultstd[0] != ' ') {
            self.SelfPtr = string_to_self(userResultstd);
            self.SelfPtr->refresh();
            
        } else {
            self.SelfPtr = make_shared<Self> ("", "", "", 0,"");
            return;
        }
        
        self.SelfPtr->refresh();
        if (self.SelfPtr->total() > 0)
            totalDebts.textColor = [UIColor colorWithRed:(57.0/255.0) green:(169.0/255.0) blue:(136.0/255.0) alpha:1];
        else if (self.SelfPtr->total() < 0)
            totalDebts.textColor = [UIColor colorWithRed:(231.0/255.0) green:(79.0/255.0) blue:(43.0/255.0) alpha:1];
        else totalDebts.textColor = [UIColor grayColor];
        totalDebts.text = [[NSString alloc] initWithFormat:@"%.0fkr", self.SelfPtr->total()]; // 0 decimals
        
        
        [self fillArray];
        
    }else {
        NSLog(@"!createSelf");
        return;
    }
    
}

- (void)reloadData
{
    NSLog(@"reloadData");
    // Reload table data
    [self.tableView reloadData];
    
    // End the refreshing
    if (self.refreshControl) {
       /*
        NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
        [formatter setDateFormat:@"MMM d, HH:mm"];
        NSString *title = [NSString stringWithFormat:@"Senast uppdaterad: %@", [formatter stringFromDate:[NSDate date]]];
        NSDictionary *attrsDictionary = [NSDictionary dictionaryWithObject:[UIColor whiteColor]
                                                                    forKey:NSForegroundColorAttributeName];
        NSAttributedString *attributedTitle = [[NSAttributedString alloc] initWithString:title attributes:attrsDictionary];
        self.refreshControl.attributedTitle = attributedTitle;
        */
        [self.refreshControl endRefreshing];
    }
}

#pragma mark Table View Delegate Methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    if ([data count] > 0) {
        
        self.tableView.separatorStyle = UITableViewCellSeparatorStyleSingleLine;
        return 1;
        
    } else {
        
        // Display a message when the table is empty
        UILabel *messageLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, self.view.bounds.size.width, self.view.bounds.size.height)];
        
        messageLabel.text = @"Tryck på (+) för att lägga till en skuld. Dra ner för att uppdatera listan.";
        messageLabel.textColor = [UIColor grayColor];
        messageLabel.numberOfLines = 10;
        messageLabel.textAlignment = NSTextAlignmentCenter;
        messageLabel.font = [UIFont fontWithName:@"Palatino-Italic" size:20];
        [messageLabel sizeToFit];
        
        self.tableView.backgroundView = messageLabel;
        self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
        
    }
    
    return 0;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of feed items (initially 0)
    //return _feedItems.count;
    return [data count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Retrieve cell
    static NSString *cellIdentifier = @"ContactCell"; // static?
    CustomTableCell *myCell = [tableView dequeueReusableCellWithIdentifier: cellIdentifier forIndexPath:indexPath];
    
    if (myCell == nil){
        myCell = [[CustomTableCell alloc]initWithStyle: UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
    }
    
    Debt *contacts = [data objectAtIndex:indexPath.row];
        
    myCell.cellName.text = contacts.fullname;
    myCell.cellTotal.text = [NSString stringWithFormat:@"%@kr", [contacts.amount stringValue]];
    
    if ([contacts.amount doubleValue] > 0)
        myCell.cellTotal.textColor = [UIColor colorWithRed:(57.0/255.0) green:(169.0/255.0) blue:(136.0/255.0) alpha:1];
    else if ([contacts.amount doubleValue] < 0)
        myCell.cellTotal.textColor = [UIColor colorWithRed:(231.0/255.0) green:(79.0/255.0) blue:(43.0/255.0) alpha:1];
    else myCell.cellTotal.textColor = [UIColor grayColor];
    
    //myCell.textLabel.text = [data objectAtIndex:indexPath.row];
    
    return myCell;
}

// Dismiss keyboard
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    [self.view endEditing:YES];
}

// Insert to Database function ***************************

// Register user
- (IBAction)registerUser:(id)sender
{
    // create string contains url address for php file, the file name is phpFile.php, it receives parameter :name
    NSString *strURL = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?username=%@&email=%@&name=%@&surname=%@&password=%@", _txtUsername.text, _txtEmail.text, _txtName.text, _txtSurname.text, _txtPassword.text];
    
    // to execute php code
    NSData *dataURL = [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL]];
    
    // to receive the returend value
    NSString *strResult = [[NSString alloc] initWithData:dataURL encoding:NSUTF8StringEncoding];
    
    NSLog(@"%@", strResult);
}

// Create Debt
- (IBAction)createDebt:(id)sender
{
    NSLog(@"createDebt");
    Self _self = *self.SelfPtr; // set self
    
    std::shared_ptr<vector<shared_ptr<User> > > debtVec = _self.get_debts();
    std::string usernamestd ([_debtUsername.text UTF8String]);
    double userDebtstd  = ([self.debtDebt.text doubleValue]);
    
    BOOL existing = false;
    
    for (auto & debt : *debtVec) {
        std::shared_ptr<Contact> cnt = std::dynamic_pointer_cast<Contact>(debt);
        
        if (cnt) {
            if (cnt->username() == usernamestd) {
                
                NSLog(@"Användare finns: ");
                _self.change_debt(usernamestd,userDebtstd);
                existing = true;
            }
        }
        
        
        std::shared_ptr<Not_Complete> nc = std::dynamic_pointer_cast<Not_Complete>(debt);
        
        if (nc) {
            if (nc->username() == usernamestd) {
                NSLog(@"Användare finns: ");
                _self.change_debt(usernamestd,userDebtstd);
                existing = true;
            }
        }
        
    }
    
    if (!existing) {
        NSString *strURL = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?i=getUserbyName&id=%@", _debtUsername.text];
        NSData *dataURL = [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL]];
        NSString *strResult = [[NSString alloc] initWithData:dataURL encoding:NSUTF8StringEncoding];
        std::string strResultstd ([strResult UTF8String]);
        
        if (strResultstd == "") { // User doesn't exist
            std::stringstream ss (usernamestd);
            std::string namenc;
            std::string surnamenc;
            ss >> namenc >> surnamenc;
            shared_ptr<Not_Complete> noCnt = std::make_shared<Not_Complete> (usernamestd, namenc, surnamenc, 0);

            _self.push_back(noCnt);
            _self.change_debt(noCnt->username(), userDebtstd);
            
        }else{
            
            std::shared_ptr<Contact> debtCont = (string_to_contact(strResultstd, 0));
            
            std::shared_ptr<Contact> selfCont = (make_shared<Contact>(_self.username(), _self.name(),  _self.surname(), _self.Id(), 0));
            
            debtCont->push_back(selfCont);
            _self.push_back(debtCont);
            _self.change_debt(debtCont->username(), userDebtstd);
        }
    }
    
    std::shared_ptr<vector<shared_ptr<User> > > debtUpd = _self.get_debts();
    NSString *userns = [NSString stringWithCString:_self.username().c_str()
                                          encoding:NSUTF8StringEncoding];
    
    
    NSString *selfdebt = [NSString stringWithCString:_self.debts_to_str().c_str()
                                            encoding:NSUTF8StringEncoding];
    
    
    NSString *strURL = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?debtUsername=%@&debtStr=%@", userns, selfdebt];
    [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL]];
    
    for (auto & u : *debtUpd) {
        std::shared_ptr<Contact> upd = std::dynamic_pointer_cast<Contact>(u);
        if (upd) {
            NSString *usernamens = [NSString stringWithCString:upd->username().c_str()
                                                      encoding:NSUTF8StringEncoding];
            NSString *debtns = [NSString stringWithCString:upd->debts_to_str().c_str()
                                                  encoding:NSUTF8StringEncoding];
            
           // NSLog(@"Uppdatera vän: %@", debtns);
            NSString *strURL2 = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?debtUsername=%@&debtStr=%@", usernamens, debtns];
            [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL2]];
        }
    }
    
    _self.refresh();
}


- (void) loginAction:(id)sender {
    if ([userNameTextField.text isEqualToString:@""] || [passwordTextField.text isEqualToString:@""]) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Fel vid inloggning" message:@"Vänligen fyll i alla fält" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alert show];
        return;
    }
    NSString *strURL = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?userName=%@&password=%@",userNameTextField.text, passwordTextField.text];
    NSData *dataURL = [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL]];
    
    // to receive the returend value
    NSString *strResult = [[NSString alloc] initWithData:dataURL encoding:NSUTF8StringEncoding];
    if ([strResult isEqualToString:@"1"] )
    {
        usernameSelf = userNameTextField.text;
        [self performSegueWithIdentifier:@"login" sender:self];
    }else{
        // invalid information
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Fel vid inloggning" message:@"Felaktigt användarnamn eller lösenord." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alert show];
        return;
    }
}

- (void)viewDidUnload
{
    NSLog(@"viewDidUnload");
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
    self.arrayLogin = nil;
    self.userNameTextField = nil;
    self.passwordTextField = nil;
}


std::shared_ptr<Self> string_to_self(const std::string info){
    std::stringstream ss;
    ss.str(info);
    std::string username{""};
    std::string name{""};
    std::string surname{""};
    int Id{0};
    std::string email{""};
    
    ss >> username >> name >> surname >> Id >> email;
    
    int nrContacts{};
    for (int i{}; i<info.size(); i++) {
        if (info[i] == ':')
            nrContacts++;
    }
    std::shared_ptr<Self> _self = make_shared<Self>(username,name,surname,Id,email);
    
    for(int i{}; i < nrContacts; i++) {
        int contId {};
        double contDebt {};
        char c;
        ss >> std::ws;
        if (std::isalpha((char)ss.peek())) {
            
            std::string namenc;
            std::string surnamenc = "";
            std::getline(ss,namenc,',');
            ss >> std::ws;
            while(true){
                
                ss>>c;
                if(c != ','){
                    surnamenc+=c;
                }else{
                    break;
                }
                
            }
            
            std::string usernamenc = namenc + " " + surnamenc;

            if(ss.peek() == ',')
                ss >> c;
            ss >> contDebt >> c;
            std::shared_ptr<Not_Complete> nc = make_shared<Not_Complete>(usernamenc, namenc, surnamenc, contDebt);
            _self->push_back(nc);
            
        }else{
            ss >> contId >> c >> contDebt >> c;
            
            NSString *strFromInt = [NSString stringWithFormat:@"%d", contId];
            //NSLog (@"StrFromInt: %@", strFromInt);
            NSString *strURL = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?i=getUserbyID&id=%@",strFromInt];
            NSData *dataURL = [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL]];
            NSString *userResult = [[NSString alloc] initWithData:dataURL encoding:NSUTF8StringEncoding];
         
            std::string userResultstd ([userResult UTF8String]);
           // NSLog (@"userResult: %@", userResult);
            
            std::shared_ptr<Contact> ContPtr = string_to_contact(userResultstd, contDebt, 0);
            _self->push_back(ContPtr);
            
        }
    }
    return _self;
}

// TODO: refactor
shared_ptr<Contact> string_to_contact(const std::string info, const double debt, unsigned char depth){
    std::stringstream ss;
    ss.str(info);
    std::string username{""};
    std::string name{""};
    std::string surname{""};
    int Id;
    ss >> username >> name >> surname >> Id;
        
    std::shared_ptr<Contact> cont = make_shared<Contact>(username,name,surname,Id,debt);
    if (depth < 3) {
        int nrContacts{};
        for (int i{}; i<info.size(); i++) {
            if (info[i] == ':')
                nrContacts++;
        }
        for (int i{}; i<nrContacts; i++) {
            //NSLog(@"string_to_contact");
            int contId {};
            double contDebt {};
            char c;
            if(isalpha(ss.peek())) {
                int contId {};
                double contDebt {};
                char c;
                ss >> std::ws;
                if (std::isalpha((char)ss.peek())) {
                    
                    std::string namenc;
                    std::string surnamenc = "";
                    std::getline(ss,namenc,',');
                    //if(ss.peek() == ',')
                    ss >> std::ws;
                    while(true){
                        
                        ss>>c;
                        if(c != ','){
                            surnamenc+=c;
                        }else{
                            break;
                        }
                    }
                    
                    if(ss.peek() == ',')
                        ss >> c;
                    ss >> contDebt >> c;
                }
            }else{
                ss >> contId >> c >> contDebt >> c;
                
                NSString *strFromInt = [NSString stringWithFormat:@"%d",contId];
                NSString *strURL = [NSString stringWithFormat:@"http://demo.lundgrendesign.se/settleapp/db.php?i=getUserbyID&id=%@",strFromInt];
                NSData *dataURL = [NSData dataWithContentsOfURL:[NSURL URLWithString:strURL]];
                NSString *userResult = [[NSString alloc] initWithData:dataURL encoding:NSUTF8StringEncoding];
                std::string userResultstd ([userResult UTF8String]);
                
                std::shared_ptr<Contact> ContPtr = string_to_contact(userResultstd, contDebt, ++depth);
                cont->push_back(ContPtr);
            }
        }
    }
    return cont;
}

@end
