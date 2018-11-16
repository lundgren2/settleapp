#import <UIKit/UIKit.h>

@interface CustomTableCell : UITableViewCell
@property(weak, nonatomic) IBOutlet UILabel *cellName;
@property(weak, nonatomic) IBOutlet UILabel *cellTotal;
@end