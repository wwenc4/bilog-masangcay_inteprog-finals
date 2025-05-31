#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <fstream>
#include <iomanip>
#include <algorithm>
using namespace std;

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
        } catch (...) {
            cerr << "Error parsing room data: " << line << endl;
            return Room{-1, "Invalid", false};
        }
        return r;
    }
};

struct Booking {
    string guestName;
    int roomNumber;
    int nights;
    double totalCost;
    int referenceNumber;
    bool confirmed = false;

    string serialize() const {
        stringstream ss;
        ss << guestName << "," << roomNumber << "," << nights << ","
           << fixed << setprecision(2) << totalCost << "," << referenceNumber << "," << confirmed;
        return ss.str();
    }

    static Booking deserialize(const string& line) {
        Booking b;
        stringstream ss(line);
        string temp;
        try {
            getline(ss, b.guestName, ',');
            getline(ss, temp, ',');
            b.roomNumber = stoi(temp);
            getline(ss, temp, ',');
            b.nights = stoi(temp);
            getline(ss, temp, ',');
            b.totalCost = stod(temp);
            getline(ss, temp, ',');
            b.referenceNumber = stoi(temp);
            getline(ss, temp);
            b.confirmed = stoi(temp);
        } catch (...) {
            cerr << "Error parsing booking data: " << line << endl;
            return Booking{"Invalid", -1, 0, 0.0, -1, false};
        }
        return b;
    }
};

string toLower(const string& s) {
    string lower = s;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

int typeOrder(const string& type) {
    string t = toLower(type);
    if (t == "single") return 1;
    if (t == "double") return 2;
    if (t == "suite") return 3;
    return 4;
}

vector<Room> loadRoomsFromFile(const string& filename) {
    vector<Room> rooms;
    ifstream inFile(filename);
    string line;
    while (getline(inFile, line)) {
        Room r = Room::deserialize(line);
        if (r.roomNumber != -1) rooms.push_back(r);
    }
    return rooms;
}

void saveRoomsToFile(const vector<Room>& rooms, const string& filename) {
    vector<Room> sortedRooms = rooms;

    sort(sortedRooms.begin(), sortedRooms.end(), [](const Room& a, const Room& b) {
        int ta = typeOrder(a.roomType);
        int tb = typeOrder(b.roomType);
        if (ta == tb) return a.roomNumber < b.roomNumber;
        return ta < tb;
    });

    ofstream outFile(filename);
    for (const Room& room : sortedRooms) {
        outFile << room.serialize() << endl;
    }
}

vector<Booking> loadBookingsFromFile(const string& filename) {
    vector<Booking> bookings;
    ifstream inFile(filename);
    string line;
    while (getline(inFile, line)) {
        Booking b = Booking::deserialize(line);
        if (b.roomNumber != -1) bookings.push_back(b);
    }
    return bookings;
}

void saveBookingsToFile(const vector<Booking>& bookings, const string& filename) {
    ofstream outFile(filename);
    for (const Booking& b : bookings) {
        outFile << b.serialize() << endl;
    }
}

int generateReferenceNumber(const vector<Booking>& bookings) {
    int maxRef = 0;
    for (const Booking& b : bookings) {
        if (b.referenceNumber > maxRef) maxRef = b.referenceNumber;
    }
    return maxRef + 1;
}

void showAvailableRooms() {
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    cout << "\nAvailable Rooms:\n";
    for (const Room& room : rooms) {
        if (room.isAvailable) {
            cout << "Room Number: " << room.roomNumber << ", Type: " << room.roomType << endl;
        }
    }
    cout << "\nOccupied Rooms (type only for privacy):\n";
    for (const Room& room : rooms) {
        if (!room.isAvailable) {
            cout << "Room Type: " << room.roomType << endl;
        }
    }
    cout << endl;
}

void viewAllRooms() {
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    vector<Booking> bookings = loadBookingsFromFile("bookings.txt");
    cout << "\nAll Rooms (Admin View):\n";
    cout << "Room Number | Room Type | Availability | Occupied By\n";
    cout << "------------|-----------|--------------|-------------\n";
    for (const Room& r : rooms) {
        string occupant = "-";
        if (!r.isAvailable) {
            for (const Booking& b : bookings) {
                if (b.roomNumber == r.roomNumber && b.confirmed) {
                    occupant = b.guestName;
                    break;
                }
            }
        }
        cout << setw(12) << r.roomNumber << " | "
             << setw(9) << r.roomType << " | "
             << setw(12) << (r.isAvailable ? "Available" : "Occupied") << " | "
             << occupant << "\n";
    }
}

void createRoom() {
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    Room newRoom;
    cout << "Enter new room number: ";
    cin >> newRoom.roomNumber;
    cin.ignore();

    for (const Room& r : rooms) {
        if (r.roomNumber == newRoom.roomNumber) {
            cout << "Room already exists.\n";
            return;
        }
    }

    cout << "Enter room type (Single/Double/Suite): ";
    getline(cin, newRoom.roomType);
    newRoom.isAvailable = true;

    rooms.push_back(newRoom);
    saveRoomsToFile(rooms, "rooms.txt");
    cout << "Room created successfully.\n";
}

void updateRoom() {
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    int roomNum;
    cout << "Enter room number to update: ";
    cin >> roomNum;
    cin.ignore();

    for (Room& r : rooms) {
        if (r.roomNumber == roomNum) {
            cout << "Current type: " << r.roomType << ", Availability: " << (r.isAvailable ? "Yes" : "No") << "\n";
            cout << "Enter new room type: ";
            getline(cin, r.roomType);
            cout << "Is the room available? (1 for Yes, 0 for No): ";
            cin >> r.isAvailable;
            saveRoomsToFile(rooms, "rooms.txt");
            cout << "Room updated successfully.\n";
            return;
        }
    }
    cout << "Room not found.\n";
}

void deleteRoom() {
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    int roomNum;
    cout << "Enter room number to delete: ";
    cin >> roomNum;

    auto it = remove_if(rooms.begin(), rooms.end(), [&](Room r) {
        return r.roomNumber == roomNum;
    });

    if (it != rooms.end()) {
        rooms.erase(it, rooms.end());
        saveRoomsToFile(rooms, "rooms.txt");
        cout << "Room deleted.\n";
    } else {
        cout << "Room not found.\n";
    }
}

void viewAllBookings() {
    vector<Booking> bookings = loadBookingsFromFile("bookings.txt");
    cout << "\nAll Bookings:\n";
    for (const Booking& b : bookings) {
        cout << "Guest: " << b.guestName
             << ", Room: " << b.roomNumber
             << ", Nights: " << b.nights
             << ", Total: $" << b.totalCost
             << ", Ref#: " << b.referenceNumber
             << ", Confirmed: " << (b.confirmed ? "Yes" : "No") << "\n";
    }
}

void cancelAnyBooking() {
    vector<Booking> bookings = loadBookingsFromFile("bookings.txt");
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");

    int ref;
    cout << "Enter booking reference number to cancel: ";
    cin >> ref;

    auto it = find_if(bookings.begin(), bookings.end(), [=](Booking b) {
        return b.referenceNumber == ref;
    });

    if (it != bookings.end()) {
        for (Room& r : rooms) {
            if (r.roomNumber == it->roomNumber) {
                r.isAvailable = true;
                break;
            }
        }
        bookings.erase(it);
        saveBookingsToFile(bookings, "bookings.txt");
        saveRoomsToFile(rooms, "rooms.txt");
        cout << "Booking cancelled.\n";
    } else {
        cout << "Booking not found.\n";
    }
}

void showRoomTypes();
void makeBooking(const string& guestName);
void confirmBooking(const string& guestName);
void cancelBooking(const string& guestName);
void showGuestBookings(const string& guestName);

class User {
protected:
    string username;
public:
    User(const string& uname) : username(uname) {}
    virtual ~User() {}
    virtual void showMenu() = 0;
    virtual void describeRole() = 0;
};

class Guest : public User {
public:
    Guest(const string& uname) : User(uname) {}

    void showMenu() override;
    void describeRole() override {
        cout << username << " is a Guest.\n";
    }
};

class Admin : public User {
public:
    Admin(const string& uname) : User(uname) {}

    void showMenu() override;

    void describeRole() override {
        cout << username << " is an Admin.\n";
    }
};

void Guest::showMenu() {
    int choice;
    do {
        cout << "\n========= Guest Menu =========\n";
        cout << " 1. 🏨 Show Available Rooms\n";
        cout << " 2. 🛏️  Show Room Types & Rates\n";
        cout << " 3. ✍️  Make a Booking\n";
        cout << " 4. ❌ Cancel a Booking\n";
        cout << " 5. 📋 View My Bookings\n";
        cout << " 6. ✅ Confirm My Booking\n";
        cout << " 7. 🔒 Logout\n";
        cout << "==============================\n";
        cout << "Enter your choice (1-7): ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 7.\n";
            continue;
        }

        switch (choice) {
            case 1: showAvailableRooms(); break;
            case 2: showRoomTypes(); break;
            case 3: makeBooking(username); break;
            case 4: cancelBooking(username); break;
            case 5: showGuestBookings(username); break;
            case 6: confirmBooking(username); break;
            case 7: return;
            default: cout << "Invalid choice. Please enter a number between 1 and 7.\n";
        }
    } while (true);
}

