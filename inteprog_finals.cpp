#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

// ----------------- Models -----------------
struct Room {
    int roomNumber;
    string roomType;
    bool isAvailable;

    string serialize() const {
        return to_string(roomNumber) + "," + roomType + "," + (isAvailable ? "1" : "0");
    }

    static Room deserialize(const string& line) {
        Room r;
        stringstream ss(line);
        string token;
        getline(ss, token, ',');
        r.roomNumber = stoi(token);
        getline(ss, r.roomType, ',');
        getline(ss, token, ',');
        r.isAvailable = (token == "1");
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
        ss << guestName << "," << roomNumber << "," << nights << ","
           << fixed << setprecision(2) << totalCost;
        return ss.str();
    }

    static Booking deserialize(const string& line) {
        Booking b;
        stringstream ss(line);
        string token;
        getline(ss, b.guestName, ',');
        getline(ss, token, ','); b.roomNumber = stoi(token);
        getline(ss, token, ','); b.nights = stoi(token);
        getline(ss, token); b.totalCost = stod(token);
        return b;
    }
};

// ----------------- File I/O -----------------
vector<Room> loadRooms() {
    vector<Room> rooms;
    ifstream file("rooms.txt");
    string line;
    while (getline(file, line)) {
        if (!line.empty()) rooms.push_back(Room::deserialize(line));
    }
    return rooms;
}

void saveRooms(const vector<Room>& rooms) {
    ofstream file("rooms.txt");
    for (const auto& r : rooms) file << r.serialize() << "\n";
}

vector<Booking> loadBookings() {
    vector<Booking> bookings;
    ifstream file("bookings.txt");
    string line;
    while (getline(file, line)) {
        if (!line.empty()) bookings.push_back(Booking::deserialize(line));
    }
    return bookings;
}

void saveBookings(const vector<Booking>& bookings) {
    ofstream file("bookings.txt");
    for (const auto& b : bookings) file << b.serialize() << "\n";
}

// ----------------- Utility -----------------
double getRoomRate(const string& type) {
    if (type == "Single") return 100.0;
    if (type == "Double") return 150.0;
    if (type == "Suite") return 250.0;
    return 0.0;
}

// ----------------- User Classes -----------------
class User {
protected:
    string username;
    string role;
public:
    User(string uname, string r) : username(uname), role(r) {}
    virtual ~User() {}
    virtual void showMenu() = 0;
    virtual void describeRole() = 0;
    string getUsername() const { return username; }
    string getRole() const { return role; }
};

// ----------------- Guest Features -----------------
void showAvailableRooms(User* user) {
    auto rooms = loadRooms();
    auto bookings = loadBookings();

    cout << "\nAvailable Rooms:\n";
    for (const auto& room : rooms) {
        if (room.isAvailable) {
            cout << "Room " << room.roomNumber << " (" << room.roomType << ")\n";
        }
    }

    cout << "---\nOccupied Rooms:\n";
    for (const auto& b : bookings) {
        for (const auto& r : rooms) {
            if (b.roomNumber == r.roomNumber) {
                cout << "Room " << r.roomNumber << " (" << r.roomType << ") - ";
                if (user->getRole() == "admin" || user->getUsername() == b.guestName)
                    cout << "Occupied by " << b.guestName;
                else
                    cout << "Occupied";
                cout << "\n";
                break;
            }
        }
    }
}

void makeBooking(User* user) {
    auto rooms = loadRooms();
    auto bookings = loadBookings();

    int roomNum, nights;
    cout << "Enter room number to book: ";
    cin >> roomNum;
    cout << "Enter number of nights: ";
    cin >> nights;

    for (auto& room : rooms) {
        if (room.roomNumber == roomNum && room.isAvailable) {
            double rate = getRoomRate(room.roomType);
            double total = rate * nights;
            bookings.push_back({user->getUsername(), roomNum, nights, total});
            room.isAvailable = false;
            saveRooms(rooms);
            saveBookings(bookings);
            cout << "Booking successful.\n";
            return;
        }
    }
    cout << "Room not available.\n";
}

