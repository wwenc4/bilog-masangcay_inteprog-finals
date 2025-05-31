#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <fstream>
#include <iomanip>
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

void showAvailableRooms() {
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    vector<Booking> bookings;
    ifstream bookingFile("bookings.txt");
    string line;

    while (getline(bookingFile, line)) {
        bookings.push_back(Booking::deserialize(line));
    }

    cout << "\nAvailable Rooms:\n";
    for (const Room& room : rooms) {
        if (room.isAvailable) {
            cout << "Room " << room.roomNumber << " (" << room.roomType << ")\n";
        }
    }

    cout << "----------------------------\n";
    cout << "Occupied Rooms:\n";
    for (const Room& room : rooms) {
        if (!room.isAvailable) {
            string guestName = "Unknown";
            for (const Booking& booking : bookings) {
                if (booking.roomNumber == room.roomNumber) {
                    guestName = booking.guestName;
                    break;
                }
            }
            cout << "Room " << room.roomNumber << " (" << room.roomType << ") - Occupied by " << guestName << "\n";
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

    cout << "\nBooking confirmed!\nReceipt:\nName: " << booking.guestName
         << "\nRoom: " << booking.roomNumber << " (" << selectedRoom->roomType
         << ")\nNights: " << booking.nights << "\nTotal Cost: $" << fixed << setprecision(2) << booking.totalCost << endl;
}

void cancelBooking(const string& guestName){
    vector<Booking> bookings;
    ifstream inFile("bookings.txt");
    string line;
    while (getline(inFile, line)) {
        bookings.push_back(Booking::deserialize(line));
    }

    int roomNum;
    cout << "Enter room number to cancel booking: ";
    cin >> roomNum;

    bool found = false;
    vector<Booking> updatedBookings;
    for (const Booking& b : bookings) {
        if (b.guestName == guestName && b.roomNumber == roomNum) {
            found = true;
        } else {
            updatedBookings.push_back(b);
        }
    }

    if (!found) {
        cout << "Booking not found.\n";
        return;
    }

    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    for (Room& room : rooms){
        if(room.roomNumber == roomNum){
            room.isAvailable = true;
            break;
        }
    }
    saveRoomsToFile(rooms, "rooms.txt");

    ofstream outFile("bookings.txt");
    for (const Booking& b : updatedBookings){
        outFile << b.serialize() << endl;
    }
    cout << "Booking has been cancelled.\n";
}

void showAllBookings(){
    ifstream inFile("bookings.txt");
    string line;
    cout << "\nAll Bookings:\n";
    while (getline(inFile, line)){
        Booking b = Booking::deserialize(line);
        if (b.roomNumber != -1){
            cout << "Name: " << b.guestName
                 << ", Room " << b.roomNumber
                 << ", Nights: " << b.nights
                 << ", Total Cost: $" << fixed << setprecision(2) << b.totalCost << endl;
        }
    }
}

void cancelBookingByAdmin() {
    showAllBookings();

    vector<Booking> bookings;
    ifstream inFile("bookings.txt");
    string line;
    while (getline(inFile, line)) {
        bookings.push_back(Booking::deserialize(line));
    }

    string guestName;
    int roomNumber;
    cout << "Enter guest name: ";
    cin.ignore();
    getline(cin, guestName);
    cout << "Enter room number to cancel: ";
    cin >> roomNumber;

    bool found = false;
    vector<Booking> updatedBookings;
    for (const Booking& b : bookings) {
        if (b.guestName == guestName && b.roomNumber == roomNumber) {
            found = true;
            continue;
        }
        updatedBookings.push_back(b);
    }

    ofstream outFile("bookings.txt");
    for (const Booking& b : updatedBookings) {
        outFile << b.serialize() << endl;
    }

    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    for (Room& r : rooms) {
        if (r.roomNumber == roomNumber) {
            r.isAvailable = true;
            break;
        }
    }
    saveRoomsToFile(rooms, "rooms.txt");

    if (found) cout << "Booking cancelled successfully.\n";
    else cout << "Booking not found.\n";
}

void makeBookingAdmin(){
    cin.ignore();
    string guest;
    cout << "Enter guest name: ";
    getline(cin, guest);
    makeBooking(guest); 
}

void showGuestBookings(const string& guestName) {
    ifstream inFile("bookings.txt");
    if (!inFile) {
        cout << "No bookings found.\n";
        return;
    }

    string line;
    bool found = false;
    cout << "\nYour Bookings:\n";
    while (getline(inFile, line)) {
        Booking b = Booking::deserialize(line);
        if (b.guestName == guestName && b.roomNumber != -1) {
            cout << "Room " << b.roomNumber
                 << ", Nights: " << b.nights
                 << ", Total Cost: $" << fixed << setprecision(2) << b.totalCost << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No bookings found for you.\n";
    }
}

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
            cout << "\nAdmin Menu:\n"
                 << "1. Show Available & Occupied Rooms\n"
                 << "2. Make Booking\n"
                 << "3. Cancel Booking\n"
                 << "4. Show All Bookings\n"
                 << "5. Create Room\n"
                 << "6. Delete Room\n"
                 << "7. Update Room Availability\n"
                 << "8. Logout\nChoice: ";
            cin >> choice;

            switch (choice) {
                case 1: showAvailableRooms(); break;
                case 2: makeBookingAdmin(); break;
                case 3: cancelBookingByAdmin(); break;
                case 4: showAllBookings(); break;
                case 5: createRoom(); break;
                case 6: deleteRoom(); break;
                case 7: updateRoomAvailability(); break;
                case 8: return;
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
            cout << "\nGuest Menu:\n"
                 << "1. Show Available Rooms\n"
                 << "2. Make Booking\n"
                 << "3. Cancel Booking\n"
                 << "4. View My Bookings\n"
                 << "5. Logout\nChoice: ";
            cin >> choice;

            switch (choice) {
                case 1: showAvailableRooms(); break;
                case 2: makeBooking(username); break;
                case 3: cancelBooking(username); break;
                case 4: showGuestBookings(username); break;
                case 5: return;
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
        if ((name == "dane" || name == "raymund") && pass == "A1234!") return new Admin(name);
        else {
            cout << "Invalid admin credentials.\n";
            return nullptr;
        }
    } else if (type == 'g') {
        cout << "Enter your name: ";
        getline(cin, name);
        return new Guest(name);
    }

    cout << "Invalid user type.\n";
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
