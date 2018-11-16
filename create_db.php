CREATE TABLE sa_user
(
  ID int(11) NOT NULL AUTO_INCREMENT,
  Username varchar(50) NOT NULL,
  Name varchar(255),
  Surname varchar(255),
  Email varchar(255),
  Password varchar(255),
  Debt varchar(255),
  PRIMARY KEY (ID),
  UNIQUE KEY Username (Username),
  UNIQUE KEY Email (Email)
) ENGINE=MyISAM DEFAULT CHARSET=UTF8 AUTO_INCREMENT=1