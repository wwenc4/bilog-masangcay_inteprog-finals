#include <iostream>
#include <string>
#include <limits> // For numeric_limits
using namespace std;

// Forward declarations
void showRoomTypes();
void makeBooking();

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
    virtual void describeRole() = 0;

    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
        int choice;
        do {
            cout << "\nAdmin Menu for " << username << ":\n";
            cout << "1. Manage bookings\n2. View reports\n3. Logout\n";
            cout << "Enter your choice: ";
            
            while (!(cin >> choice)) {
                cout << "Invalid input. Please enter a number: ";
                clearInputBuffer();
            }

            switch (choice) {
                case 1:
                    cout << "Managing bookings..." << endl;
                    break;
                case 2:
                    cout << "Viewing reports..." << endl;
                    break;
                case 3:
                    cout << "Logging out..." << endl;
                    return;
                default:
                    cout << "Invalid choice! Please try again." << endl;
            }
        } while (true);
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
        int choice;
        do {
            cout << "\nGuest Menu for " << username << ":\n";
            cout << "1. View room types\n2. Make a booking\n3. Logout\n";
            cout << "Enter your choice: ";
            
            while (!(cin >> choice)) {
                cout << "Invalid input. Please enter a number: ";
                clearInputBuffer();
            }

            switch (choice) {
                case 1:
                    showRoomTypes();
                    handleRoomTypeSelection();
                    break;
                case 2:
                    makeBooking();
                    break;
                case 3:
                    cout << "Logging out..." << endl;
                    return;
                default:
                    cout << "Invalid choice! Please try again." << endl;
            }
        } while (true);
    }

    void describeRole() override {
        cout << username << " is a Guest with booking access." << endl;
    }

private:
    void handleRoomTypeSelection() {
        int subChoice;
        cout << "\n1. Book a room\n2. Return to menu\n";
        cout << "Enter choice: ";
        
        while (!(cin >> subChoice) || (subChoice != 1 && subChoice != 2)) {
            cout << "Invalid input. Please enter 1 or 2: ";
            clearInputBuffer();
        }

        if (subChoice == 1) {
            makeBooking();
        } else {
            cout << "Returning to menu..." << endl;
        }
    }
};

// Room options display
void showRoomTypes() {
    cout << "\nWe have the following room types available:\n";
    cout << "1. Single Room - $100 per night\n";
    cout << "2. Double Room - $150 per night\n";
    cout << "3. Suite - $250 per night\n";
}

// Booking function
void makeBooking() {
    cout << "\nEnter room type to book (1-3): ";
    int roomChoice;
    while (!(cin >> roomChoice) || roomChoice < 1 || roomChoice > 3) {
        cout << "Invalid choice. Please enter 1, 2, or 3: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    string roomType;
    switch (roomChoice) {
        case 1: roomType = "Single Room"; break;
        case 2: roomType = "Double Room"; break;
        case 3: roomType = "Suite"; break;
    }
    
    cout << "Booking confirmed for " << roomType << "!" << endl;
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

    while (!(cin >> position) || position < 1 || position > 3) {
        cout << "Invalid input! Please enter 1, 2, or 3: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    switch (position) {
        case 1: {
            cout << "Enter admin username: ";
            cin >> adminName;
            cout << "Enter password: ";
            cin >> adminPassword;

            if ((adminName == admins[0] || adminName == admins[1]) && adminPassword == password) {
                Admin admin(adminName, password);
                cout << "Welcome Admin " << admin.getUsername() << "!" << endl;
                admin.showMenu();
            } else {
                cout << "Invalid credentials! Exiting..." << endl;
            }
            break;
        }

        case 2: {
            string guestName;
            cout << "Enter your name: ";
            cin.ignore();
            getline(cin, guestName);
            
            Guest guest(guestName);
            cout << "Welcome, " << guest.getUsername() << "!" << endl;
            guest.showMenu();
            break;
        }

        case 3:
            cout << "Exiting program..." << endl;
            return 0;
    }

    return 0;
}