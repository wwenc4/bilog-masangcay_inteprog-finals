#include <iostream>
#include <string>
using namespace std;

// Base User class
class User {
protected:
    string username;
public:
    User(const string& uname) : username(uname) {}
    virtual ~User() {}

    string getUsername() const {
        return username;
    }

    virtual void showMenu() = 0;
    virtual void describeRole() {
        cout << "User." << endl;
    }
};

// Admin class
class Admin : public User {
private:
    string password;
public:
    Admin(const string& uname, const string& pwd) : User(uname), password(pwd) {}

    bool checkPassword(const string& input) const {
        return input == password;
    }

    void showMenu() override {
        cout << "\nAdmin Menu for " << username << ":\n";
        cout << "1. Manage bookings\n2. View reports\n3. Logout\n";
    }

    void describeRole() override {
        cout << username << " is an Admin with full access." << endl;
    }
};

// Guest class
class Guest : public User {
public:
    Guest(const string& uname) : User(uname) {}

    void showMenu() override {
        cout << "\nGuest Menu for " << username << ":\n";
        cout << "1. View room types\n2. Make a booking\n3. Logout\n";
    }

    void describeRole() override {
        cout << username << " is a Guest with limited access." << endl;
    }
};

// Room options display
void showRoomTypes() {
    cout << "We have the following room types available:\n";
    cout << "1. Single Room - $100 per night\n";
    cout << "2. Double Room - $150 per night\n";
    cout << "3. Suite - $250 per night\n";
}

// Decorative banner
void Banner() {
    cout << "*************************************************" << endl;
}

int main() {
    string admins[] = {"raymund", "dane"};
    string password = "A1234!";
    int position;
    string adminName, adminPassword;

    cout << "\nWelcome to the Hotel Management System" << endl;
    Banner();
    cout << "1. Admin\n2. Guest\n3. Exit" << endl;
    Banner();
    cout << "Please enter your Role (1-3): ";
    cin >> position;

    switch (position) {
        case 1:
            cout << "Enter admin username: ";
            cin >> adminName;
            cout << "Enter password: ";
            cin >> adminPassword;

            if ((adminName == admins[0] || adminName == admins[1]) && adminPassword == password) {
                Admin admin(adminName, password);
                cout << "Welcome Admin " << admin.getUsername() << "!" << endl;
                admin.describeRole();  // polymorphism
                admin.showMenu();
            } else {
                cout << "Invalid credentials! Exiting..." << endl;
            }
            break;

        case 2: {
            Guest guest("Guest");
            cout << "Welcome, Guest!" << endl;
            guest.describeRole();  // polymorphism
            showRoomTypes();

            int roomType;
            cout << "Please select a room type (1-3): ";
            cin >> roomType;

            if (roomType < 1 || roomType > 3) {
                cout << "Invalid room type selected!" << endl;
            } else {
                cout << "You have selected room type " << roomType << "." << endl;
                guest.showMenu();
            }
            break;
        }

        case 3:
            cout << "Exiting program..." << endl;
            return 0;

        default:
            cout << "Invalid option! Program will now exit." << endl;
    }

    return 0;
}
