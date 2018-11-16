#ifndef USER_CLASS
#define USER_CLASS
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
using namespace std;

//Måste definera SQL_Control eftersom att de beror på varandra
//class SQL_Control;
//Måste göras i SQL_Control också, fast för User. så, "class User;" alltså.
class User;
class Contact;
class Self;
class Not_Complete;

class User {
public:
    User(const string & _username,const string & _name,const string & _surname
         ,const int & _id ):
    username_{_username}, name_{_name}, surname_{_surname}, id_{_id},
    debts{std::vector<std::shared_ptr<User> >()}{};
    
    virtual ~User() = default;
    
    string name() const {return name_;}
    int id(){return id_;}
    string surname() const {return surname_;}
    string username() const {return username_;}
    shared_ptr<vector<shared_ptr<User> > >get_debts();
    shared_ptr<vector<shared_ptr<User> > >get_hans_debts(const string& _username) const;
    std::string debts_to_str(shared_ptr<User> user);
    
    void insert_end(const vector<shared_ptr<User> > & new_debts);
    void push_back(shared_ptr<User> const & user);
    int size(){return debts.size();}
    
    virtual bool change_debt (const string & _username, const double & debt) = 0;
    virtual shared_ptr<User> clone() = 0;
    
private:
    string username_;
    string name_;
    string surname_;
    int id_;
protected:
    vector<shared_ptr<User> > debts;
};


void User::insert_end(vector<shared_ptr<User> >  const & new_debts){
    debts.insert(debts.end(), new_debts.begin(), new_debts.end());
}

void User::push_back(shared_ptr<User> const & user){
    debts.push_back(user->clone());
}

shared_ptr<vector<shared_ptr<User> > >User::get_debts(){
    //cout << "get_debts     " << username() << endl;
    // shared_ptr<vector<shared_ptr<User> > >tmp =
    //make_shared<vector<shared_ptr<User> > >(debts);
    shared_ptr<vector<shared_ptr<User> > >tmp = make_shared<vector<shared_ptr<User> > >();
    //cout << "before if\n";
    
    //cout<< "before for\n";
    for(auto& d: debts){
        //cout << "in for\n";
        tmp->push_back(d->clone());
    }
    
    //cout << "hej" << endl;
    //cout << "get_debts(): " << tmp->size() << endl;
    return tmp;
}

shared_ptr<vector<shared_ptr<User> > >User::get_hans_debts(const string& _username) const{
    if(!debts.empty()){
        for(auto& cont:debts){
            if(_username == cont->username()){
                //cout << "elire hittad" << endl;
                return cont->get_debts();
            }
        }
    }
    //cout << "ingen elire\n";
    shared_ptr<vector<shared_ptr<User> > > fr;
    return fr;
}




class Not_Complete: public User, public
std::enable_shared_from_this<Not_Complete>{
public:
    Not_Complete() = default;
    Not_Complete(const string & _name, const string & _surname, const double &
                 _debt):User(_name, _name, _surname, -1), debt_{_debt}{}
    ~Not_Complete()= default;
    
    virtual bool change_debt(const string & str, const double & _debt) final;
    virtual shared_ptr<User> clone() final;
    shared_ptr<User> login(const string & _username,const string & passw);
    shared_ptr<User> create(const string & _username, const string & _name, const
                            string & surname, const string & email);
private:
    double debt_;
};

bool Not_Complete::change_debt(const string & str, const double & _debt){
    debt_ += _debt;
    return true;
}

shared_ptr<User> Not_Complete::clone(){
    return this->shared_from_this();
}

shared_ptr<User> Not_Complete::login(const string & _username,const string & passw){
    /*Från designspecifikation:
     Login kommer att ta två strängar som parametrar och returnerar en ”User”. Dessa skall vara
     användarnamnet och dess lösenord. Om de båda stämmer överens med databasen returneras
     en ”Self” från ”SQL_control”, annars returneras en ”Not_complete”.
     */
    return make_shared<Not_Complete>("notC", "notC",-1);
    
}
shared_ptr<User> Not_Complete::create(const string & _username, const string & _name, const
                                      string & surname, const string & email){
    /*Från designspecifikation:
     Create tar en parameter för varje fält i databasen(inte id) och returnerar en ”User”. Om
     användarnamnet eller email-adressen inte redan används i databasen kommer ”SQL_control” bes
     att skapa en ny användare och en ”Self” returneras. Annars returneras en ”Not_complete”.
     */
    return make_shared<Not_Complete>("notC", "notC",-1);
    
}


