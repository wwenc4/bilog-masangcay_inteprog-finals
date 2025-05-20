#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <fstream>
#include <iomanip> // for setprecision

using namespace std;

// Forward declarations
void showRoomTypes();

// Booking Struct
struct Booking {
    string guestName;
    string roomType;
    int nights;
    double totalCost;

    string serialize() const {
        stringstream ss;
        ss << guestName << "," << roomType << "," << nights << "," << fixed << setprecision(2) << totalCost;
        return ss.str();
    }

    static Booking deserialize(const string& line) {
        Booking b;
        stringstream ss(line);
        string temp;

        getline(ss, b.guestName, ',');
        getline(ss, b.roomType, ',');

        getline(ss, temp, ',');
        if (temp.empty()) throw invalid_argument("Invalid nights value");
        b.nights = stoi(temp);

        getline(ss, temp, ',');
        if (temp.empty()) throw invalid_argument("Invalid total cost value");
        b.totalCost = stod(temp);

        return b;
    }
};

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
private:
    vector<Booking> bookings;

public:
    Guest(const string& uname) : User(uname) {
        loadBookingsFromFile();
    }

    void showMenu() override {
        int choice;
        do {
            cout << "\nGuest Menu for " << username << ":\n";
            cout << "1. View room types\n2. Make a booking\n3. View My Reservations\n4. Logout\n";
            cout << "Enter your choice: ";
            
            while (!(cin >> choice)) {
                cout << "Invalid input. Please enter a number: ";
                clearInputBuffer();
            }

            switch (choice) {
                case 1:
                    viewRoomTypesAndOptionallyBook();
                    break;
                case 2:
                    makeAndStoreBooking();
                    break;
                case 3:
                    viewMyBookings();
                    break;
                case 4:
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
    void viewRoomTypesAndOptionallyBook() {
        showRoomTypes();
        int subChoice;
        cout << "\n1. Book a room\n2. Return to menu\n";
        cout << "Enter choice: ";
        
        while (!(cin >> subChoice) || (subChoice != 1 && subChoice != 2)) {
            cout << "Invalid input. Please enter 1 or 2: ";
            clearInputBuffer();
        }

        if (subChoice == 1) {
            makeAndStoreBooking();
        } else {
            cout << "Returning to menu..." << endl;
        }
    }

    void makeAndStoreBooking() {
        int roomChoice;
        int nights;
        double rate = 0.0;
        string roomType;
        showRoomTypes();
        cout << "\nEnter room type to book (1-3): ";
        while (!(cin >> roomChoice) || roomChoice < 1 || roomChoice > 3) {
            cout << "Invalid choice. Please enter 1, 2, or 3: ";
            clearInputBuffer();
        }

        switch (roomChoice) {
            case 1: roomType = "Single Room"; rate = 100.0; break;
            case 2: roomType = "Double Room"; rate = 150.0; break;
            case 3: roomType = "Suite"; rate = 250.0; break;
        }

        cout << "Enter number of nights: ";
        while (!(cin >> nights) || nights <= 0) {
            cout << "Please enter a valid number of nights: ";
            clearInputBuffer();
        }

        double totalCost = rate * nights;

        Booking b { username, roomType, nights, totalCost };
        bookings.push_back(b);

        cout << "Booking confirmed for " << roomType
             << " for " << nights << " nights. Total: $"
             << fixed << setprecision(2) << totalCost << "\n";
        cout << "Your booking details have been saved.\n";

        saveBookingsToFile();
        cout << "Returning to menu..." << endl;
    }

    void viewMyBookings() {
        if (bookings.empty()) {
            cout << "You have no bookings yet.\n";
        } else {
            cout << "\nYour current bookings:\n";
            for (size_t i = 0; i < bookings.size(); ++i) {
                cout << i + 1 << ". " << bookings[i].roomType
                     << " | Nights: " << bookings[i].nights
                     << " | Total: $" << fixed << setprecision(2) << bookings[i].totalCost << "\n";
            }
        }
    }

    void saveBookingsToFile() {
        ofstream outFile("bookings.txt", ios::app); // append mode
        if (!outFile) {
            cerr << "Error opening bookings.txt for writing.\n";
            return;
        }
        for (const auto& b : bookings) {
            outFile << b.serialize() << endl;
        }
        outFile.close();
    }

    void loadBookingsFromFile() {
        ifstream inFile("bookings.txt");
        if (!inFile) {
            cerr << "Error opening bookings.txt for reading.\n";
            return;
        }

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            try {
                Booking b = Booking::deserialize(line);
                if (b.guestName == username) {
                    bookings.push_back(b);
                }
            } catch (const exception& e) {
                cerr << "Skipping invalid line: " << line << " (" << e.what() << ")\n";
            }
        }

        inFile.close();
    }
};

// Room options display
void showRoomTypes() {
    cout << "\nWe have the following room types available:\n";
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
