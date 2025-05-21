#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <fstream>
#include <iomanip>
using namespace std;

// --------- Room Struct ---------
struct Room {
    int roomNumber;
    string roomType;
    bool isAvailable;

    string serialize() const {
        stringstream ss;
        ss << roomNumber << "," << roomType << "," << isAvailable;
        return ss.str();
    }

    static Room deserialize(const string& line) {
        Room r;
        stringstream ss(line);
        string temp;
        try {
            getline(ss, temp, ',');
            r.roomNumber = stoi(temp);
            getline(ss, r.roomType, ',');
            getline(ss, temp);
            r.isAvailable = stoi(temp);
        } catch (const exception& e) {
            cerr << "Error parsing room data: " << line << endl;
            return Room{-1, "Invalid", false};
        }
        return r;
    }
};

// --------- Function Declarations (needed early) ---------
vector<Room> loadRoomsFromFile(const string& filename);
void createRoom();
void deleteRoom();
void updateRoomAvailability();
void showAvailableRooms();

// --------- General Room Type Descriptions ---------
void showRoomTypeGeneral() {
    cout << "\nWe have the following room types available:\n";
    cout << "1. Single Room - $100 per night\n";
    cout << "2. Double Room - $150 per night\n";
    cout << "3. Suite - $250 per night\n";
}

// --------- Booking Struct ---------
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
        try {
            getline(ss, b.guestName, ',');
            getline(ss, b.roomType, ',');
            getline(ss, temp, ',');
            if (temp.empty()) throw invalid_argument("Invalid nights value");
            b.nights = stoi(temp);
            getline(ss, temp);
            b.totalCost = stod(temp);
        } catch (const exception& e) {
            cerr << "Error parsing booking data: " << line << endl;
            return Booking{"Invalid", "Invalid", 0, 0.0};
        }
        return b;
    }
};

// --------- Base User Class ---------
class User {
protected:
    string username;
public:
    User(const string& uname) : username(uname) {}
    virtual ~User() {}

    string getUsername() const { return username; }
    virtual void showMenu() = 0;
    virtual void describeRole() = 0;

    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
};

// --------- Admin Class ---------
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
            cout << "1. View available rooms\n";
            cout << "2. Create a room\n";
            cout << "3. Delete a room\n";
            cout << "4. Update room availability\n";
            cout << "5. Logout\n";

            while (!(cin >> choice)) {
                cout << "Invalid input. Please enter a number: ";
                clearInputBuffer();
            }

            switch (choice) {
                case 1:
                    showAvailableRooms();
                    break;
                case 2:
                    createRoom();
                    break;
                case 3:
                    deleteRoom();
                    break;
                case 4:
                    updateRoomAvailability();
                    break;
                case 5:
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

// --------- Guest Class ---------
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
            cout << "1. View room types\n2. Make a booking\n3. View My Reservations\n4. View Available Rooms\n5. Logout\n";
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
                    showAvailableRooms();
                    break;
                case 5:
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
        showRoomTypeGeneral();
        int subChoice;
        cout << "\n1. Book a room\n2. Return to menu\n";
        cout << "Enter choice: ";

        while (!(cin >> subChoice) || (subChoice != 1 && subChoice != 2)) {
            cout << "Invalid input. Please enter 1 or 2: ";
            clearInputBuffer();
        }

        if (subChoice == 1)
            makeAndStoreBooking();
    }

    void makeAndStoreBooking() {
        showAvailableRooms();

        int roomNum;
        cout << "\nEnter the room number you wish to book: ";
        while (!(cin >> roomNum)) {
            cout << "Invalid input. Please enter a room number: ";
            clearInputBuffer();
        }

        vector<Room> rooms = loadRoomsFromFile("rooms.txt");
        bool found = false;
        Room selectedRoom;

        for (auto& room : rooms) {
            if (room.roomNumber == roomNum && room.isAvailable) {
                selectedRoom = room;
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Room not available or doesn't exist. Please try again.\n";
            return;
        }

        int nights;
        cout << "Enter number of nights: ";
        while (!(cin >> nights) || nights <= 0) {
            cout << "Please enter a valid number of nights: ";
            clearInputBuffer();
        }

        double rate;
        if (selectedRoom.roomType == "Single Room") rate = 100.0;
        else if (selectedRoom.roomType == "Double Room") rate = 150.0;
        else if (selectedRoom.roomType == "Suite") rate = 250.0;
        else rate = 120.0;

        double totalCost = rate * nights;
        Booking b{username, selectedRoom.roomType, nights, totalCost};
        bookings.push_back(b);

        cout << "Booking confirmed for Room #" << selectedRoom.roomNumber << " (" << selectedRoom.roomType << ") for "
             << nights << " nights. Total: $" << fixed << setprecision(2) << totalCost << "\n";

        for (auto& room : rooms) {
            if (room.roomNumber == selectedRoom.roomNumber) {
                room.isAvailable = false;
                break;
            }
        }

        ofstream outFile("rooms.txt");
        if (!outFile) {
            cerr << "Error: Could not open rooms.txt for writing.\n";
            return;
        }
        for (const auto& room : rooms) {
            outFile << room.serialize() << endl;
        }
        outFile.close();

        saveBookingsToFile();
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
        ofstream outFile("bookings.txt"); // Overwrite mode to avoid duplicates
        if (!outFile) {
            cerr << "Error: Could not open bookings.txt for writing.\n";
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
            Booking b = Booking::deserialize(line);
            if (b.guestName == username)
                bookings.push_back(b);
        }
        inFile.close();
    }
};