class Contact: public User, public std::enable_shared_from_this<Contact>{
public:
    Contact(const string & _username,const string & _name,
            const string & _surname,const int & _id,const double & _debt):
    User(_username,_name,_surname,_id), debt_{_debt}{
    }
    
    virtual ~Contact()=default;
    
    double debt(){return debt_;}
    virtual bool change_debt(const string & _username, const double & _debt) final;
    virtual shared_ptr<User> clone() final;
private:
    double debt_;
};
/*
 vector<shared_ptr<User> > Contact::get_common(shared_ptr<User> user){
 for(auto & conts: debts){
 shared_ptr<Self> slf = std::dynamic_pointer_cast<Self>(user);
 shared_ptr<Contact> cont = std::dynamic_pointer_cast<Contact>(user);
 if(cont){
 
 }else if (slf){
 
 }
 
 }
 }
 */
bool Contact::change_debt(const string & _username, const double & _debt){
    debt_ += _debt;
    return true;
}

shared_ptr<User> Contact::clone(){
    //cout << "clone" << endl;
    return this->shared_from_this();
}



class Self: public User, public std::enable_shared_from_this<Self>{
public:
    Self(const string & _username,const string & _name,
         const string & _surname,const int & _id,const string & _email):
    User(_username,_name,_surname,_id), email_{_email}, total_debt{0},
    update_list{std::vector<std::shared_ptr<User> >()}{
    }
    virtual ~Self() = default;
    
    string email() const {return email_;}
    double total() const {return total_debt;}
    shared_ptr<vector<shared_ptr<User> > > get_update();
    
    bool refresh();
    bool update();
    
    virtual bool change_debt(const string & _username, const double & debt) final;
    virtual shared_ptr<User> clone() final;
    
    
private:
    string email_;
    double total_debt;
    vector<shared_ptr<User> > update_list;
    //SQL_Control sql;
};


shared_ptr<vector<shared_ptr<User> > >Self::get_update(){
    //cout << "get_debts     " << username() << endl;
    // shared_ptr<vector<shared_ptr<User> > >tmp =
    //make_shared<vector<shared_ptr<User> > >(debts);
    shared_ptr<vector<shared_ptr<User> > >tmp = make_shared<vector<shared_ptr<User> > >();
    //cout << "before if\n";
    
    //cout<< "before for\n";
    for(auto& d: update_list){
        //cout << "in for\n";
        tmp->push_back(d->clone());
    }
    
    //cout << "hej" << endl;
    //cout << "get_debts(): " << tmp->size() << endl;
    return tmp;
}



shared_ptr<User> Self::clone(){
    return this->shared_from_this();
}

bool Self::refresh(){
    /* Från designspecifikation:
     Refresh, tar inga parametrar och returnerar en bool om det gick bra eller inte. Den ber
     ”SQL_control” att hämta information om skulderna och uppdaterar total_debt, sedan returnerar
     den true. Om ingen kontakt kunde nås med databasen returneras false.
     */
    total_debt = 0;
    for(auto& user: debts){
        shared_ptr<Contact> cont = std::dynamic_pointer_cast<Contact>(user);
        if(cont){
            total_debt+= cont->debt();
        }
    }
    return true;
}

bool Self::update(){
    /* Från designspecifikation:
     Update, tar inga parametrar och returnerar en bool. Update kallar ”SQL_control::update” för alla
     kontakter i updatelistan. True returneras om det gick bra att skriva över skuldfältet, annars false.
     */
    
    for(auto & user: update_list){
        if(user->name() == "") //if(!sql.update(user) return false;
            return false;
    }
    //return sql.update(*this);
    return true;
}