void cancelBooking(User* user) {
    auto rooms = loadRooms();
    auto bookings = loadBookings();
    int roomNum;
    cout << "Enter room number to cancel: ";
    cin >> roomNum;

    auto it = remove_if(bookings.begin(), bookings.end(), [&](const Booking& b) {
        return b.roomNumber == roomNum && b.guestName == user->getUsername();
    });

    if (it != bookings.end()) {
        bookings.erase(it, bookings.end());
        for (auto& r : rooms) if (r.roomNumber == roomNum) r.isAvailable = true;
        saveRooms(rooms);
        saveBookings(bookings);
        cout << "Booking cancelled.\n";
    } else {
        cout << "No booking found.\n";
    }
}

void showGuestBookings(User* user) {
    auto bookings = loadBookings();
    for (const auto& b : bookings) {
        if (b.guestName == user->getUsername()) {
            cout << "Room " << b.roomNumber << ", Nights: " << b.nights
                 << ", Total: $" << fixed << setprecision(2) << b.totalCost << "\n";
        }
    }
}

// ----------------- Admin Features -----------------
void createRoom() {
    auto rooms = loadRooms();

    // Show existing rooms
    cout << "\n--- Existing Rooms ---\n";
    if (rooms.empty()) {
        cout << "No rooms currently available.\n";
    } else {
        for (const auto& r : rooms) {
            cout << "Room " << r.roomNumber << " (" << r.roomType << ") - "
                 << (r.isAvailable ? "Available" : "Booked") << "\n";
        }
    }

    cout << "\n--- Add Room ---\n";
    Room r;
    cout << "Enter room number (0 to cancel): ";
    cin >> r.roomNumber;

    if (r.roomNumber == 0) {
        cout << "Operation cancelled.\n";
        return;
    }

    // Check if room number already exists
    for (const auto& existing : rooms) {
        if (existing.roomNumber == r.roomNumber) {
            cout << "Room number already exists. Operation cancelled.\n";
            return;
        }
    }

    cout << "Enter room type (Single/Double/Suite): ";
    cin >> r.roomType;
    r.isAvailable = true;
    rooms.push_back(r);
    saveRooms(rooms);
    cout << "Room added.\n";
}


void deleteRoom() {
    auto rooms = loadRooms();

    if (rooms.empty()) {
        cout << "No rooms to delete.\n";
        return;
    }

    cout << "\n--- Current Rooms ---\n";
    for (const auto& r : rooms) {
        cout << "Room " << r.roomNumber << " (" << r.roomType << ") - "
             << (r.isAvailable ? "Available" : "Booked") << "\n";
    }

    int roomNum;
    cout << "\nEnter room number to delete (0 to cancel): ";
    cin >> roomNum;

    if (roomNum == 0) {
        cout << "Deletion cancelled.\n";
        return;
    }

    auto it = remove_if(rooms.begin(), rooms.end(), [&](const Room& r) {
        return r.roomNumber == roomNum;
    });

    if (it != rooms.end()) {
        rooms.erase(it, rooms.end());
        saveRooms(rooms);
        cout << "Room " << roomNum << " deleted.\n";
    } else {
        cout << "Room number not found.\n";
    }
}