// --------- Room Functions ---------
vector<Room> loadRoomsFromFile(const string& filename) {
    vector<Room> rooms;
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error: Could not open " << filename << " for reading.\n";
        return rooms;
    }
    string line;
    while (getline(inFile, line)) {
        if (!line.empty()) {
            Room r = Room::deserialize(line);
            if (r.roomNumber != -1) { // Skip invalid rooms
                rooms.push_back(r);
            }
        }
    }
    inFile.close();
    return rooms;
}

void showAvailableRooms() {
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    cout << "\nList of Available Rooms:\n";
    cout << left << setw(15) << "Room Number" << setw(20) << "Room Type" << endl;
    cout << "-----------------------------------\n";
    bool hasAvailable = false;
    for (const auto& room : rooms) {
        if (room.isAvailable) {
            cout << left << setw(15) << room.roomNumber << setw(20) << room.roomType << endl;
            hasAvailable = true;
        }
    }
    if (!hasAvailable) {
        cout << "No rooms available.\n";
    }
}

void createRoom() {
    Room r;
    cout << "Enter Room Number: ";
    while (!(cin >> r.roomNumber)) {
        cout << "Invalid input. Please enter a valid room number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore();
    cout << "Enter Room Type (Single Room, Double Room, Suite): ";
    getline(cin, r.roomType);
    r.isAvailable = true;

    if (r.roomType != "Single Room" && r.roomType != "Double Room" && r.roomType != "Suite") {
        cout << "Invalid room type. Room creation cancelled.\n";
        return;
    }

    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    for (const auto& room : rooms) {
        if (room.roomNumber == r.roomNumber) {
            cout << "Room number " << r.roomNumber << " already exists. Room creation cancelled.\n";
            return;
        }
    }

    ofstream outFile("rooms.txt", ios::app);
    if (!outFile) {
        cerr << "Error: Could not open rooms.txt for writing.\n";
        return;
    }
    outFile << r.serialize() << endl;
    outFile.close();
    cout << "Room created and added successfully.\n";
}

void deleteRoom() {
    int roomNumber;
    cout << "Enter Room Number to delete: ";
    while (!(cin >> roomNumber)) {
        cout << "Invalid input. Please enter a valid room number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    bool found = false;
    for (const auto& room : rooms) {
        if (room.roomNumber == roomNumber) {
            found = true;
            cout << "Found room #" << room.roomNumber << " (" << room.roomType << "). Confirm deletion? (y/n): ";
            char confirm;
            cin >> confirm;
            if (confirm != 'y' && confirm != 'Y') {
                cout << "Room deletion cancelled.\n";
                return;
            }
            break;
        }
    }

    if (!found) {
        cout << "Room number " << roomNumber << " not found.\n";
        return;
    }

    ofstream outFile("rooms.txt");
    if (!outFile) {
        cerr << "Error: Could not open rooms.txt for writing.\n";
        return;
    }
    for (const auto& room : rooms) {
        if (room.roomNumber != roomNumber) {
            outFile << room.serialize() << endl;
        }
    }
    outFile.close();
    cout << "Room deleted successfully.\n";
}

void updateRoomAvailability() {
    int roomNumber;
    bool newAvailability;

    cout << "Enter Room Number to update: ";
    while (!(cin >> roomNumber)) {
        cout << "Invalid input. Please enter a valid room number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << "Enter new availability (1 = Available, 0 = Unavailable): ";
    while (!(cin >> newAvailability)) {
        cout << "Invalid input. Please enter 1 or 0: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    bool found = false;
    for (auto& room : rooms) {
        if (room.roomNumber == roomNumber) {
            room.isAvailable = newAvailability;
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Room number " << roomNumber << " not found.\n";
        return;
    }

    ofstream outFile("rooms.txt");
    if (!outFile) {
        cerr << "Error: Could not open rooms.txt for writing.\n";
        return;
    }
    for (const auto& room : rooms) {
        outFile << room.serialize() << endl;
    }
    outFile.close();
    cout << "Room availability updated.\n";
}

// --------- Main Function ---------
void Banner() {
    cout << "*************************************************\n";
}

int main() {
    string admins[] = {"raymund", "dane"};
    string password = "A1234!"; // TODO: Store admin credentials securely in a file or database
    int position;
    string adminName, adminPassword;

    cout << "\nWelcome to the Hotel Management System\n";
    Banner();
    cout << "1. Admin\n2. Guest\n3. Exit\n";
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