void Admin::showMenu() {
    int choice;
    do {
        cout << "\n========= Admin Menu =========\n";
        cout << " 1. View All Rooms\n";
        cout << " 2. Create New Room\n";
        cout << " 3. Update Room\n";
        cout << " 4. Delete Room\n";
        cout << " 5. View All Bookings\n";
        cout << " 6. Cancel Any Booking\n";
        cout << " 7. Logout\n";
        cout << "==============================\n";
        cout << "Enter your choice (1-7): ";
        cin >> choice;

        switch (choice) {
            case 1: viewAllRooms(); break;
            case 2: createRoom(); break;
            case 3: updateRoom(); break;
            case 4: deleteRoom(); break;
            case 5: viewAllBookings(); break;
            case 6: cancelAnyBooking(); break;
            case 7: return;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (true);
}

User* login() {
    string name, pass;
    char type;
    cout << "\nLogin as Admin or Guest? >> [a/g]: ";
    cin >> type;
    cin.ignore();

    if (type == 'g') {
        cout << "Enter your name: ";
        getline(cin, name);
        return new Guest(name);
    } else if (type == 'a') {
        cout << "Enter admin username: ";
        getline(cin, name);
        cout << "Enter admin password: ";
        getline(cin, pass);
        if (name == "admin" && pass == "admin123") {
            return new Admin(name);
        } else {
            cout << "Invalid admin credentials.\n";
            return nullptr;
        }
    }

    cout << "Invalid or unsupported user type.\n";
    return nullptr;
}

int main() {
    cout << "Welcome to the Hotel Management System\n";

    while (true) {
        User* user = nullptr;
        while (!user) {
            user = login();
        }

        user->describeRole();
        user->showMenu();

        delete user;
        cout << "\nLogging out...\nReturning to login screen.\n";
    }

    return 0;
}