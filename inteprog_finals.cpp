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
        } catch (...) {
            cerr << "Error parsing room data: " << line << endl;
            return Room{-1, "Invalid", false};
        }
        return r;
    }
};

// --------- Booking Struct ---------
struct Booking {
    string guestName;
    int roomNumber;
    int nights;
    double totalCost;

    string serialize() const {
        stringstream ss;
        ss << guestName << "," << roomNumber << "," << nights << "," << fixed << setprecision(2) << totalCost;
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
            getline(ss, temp);
            b.totalCost = stod(temp);
        } catch (...) {
            cerr << "Error parsing booking data: " << line << endl;
            return Booking{"Invalid", -1, 0, 0.0};
        }
        return b;
    }
};

// --------- File Functions ---------
vector<Room> loadRoomsFromFile(const string& filename) {
    vector<Room> rooms;
    ifstream inFile(filename);
    string line;
    while (getline(inFile, line)) {
        rooms.push_back(Room::deserialize(line));
    }
    return rooms;
}

void saveRoomsToFile(const vector<Room>& rooms, const string& filename) {
    ofstream outFile(filename);
    for (const Room& room : rooms) {
        outFile << room.serialize() << endl;
    }
}

// --------- Room Management ---------
void showAvailableRooms() {
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    cout << "\nAvailable Rooms:\n";
    for (const Room& room : rooms) {
        if (room.isAvailable) {
            cout << "Room " << room.roomNumber << " (" << room.roomType << ")\n";
        }
    }
}

void createRoom() {
    Room newRoom;
    cout << "Enter room number: ";
    cin >> newRoom.roomNumber;
    cin.ignore();
    cout << "Enter room type (Single/Double/Suite): ";
    getline(cin, newRoom.roomType);
    newRoom.isAvailable = true;

    ofstream outFile("rooms.txt", ios::app);
    if (outFile) {
        outFile << newRoom.serialize() << endl;
        cout << "Room created successfully.\n";
    } else {
        cerr << "Error opening file for writing.\n";
    }
}

void deleteRoom() {
    int roomNum;
    cout << "Enter room number to delete: ";
    cin >> roomNum;

    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    vector<Room> updatedRooms;
    bool found = false;

    for (const Room& room : rooms) {
        if (room.roomNumber != roomNum) {
            updatedRooms.push_back(room);
        } else {
            found = true;
        }
    }

    saveRoomsToFile(updatedRooms, "rooms.txt");

    if (found) {
        cout << "Room deleted successfully.\n";
    } else {
        cout << "Room not found.\n";
    }
}

void updateRoomAvailability() {
    int roomNum;
    cout << "Enter room number to update availability: ";
    cin >> roomNum;

    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    bool found = false;

    for (Room& room : rooms) {
        if (room.roomNumber == roomNum) {
            room.isAvailable = !room.isAvailable;
            cout << "Room is now " << (room.isAvailable ? "Available" : "Unavailable") << ".\n";
            found = true;
            break;
        }
    }

    saveRoomsToFile(rooms, "rooms.txt");

    if (!found) {
        cout << "Room not found.\n";
    }
}

double getRoomRate(const string& type) {
    if (type == "Single") return 100.0;
    if (type == "Double") return 150.0;
    if (type == "Suite") return 250.0;
    return 0.0;
}

void makeBooking(const string& guestName) {
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    showAvailableRooms();

    int roomNum;
    cout << "Enter room number to book: ";
    cin >> roomNum;

    Room* selectedRoom = nullptr;
    for (Room& room : rooms) {
        if (room.roomNumber == roomNum && room.isAvailable) {
            selectedRoom = &room;
            break;
        }
    }

    if (!selectedRoom) {
        cout << "Room not available.\n";
        return;
    }

    int nights;
    cout << "Enter number of nights: ";
    cin >> nights;

    double rate = getRoomRate(selectedRoom->roomType);
    double total = rate * nights;

    Booking booking{guestName, roomNum, nights, total};
    ofstream outFile("bookings.txt", ios::app);
    outFile << booking.serialize() << endl;

    selectedRoom->isAvailable = false;
    saveRoomsToFile(rooms, "rooms.txt");

    cout << "\nBooking confirmed!\n";
    cout << "Receipt:\n";
    cout << "Name: " << booking.guestName << "\nRoom: " << booking.roomNumber
         << " (" << selectedRoom->roomType << ")\nNights: " << booking.nights
         << "\nTotal Cost: $" << fixed << setprecision(2) << booking.totalCost << endl;
}

// --------- User System ---------
class User {
protected:
    string username;
public:
    User(const string& uname) : username(uname) {}
    virtual ~User() {}
    virtual void showMenu() = 0;
    virtual void describeRole() = 0;
};

class Admin : public User {
public:
    Admin(const string& uname) : User(uname) {}

    void showMenu() override {
        int choice;
        do {
            cout << "\nAdmin Menu:\n1. Show Available Rooms\n2. Create Room\n3. Delete Room\n4. Update Availability\n5. Logout\nChoice: ";
            cin >> choice;

            switch (choice) {
                case 1: showAvailableRooms(); break;
                case 2: createRoom(); break;
                case 3: deleteRoom(); break;
                case 4: updateRoomAvailability(); break;
                case 5: return;
                default: cout << "Invalid choice.\n";
            }
        } while (true);
    }

    void describeRole() override {
        cout << username << " is an Admin with full access.\n";
    }
};

class Guest : public User {
public:
    Guest(const string& uname) : User(uname) {}

    void showMenu() override {
        int choice;
        do {
            cout << "\nGuest Menu:\n1. Show Available Rooms\n2. Make Booking\n3. Logout\nChoice: ";
            cin >> choice;

            switch (choice) {
                case 1: showAvailableRooms(); break;
                case 2: makeBooking(username); break;
                case 3: return;
                default: cout << "Invalid choice.\n";
            }
        } while (true);
    }

    void describeRole() override {
        cout << username << " is a Guest.\n";
    }
};

User* login() {
    string name, pass;
    char type;
    cout << "\nLogin as Admin or Guest? >> [a/g]: ";
    cin >> type;
    cin.ignore();

    if (type == 'a') {
        cout << "Admin username: ";
        getline(cin, name);
        cout << "Password: ";
        getline(cin, pass);
        if (name == "dane" || "raymund" && pass == "A1234!") return new Admin(name);
        cout << "Invalid admin credentials.\n";
        return nullptr;
    } else if (type == 'g') {
        cout << "Enter your name: ";
        getline(cin, name);
        return new Guest(name);
    }

    cout << "Invalid user type.\n";
    return nullptr;
}

// --------- Main ---------
int main() {
    cout << "Welcome to the Hotel Management System\n";

    User* user = nullptr;
    while (!user) user = login();

    user->describeRole();
    user->showMenu();

    delete user;
    cout << "Goodbye!\n";
    return 0;
}