bool Self::change_debt(const string & _username, const double & debt){
    /* Från designspecifikation:
     Change_debt, tar en double och en sträng som parametrar. Dessa ska representera förändring av
     skuld samt användarnamn. Först söker igenom nuvarande skulder och ser om användarnamnet
     redan finns bland skulderna. Om användarnamnet finns så ändras den befintliga skulden och
     lägger till kontakten i update-listan, om skulden blir 0 så tas den bort, sedan kallas update och
     true returneras. Om användarnamnet inte finns bland skulderna ber den ”SQL_control” att söka i
     databasen efter användarnamnet, hittas användarnamnet läggs en ny skuld till i skuldlistan,
     kontakten läggs även till i update-listan. Update kallas och true returneras, i övriga fall returneras
     false.
     */
    for(auto & user: debts){
        
        shared_ptr<Contact> cont = std::dynamic_pointer_cast<Contact>(user);
        shared_ptr<Not_Complete> nc =
        std::dynamic_pointer_cast<Not_Complete>(user);
        if(nc){
            if(nc->username() == _username){
                return cont->change_debt("", debt);
                //update_list.push_back(user); // not complete behöver inte uppdateras
            }
        }
        if(cont){
            if(cont->username() == _username){ //kontakten vars skuld ska ändras
                vector<shared_ptr<User> > common_debts{};
                shared_ptr<Contact> his_me;
                shared_ptr<vector<shared_ptr<User> > > gd1 = cont->get_debts();
                for(auto& cont_debt: *gd1){ //kontaktens kontakter
                    shared_ptr<Contact> cnt =
                    std::dynamic_pointer_cast<Contact>(cont_debt);//ska
                    if(cnt){
                        if(cnt->username() == username()){
                            his_me = std::dynamic_pointer_cast<Contact>(cnt);
                        }
                        shared_ptr<vector<shared_ptr<User> > > gd2 = cnt->get_debts();
                        for(auto& find_self: *gd2){ //om någon av dessa har
                            //skuld till användaren
                            shared_ptr<Contact> me =
                            std::dynamic_pointer_cast<Contact>(find_self);
                            if(me){
                                if(me->username() == username()){
                                    common_debts.push_back(cnt->clone());
                                }
                            }
                        }
                        
                    }
                } // Lös skulder mellan *this, cont och common_debts här:
                
                /*
                 * Se om både *this och cont har negativ/positiv skuld till
                 samma kontakt
                 
                 * Om den skulden är större eller lika med den skuld som ska
                 läggas till, sätt hela skulden till denne kontakt
                 
                 * Annars ska den skulden lösas och en ny kontakt ska letas efter
                 
                 * upprepa tills inget är kvar av summan eller tills det inte finns
                 några kontakter kvar
                 
                 * ändra cont:s skuld med det som är kvar av skulden
                 */
                //cont->change_debt("", debt);
                //update_list.push_back(user);
                //return true;
                if(common_debts.empty()){
                    cont->change_debt("", debt);
                    update_list.push_back(user);
                    update_list.push_back(cont);
                
                    return true;
                }else{
                    double rest{debt};
                    for(auto & his_common: common_debts){
                        for(auto & my_common: debts){
                            shared_ptr<Contact> his_com =
                            std::dynamic_pointer_cast<Contact>(his_common);
                            shared_ptr<Contact> my_com =
                            std::dynamic_pointer_cast<Contact>(my_common);
                            
                            if(my_com->username() == his_com->username()){
                                if((my_com->debt()<0) && (his_com->debt() <0)){
                                    //hittat en där båda är skyldiga
                                    if(rest <0){// - på mina pengar
                                        if(rest >= his_com->debt()){//om mindre än hans totala skuld
                                            //då den är negativ
                                            his_com->change_debt("", abs(rest));
                                            my_com->change_debt("", rest);
                                            rest += his_com->debt();
                                            cont->change_debt("", rest);
                                            update_list.push_back(his_com);
                                            update_list.push_back(my_com);
                                            update_list.push_back(cont);
                                            return true;
                                            
                                        }else{//om mer än hans skuld -''-
                                            rest -= his_com->debt(); //hans är
                                            //negativ och är mindre än debt
                                            my_com->change_debt("",his_com->debt());
                                            
                                            his_com->change_debt("",abs(his_com->debt()));
                                            
                                            //update_list.push_back(his_com);
                                            update_list.push_back(my_com);
                                        }
                                    }else{//       + på mina pengar
                                        
                                    }
                                }else if((my_com->debt()>0 && his_com->debt()>0) ){
                                    //hittat en där båda har positiv skuld
                                    if(debt <0){// - på mina pengar
                                        
                                    }else{//       + på mina pengar
                                        
                                    }
                                }
                            }
                        }
                    }
                    if(his_me) // går inte igenom.. varför?
                        his_me->change_debt("", abs(rest)); // ger segmenteringsfel
                        cont->change_debt("", rest);
                        update_list.push_back(cont);
                    
                    return true;
                }
            }
            
        }
    }
    return false;
}




std::string User::debts_to_str(shared_ptr<User> user){
    shared_ptr<vector<shared_ptr<User> > > tmp = user->get_debts();
    std::string str{""};
    
    for(auto& debt: *tmp){
        shared_ptr<Contact> cnt = std::dynamic_pointer_cast<Contact>(debt);
        if(cnt)
            str+= cnt->id() + "," + std::to_string(cnt->debt()) + ":";
    }
    return str;
}

#endif