void updateAvailability() {
    vector<Room> rooms = loadRooms();
    if (rooms.empty()) {
        cout << "No rooms available to update.\n";
        return;
    }

    cout << "\n--- Room Availability ---\n";
    for (const auto& r : rooms) {
        cout << "Room Number: " << r.roomNumber
             << ", Type: " << r.roomType
             << ", Status: " << (r.isAvailable ? "Available" : "Booked") << endl;
    }

    int roomNum;
    cout << "Enter the room number to toggle availability (0 to cancel): ";
    cin >> roomNum;

    if (roomNum == 0) {
        cout << "Returning to admin menu.\n";
        return;
    }

    bool found = false;
    for (auto& r : rooms) {
        if (r.roomNumber == roomNum) {
            r.isAvailable = !r.isAvailable;
            cout << "Room " << r.roomNumber << " availability updated to: "
                 << (r.isAvailable ? "Available" : "Booked") << endl;
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Room number not found.\n";
        return;
    }

    saveRooms(rooms);
}

void cancelBookingByAdmin() {
    auto rooms = loadRooms();
    auto bookings = loadBookings();

    if (bookings.empty()) {
        cout << "No bookings to cancel.\n";
        return;
    }

    cout << "\n--- Currently Booked Rooms ---\n";
    for (const auto& b : bookings) {
        cout << "Room " << b.roomNumber << " - Booked by " << b.guestName << "\n";
    }

    int roomNum;
    cout << "Enter room number to cancel booking (0 to return): ";
    cin >> roomNum;

    if (roomNum == 0) {
        cout << "Returning to admin menu.\n";
        return;
    }

    bool found = false;

    // Remove the booking and mark room as available
    bookings.erase(remove_if(bookings.begin(), bookings.end(), [&](const Booking& b) {
        if (b.roomNumber == roomNum) {
            found = true;
            return true;
        }
        return false;
    }), bookings.end());

    if (found) {
        for (auto& r : rooms)
            if (r.roomNumber == roomNum)
                r.isAvailable = true;

        saveRooms(rooms);
        saveBookings(bookings);
        cout << "Booking for room " << roomNum << " cancelled successfully.\n";
    } else {
        cout << "No booking found for room " << roomNum << ".\n";
    }
}

void showAllBookings() {
    auto bookings = loadBookings();
    for (const auto& b : bookings) {
        cout << "Guest: " << b.guestName << ", Room: " << b.roomNumber
             << ", Nights: " << b.nights << ", Total: $" << fixed << setprecision(2) << b.totalCost << "\n";
    }
}

// ----------------- Admin & Guest -----------------
class Admin : public User {
public:
    Admin(string uname) : User(uname, "admin") {}
    void describeRole() override { cout << "Admin: " << username << "\n"; }
    void showMenu() override {
        int choice;
        do {
            cout << "\nADMIN MENU\n1. View Rooms\n2. Cancel Booking\n3. View Bookings\n4. Add Room\n5. Delete Room\n6. Update Availability\n7. Logout\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: showAvailableRooms(this); break;
                case 2: cancelBookingByAdmin(); break;
                case 3: showAllBookings(); break;
                case 4: createRoom(); break;
                case 5: deleteRoom(); break;
                case 6: updateAvailability(); break;
                case 7: return;
                default: cout << "Invalid. Try again.\n";
            }
        } while (true);
    }
};

class Guest : public User {
public:
    Guest(string uname) : User(uname, "guest") {}
    void describeRole() override { cout << "Guest: " << username << "\n"; }
    void showMenu() override {
        int choice;
        do {
            cout << "\nGUEST MENU\n1. View Rooms\n2. Book Room\n3. Cancel Booking\n4. My Bookings\n5. Logout\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: showAvailableRooms(this); break;
                case 2: makeBooking(this); break;
                case 3: cancelBooking(this); break;
                case 4: showGuestBookings(this); break;
                case 5: return;
                default: cout << "Invalid. Try again.\n";
            }
        } while (true);
    }
};

// ----------------- Main -----------------
User* login() {
    int type;
    string uname, pass;
    cout << "Login as: 1. Admin  2. Guest\nChoice: ";
    cin >> type;
    cin.ignore();

    if (type == 1) {
        cout << "Username: "; getline(cin, uname);
        cout << "Password: "; getline(cin, pass);
        if (uname == "admin" && pass == "123") return new Admin(uname);
        else { cout << "Invalid credentials\n"; return nullptr; }
    } else {
        cout << "Enter name: "; getline(cin, uname);
        return uname.empty() ? nullptr : new Guest(uname);
    }
}

int main() {
    cout << "HOTEL MANAGEMENT SYSTEM\n";
    while (true) {
        User* user = nullptr;
        while (!user) user = login();
        user->describeRole();
        user->showMenu();
        delete user;
    }
    return 0;
}
