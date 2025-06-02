#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <stdexcept>

// Using standard namespace for convenience
using namespace std;

// ----------------- Models -----------------

// Represents a hotel room with number, type, price, and availability
struct Room {
    int roomNumber;         // Unique identifier for the room
    string roomType;       // Type of room (e.g., Single, Double, Suite)
    double price;          // Price per night
    bool isAvailable;      // Availability status

    // Serializes room data to a comma-separated string for file storage
    string serialize() const {
        stringstream ss;
        ss << roomNumber << "," << roomType << "," << fixed << setprecision(2) << price << "," << (isAvailable ? "1" : "0");
        return ss.str();
    }

    // Deserializes a comma-separated string into a Room object
    static Room deserialize(const string& line) {
        Room r;
        stringstream ss(line);
        string token;

        getline(ss, token, ','); r.roomNumber = stoi(token);
        getline(ss, r.roomType, ',');
        getline(ss, token, ','); r.price = stod(token);
        getline(ss, token, ','); r.isAvailable = (token == "1");

        return r;
    }
};

// Represents a booking with guest details, room, duration, cost, and reference ID
struct Booking {
    string guestName;      // Name of the guest making the booking
    int roomNumber;       // Room number booked
    int nights;           // Number of nights for the stay
    double totalCost;     // Total cost of the booking
    string referenceID;   // Unique reference ID for confirmed bookings

    // Serializes booking data to a comma-separated string for file storage
    string serialize() const {
        stringstream ss;
        ss << guestName << "," << roomNumber << "," << nights << "," << fixed << setprecision(2) << totalCost << "," << referenceID;
        return ss.str();
    }

    // Deserializes a comma-separated string into a Booking object
    static Booking deserialize(const string& line) {
        Booking b;
        try {
            stringstream ss(line);
            string token;

            getline(ss, b.guestName, ',');
            getline(ss, token, ','); b.roomNumber = stoi(token);
            getline(ss, token, ','); b.nights = stoi(token);
            getline(ss, token, ','); b.totalCost = stod(token);
            getline(ss, b.referenceID, ',');
        } catch (const exception& e) {
            throw runtime_error("Booking deserialization failed: " + string(e.what()));
        }
        return b;
    }
};

// Generates a unique reference ID for confirmed bookings
string generateReferenceID() {
    int refNum = 1000; // Starting reference number
    ifstream in("ref_counter.txt");
    if (in >> refNum) refNum++; // Increment if file exists and contains a number
    in.close();

    ofstream out("ref_counter.txt");
    out << refNum; // Save updated number
    out.close();

    return "REF" + to_string(refNum); // Return formatted reference ID
}

// ----------------- File I/O -----------------

// Loads all rooms from rooms.txt file
vector<Room> loadRooms() {
    vector<Room> rooms;
    try {
        ifstream file("rooms.txt");
        if (!file.is_open()) throw runtime_error("Unable to open rooms.txt");

        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                try {
                    rooms.push_back(Room::deserialize(line));
                } catch (const exception& e) {
                    cerr << "Error parsing room data: " << e.what() << " (line: " << line << ")\n";
                }
            }
        }
    } catch (const exception& e) {
        cerr << "Exception in loadRooms(): " << e.what() << "\n";
    }
    return rooms;
}

// Saves all rooms to rooms.txt file
void saveRooms(const vector<Room>& rooms) {
    ofstream file("rooms.txt");
    for (const auto& r : rooms) file << r.serialize() << "\n";
}

// Loads all bookings from bookings.txt file
vector<Booking> loadBookings() {
    vector<Booking> bookings;
    try {
        ifstream file("bookings.txt");
        if (!file.is_open()) throw runtime_error("Unable to open bookings.txt");

        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                try {
                    bookings.push_back(Booking::deserialize(line));
                } catch (const exception& e) {
                    cerr << "Error parsing booking data: " << e.what() << " (line: " << line << ")\n";
                }
            }
        }
    } catch (const exception& e) {
        cerr << "Exception in loadBookings(): " << e.what() << "\n";
    }
    return bookings;
}

// Saves all bookings to bookings.txt file
void saveBookings(const vector<Booking>& bookings) {
    ofstream file("bookings.txt");
    for (const auto& b : bookings) file << b.serialize() << "\n";
}

// ----------------- Pricing Strategy -----------------

// Abstract base class for pricing strategies (Strategy Design Pattern)
class PricingStrategy {
public:
    // Pure virtual function to calculate price based on number of nights
    virtual double calculate(int nights) = 0;
    virtual ~PricingStrategy() = default; // Virtual destructor for proper cleanup
};

// Pricing strategy for Single rooms
class SingleRoomPricing : public PricingStrategy {
public:
    // Calculates price as $100 per night
    double calculate(int nights) override { return nights * 100; }
};

// Pricing strategy for Double rooms
class DoubleRoomPricing : public PricingStrategy {
public:
    // Calculates price as $150 per night
    // Note: Inconsistent with Admin::addRoom ($180)
    double calculate(int nights) override { return nights * 150; }
};

<<<<<<< HEAD
// Pricing strategy for Suite rooms
class SuiteRoomPricing : public PricingStrategy {
public:
    // Calculates price as $250 per night
    // Note: Inconsistent with Admin::addRoom ($300)
    double calculate(int nights) override { return nights * 250; }
};

// Selects appropriate pricing strategy based on room type and calculates cost
double calculatePrice(const string& type, int nights) {
    PricingStrategy* strategy = nullptr;
    if (type == "Single") strategy = new SingleRoomPricing();
    else if (type == "Double") strategy = new DoubleRoomPricing();
    else if (type == "Suite") strategy = new SuiteRoomPricing();
    else strategy = new SingleRoomPricing(); // Fallback to Single pricing

    double cost = strategy->calculate(nights);
    delete strategy; // Clean up to prevent memory leak
    return cost;
=======
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

void showRoomRates() {
    cout << "\nRoom Rates:\n";
    cout << "Single: $100.00 per night\n";
    cout << "Double: $150.00 per night\n";
    cout << "Suite: $250.00 per night\n";
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

void cancelBooking(const string& guestName){
    vector<Booking> bookings;
    ifstream inFile("bookings.txt");
    string line;
    while (getline(inFile, line)) {
        bookings.push_back(Booking::deserialize(line));
    }
    inFile.close();

    if(bookings.empty()) {
        cout << "No bookings found.\n";
        return;
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

    // Update the room's availability
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    for (Room& room : rooms){
        if(room.roomNumber == roomNum){
            room.isAvailable = true;
            break;
        }
    }
    saveRoomsToFile(rooms, "rooms.txt");

    // Update bookings
    ofstream outFile("bookings.txt");
    for (const Booking& b : updatedBookings){
        outFile << b.serialize() << endl;
    }
    cout << "Booking has been cancelled.\n";
}

void showAllBookings(){
    ifstream inFile("bookings.txt");
    if (!inFile){
        cout<<"No bookings found,\n";
        return;
    }

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
    inFile.close();

    if (bookings.empty()) {
        cout << "No bookings found.\n";
        return;
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
            continue; // skip this booking
        }
        updatedBookings.push_back(b);
    }

    ofstream outFile("bookings.txt");
    for (const Booking& b : updatedBookings) {
        outFile << b.serialize() << endl;
    }
    outFile.close();

    // Update room availability
    vector<Room> rooms = loadRoomsFromFile("rooms.txt");
    for (Room& r : rooms) {
        if (r.roomNumber == roomNumber) {
            r.isAvailable = true;
            break;
        }
    }
    saveRoomsToFile(rooms, "rooms.txt");

    if (found) {
        cout << "Booking cancelled successfully.\n";
    } else {
        cout << "Booking not found.\n";
    }
}

void makeBookingAdmin(){
    cin.ignore();
    string guest;
    cout << "Enter guest name: ";
    getline(cin, guest);
    makeBooking(guest); 
>>>>>>> main
}

// ----------------- User Base Class -----------------

// Abstract base class for users (Guest or Admin)
class User {
protected:
    string username; // User's name
public:
    User(const string& uname) : username(uname) {}
    // Pure virtual function for displaying user-specific menu
    virtual void showMenu() = 0;
    virtual ~User() = default; // Virtual destructor for proper cleanup
};

// ----------------- Guest -----------------

// Represents a guest user with booking-related functionality
class Guest : public User {
public:
    Guest(const string& name) : User(name) {}

    // Displays the guest menu and handles user interactions
    void showMenu() override {
        int choice;
        do {
<<<<<<< HEAD
            cout << "\n--- Guest Menu ---\n";
            cout << "1. View Available Rooms\n2. Book a Room\n3. Cancel Booking\n4. View My Bookings\n5. Exit\nChoice: ";
            while (!(cin >> choice) || choice < 1 || choice > 5) {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid choice. Try again: ";
            }

            switch (choice) {
                case 1: viewAvailableRooms(); break;
                case 2: bookRoom(); break;
                case 3: cancelBooking(); break;
                case 4: viewMyBookings(); break;
                case 5: cout << "Goodbye, " << username << "!\n"; break;
=======
            cout << "\nGuest Menu:\n1. Show Available Rooms\n2. Show Room Rates\n3. Make Booking\n4. Cancel Booking\n5. Logout\nChoice: ";
            cin >> choice;

            switch (choice) {
                case 1: showAvailableRooms(); break;
                case 2: showRoomRates(); break;
                case 3: makeBooking(username); break;
                case 4: cancelBooking(username); break;
                case 5: return;
                default: cout << "Invalid choice.\n";
>>>>>>> main
            }
        } while (choice != 5);
    }

    // Displays all available rooms
    void viewAvailableRooms() {
        auto rooms = loadRooms();
        cout << "\nAvailable Rooms:\n";
        for (const auto& r : rooms)
            if (r.isAvailable)
                cout << "Room " << r.roomNumber << " (" << r.roomType << ")\n";
    }

    // Prints available rooms with their prices
    void printRoomPrices(const vector<Room>& rooms) {
        cout << "\nAvailable Rooms and Prices:\n";
        cout << left << setw(12) << "Room No." << setw(12) << "Type" << setw(12) << "Price\n";
        cout << "-----------------------------------------\n";
        for (const auto& r : rooms) {
            if (r.isAvailable) {
                cout << left << setw(12) << r.roomNumber
                     << setw(12) << r.roomType
                     << "$" << fixed << setprecision(2) << r.price << "\n";
            }
        }
    }

    // Handles room booking process
    void bookRoom() {
        auto rooms = loadRooms();
        auto bookings = loadBookings();

        // Show available rooms with prices
        printRoomPrices(rooms);

        // Check if any rooms are available
        bool anyAvailable = any_of(rooms.begin(), rooms.end(), [](const Room& r) {
            return r.isAvailable;
        });

        if (!anyAvailable) {
            cout << "Sorry, no rooms are available at the moment.\n";
            return;
        }

        // Select room
        string roomInput;
        int roomNum;
        while (true) {
            cout << "Enter room number to book (0 to return to menu): ";
            cin >> roomInput;
            stringstream ss(roomInput);
            if ((ss >> roomNum) && ss.eof()) break;
            cout << "Invalid input. Please enter a valid room number: ";
        }

        if (roomNum == 0) {
            cout << "Returning to menu...\n";
            return;
        }

        // Check if room exists and is available
        auto it = find_if(rooms.begin(), rooms.end(),
            [roomNum](const Room& r) { return r.roomNumber == roomNum; });

        if (it == rooms.end() || !it->isAvailable) {
            cout << "Room is not available or does not exist.\n";
            return;
        }

        // Prevent double booking by the same guest
        for (const auto& b : bookings) {
            if (b.guestName == username && b.roomNumber == roomNum) {
                cout << "You already have a booking for this room.\n";
                return;
            }
        }

        // Get number of nights
        string nightsInput;
        int nights;
        while (true) {
            cout << "Enter number of nights: ";
            cin >> nightsInput;
            stringstream ss(nightsInput);
            if ((ss >> nights) && ss.eof() && nights > 0 && nights <= 30) break;
            cout << "Invalid. Enter a positive number (1–30): ";
        }

        // Calculate cost and book
        double cost = calculatePrice(it->roomType, nights);
        Booking booking{username, roomNum, nights, cost, ""}; // Reference assigned on confirmation
        bookings.push_back(booking);
        saveBookings(bookings);

        it->isAvailable = false;
        saveRooms(rooms);

        cout << "Booking successful!\nTotal cost for Room " << roomNum << ": $"
             << fixed << setprecision(2) << cost << " for " << nights << " night(s).\n";
    }

    // Cancels a booking for the current guest
    void cancelBooking() {
        auto bookings = loadBookings();
        auto rooms = loadRooms();

        // Filter bookings for the current guest
        vector<Booking> myBookings;
        for (const auto& b : bookings) {
            if (b.guestName == username) {
                myBookings.push_back(b);
            }
        }

        if (myBookings.empty()) {
            cout << "You have no bookings to cancel.\n";
            return;
        }

        // Display guest's bookings
        cout << "\nYour Current Bookings:\n";
        for (const auto& b : myBookings) {
            cout << "Guest: " << b.guestName
                 << ", Room " << b.roomNumber
                 << ", Nights: " << b.nights
                 << ", Total: $" << fixed << setprecision(2) << b.totalCost
                 << ", Status: " << (b.referenceID.empty() ? "Unpaid" : "Paid") << "\n";
        }

        // Get room number to cancel
        int roomNum;
        cout << "Enter the room number to cancel (0 to return to menu): ";
        while (!(cin >> roomNum)) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter a valid room number (0 to cancel): ";
        }

        if (roomNum == 0) {
            cout << "Returning to menu...\n";
            return;
        }

        // Find and remove the booking
        bool found = false;
        for (auto it = bookings.begin(); it != bookings.end(); ++it) {
            if (it->guestName == username && it->roomNumber == roomNum) {
                bookings.erase(it);
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "No booking found under your name for room " << roomNum << ".\n";
            return;
        }

        // Mark room as available
        for (auto& r : rooms) {
            if (r.roomNumber == roomNum) {
                r.isAvailable = true;
                break;
            }
        }

        saveBookings(bookings);
        saveRooms(rooms);

        cout << "Booking for room " << roomNum << " has been canceled.\n";
    }

    // Displays and allows confirmation of guest's bookings
    void viewMyBookings() {
        auto bookings = loadBookings();
        bool found = false;

        cout << "\n--- Your Bookings ---\n";
        for (const auto& b : bookings) {
            if (b.guestName == username) {
                cout << "Room " << b.roomNumber
                     << ", Nights: " << b.nights
                     << ", Cost: $" << fixed << setprecision(2) << b.totalCost
                     << ", Status: " << (b.referenceID.empty() ? "Unpaid" : "Paid");
                if (!b.referenceID.empty())
                    cout << ", Reference ID: " << b.referenceID;
                cout << "\n";
                found = true;
            }
        }

        if (!found) {
            cout << "You have no bookings.\n";
            return;
        }

        // Ask if user wants to confirm a booking
        string confirmInput;
        char confirmChar;
        while (true) {
            cout << "\nWould you like to confirm a booking and receive a reference ID? (y/n): ";
            cin >> confirmInput;
            if (confirmInput.length() == 1) {
                confirmChar = tolower(confirmInput[0]);
                if (confirmChar == 'y' || confirmChar == 'n') break;
            }
            cout << "Invalid input. Please enter 'y' or 'n'.\n";
        }

        if (confirmChar != 'y') return;

        // Get room number to confirm
        string roomInput;
        int roomNum;
        while (true) {
            cout << "Enter the room number to confirm: ";
            cin >> roomInput;
            stringstream ss(roomInput);
            if ((ss >> roomNum) && (ss.eof())) break;
            cout << "Invalid input. Enter a valid numeric room number: ";
        }

        // Update booking with reference ID
        bool updated = false;
        for (auto& b : bookings) {
            if (b.guestName == username && b.roomNumber == roomNum) {
                if (!b.referenceID.empty()) {
                    cout << "This booking is already confirmed with Reference ID: " << b.referenceID << "\n";
                    return;
                }
                b.referenceID = generateReferenceID();
                cout << "Booking confirmed! Reference ID: " << b.referenceID << "\n";
                updated = true;
                break;
            }
        }

        if (updated) saveBookings(bookings);
        else cout << "No matching booking found to confirm.\n";
    }
};

// ----------------- Admin -----------------

// Represents an admin user with management functionality
class Admin : public User {
public:
    Admin(const string& name) : User(name) {}

    // Displays the admin menu and handles user interactions
    void showMenu() override {
        int choice;
        do {
            cout << "\n--- Admin Menu ---\n";
            cout << "1. View All Rooms\n2. View All Bookings\n3. Add Room\n4. Delete Room\n5. Update Room Type\n6. Cancel Any Booking\n7. Logout\nChoice: ";
            while (!(cin >> choice) || choice < 1 || choice > 7) {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid choice. Try again: ";
            }

            switch (choice) {
                case 1: viewAllRooms(); break;
                case 2: viewAllBookings(); break;
                case 3: addRoom(); break;
                case 4: deleteRoom(); break;
                case 5: updateRoomType(); break;
                case 6: cancelAnyBooking(); break;
                case 7: cout << "Logging out...\n"; break;
            }
        } while (choice != 7);
    }

    // Displays all rooms, including availability
    void viewAllRooms() {
        auto rooms = loadRooms();
        cout << "\nAll Rooms:\n";
        for (const auto& r : rooms)
            cout << "Room " << r.roomNumber << " (" << r.roomType << ") - "
                 << (r.isAvailable ? "Available" : "Occupied") << "\n";
    }

    // Displays all bookings in the system
    void viewAllBookings() {
        auto bookings = loadBookings();
        cout << "\nAll Bookings:\n";
        for (const auto& b : bookings)
            cout << "Guest: " << b.guestName
                 << ", Room " << b.roomNumber
                 << ", Nights: " << b.nights
                 << ", Total: $" << fixed << setprecision(2) << b.totalCost
                 << ", Status: " << (b.referenceID.empty() ? "Unpaid" : "Paid") << "\n";
    }

    // Adds a new room to the system
    void addRoom() {
        auto rooms = loadRooms();

        // Display current rooms
        cout << "\n--- Current Rooms in the System ---\n";
        if (rooms.empty()) {
            cout << "No rooms found.\n";
        } else {
            for (const auto& r : rooms) {
                cout << "Room " << r.roomNumber << " (" << r.roomType << ") - "
                     << (r.isAvailable ? "Available" : "Occupied") << "\n";
            }
        }

        // Get new room number
        int num;
        cout << "\nEnter new room number (0 to cancel): ";
        while (!(cin >> num) || num < 0) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter a valid room number (0 to cancel): ";
        }

        if (num == 0) {
            cout << "Room addition cancelled.\n";
            return;
        }

        // Check for duplicate room number
        auto it = find_if(rooms.begin(), rooms.end(), [num](const Room& r) {
            return r.roomNumber == num;
        });

        if (it != rooms.end()) {
            cout << "Room number already exists. Cannot add duplicate.\n";
            return;
        }

        // Get room type
        cin.ignore();
        string type;
        while (true) {
            cout << "Enter room type (Single/Double/Suite): ";
            getline(cin, type);

            transform(type.begin(), type.end(), type.begin(), ::tolower);
            type[0] = toupper(type[0]);

            if (type == "Single" || type == "Double" || type == "Suite") break;
            cout << "Invalid room type. Please enter Single, Double, or Suite.\n";
        }

        // Assign price based on type
        // Note: Prices here ($100, $180, $300) differ from PricingStrategy ($100, $150, $250)
        double price;
        if (type == "Single") price = 100.0;
        else if (type == "Double") price = 180.0;
        else if (type == "Suite") price = 300.0;

        Room newRoom = { num, type, price, true }; // New room is available by default

        // Note: Appending directly to file; consider loading, modifying, and rewriting for consistency
        ofstream outFile("rooms.txt", ios::app);
        if (!outFile) {
            cout << "Error opening rooms.txt for writing.\n";
            return;
        }

        outFile << newRoom.serialize() << endl;
        outFile.close();

        cout << "Room added successfully.\n";
    }

    // Deletes a room from the system
    void deleteRoom() {
        auto rooms = loadRooms();
        auto bookings = loadBookings();

        // Display current rooms
        cout << "\n--- Current Rooms in the System ---\n";
        if (rooms.empty()) {
            cout << "No rooms found.\n";
            return;
        }

        for (const auto& r : rooms) {
            cout << "Room " << r.roomNumber << " (" << r.roomType << ") - "
                 << (r.isAvailable ? "Available" : "Occupied") << "\n";
        }

        // Get room number to delete
        int num;
        cout << "\nEnter room number to delete (0 to cancel): ";
        while (!(cin >> num) || num < 0) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid. Enter a valid room number (0 to cancel): ";
        }

        if (num == 0) {
            cout << "Deletion cancelled. Returning to menu...\n";
            return;
        }

        // Check if room has active bookings
        bool hasBooking = any_of(bookings.begin(), bookings.end(),
                                 [num](const Booking& b) { return b.roomNumber == num; });

        if (hasBooking) {
            cout << "Cannot delete room " << num << " because it has an active booking.\n";
            return;
        }

        // Remove room
        auto it = remove_if(rooms.begin(), rooms.end(),
            [num](const Room& r) { return r.roomNumber == num; });

        if (it != rooms.end()) {
            rooms.erase(it, rooms.end());
            saveRooms(rooms);
            cout << "Room " << num << " deleted successfully.\n";
        } else {
            cout << "Room number not found. Nothing deleted.\n";
        }
    }

    // Updates the type and price of an existing room
    void updateRoomType() {
        auto rooms = loadRooms();

        // Display all rooms
        cout << "\n--- Current Rooms in the System ---\n";
        if (rooms.empty()) {
            cout << "No rooms available to update.\n";
            return;
        }

        for (const auto& r : rooms) {
            cout << "Room " << r.roomNumber << " (" << r.roomType << ") - "
                 << (r.isAvailable ? "Available" : "Occupied") << "\n";
        }

        // Get room number
        string input;
        int num;
        while (true) {
            cout << "\nEnter room number to update (0 to cancel): ";
            cin >> input;
            stringstream ss(input);
            if ((ss >> num) && ss.eof() && num >= 0) break;
            cout << "Invalid input. Enter a valid room number (0 to cancel): ";
        }

        if (num == 0) {
            cout << "Update cancelled. Returning to menu...\n";
            return;
        }

        // Find room
        auto it = find_if(rooms.begin(), rooms.end(),
                          [num](const Room& r) { return r.roomNumber == num; });

        if (it == rooms.end()) {
            cout << "Room number not found.\n";
            return;
        }

        // Get new room type
        cin.ignore();
        string type;
        while (true) {
            cout << "Enter new room type (Single/Double/Suite): ";
            getline(cin, type);

            transform(type.begin(), type.end(), type.begin(), ::tolower);
            if (!type.empty()) type[0] = toupper(type[0]);

            if (type == "Single" || type == "Double" || type == "Suite") break;
            cout << "Invalid room type. Please enter Single, Double, or Suite.\n";
        }

        // Update room type and price
        // Note: Prices here ($100, $180, $300) differ from PricingStrategy ($100, $150, $250)
        it->roomType = type;
        if (type == "Single") it->price = 100.0;
        else if (type == "Double") it->price = 180.0;
        else if (type == "Suite") it->price = 300.0;

        saveRooms(rooms);
        cout << "Room " << num << " type updated successfully to " << type
             << " with new price $" << fixed << setprecision(2) << it->price << ".\n";
    }

    // Cancels any booking in the system
    void cancelAnyBooking() {
        auto bookings = loadBookings();
        if (bookings.empty()) {
            cout << "\nNo bookings found to cancel.\n";
            return;
        }

        // Display all bookings
        cout << "\n--- Current Bookings ---\n";
        for (const auto& b : bookings) {
            cout << "Guest: " << b.guestName
                 << ", Room " << b.roomNumber
                 << ", Nights: " << b.nights
                 << ", Total: $" << fixed << setprecision(2) << b.totalCost
                 << ", Status: " << (b.referenceID.empty() ? "Unpaid" : "Paid") << "\n";
        }

        // Get room number to cancel
        int roomNum;
        cout << "\nEnter room number to cancel booking (0 to cancel): ";
        while (!(cin >> roomNum) || roomNum < 0) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter a valid room number (0 to cancel): ";
        }

        if (roomNum == 0) {
            cout << "Cancellation aborted. Returning to menu...\n";
            return;
        }

        // Remove booking
        auto it = remove_if(bookings.begin(), bookings.end(),
            [roomNum](const Booking& b) { return b.roomNumber == roomNum; });

        if (it != bookings.end()) {
            bookings.erase(it, bookings.end());
            auto rooms = loadRooms();
            for (auto& r : rooms)
                if (r.roomNumber == roomNum)
                    r.isAvailable = true;

            saveRooms(rooms);
            saveBookings(bookings);
            cout << "Booking for room " << roomNum << " canceled successfully.\n";
        } else {
            cout << "No booking found for room number " << roomNum << ".\n";
        }
    }
};

// ----------------- Login -----------------

// Handles user login and returns appropriate User object
User* login() {
    int choice;
    cout << "\nLogin as:\n1. Admin\n2. Guest\nChoice: ";
    while (!(cin >> choice) || (choice != 1 && choice != 2)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid. Enter 1 for Admin, 2 for Guest: ";
    }

    string name, pass;
    cin.ignore();

    if (choice == 1) {
        cout << "Username: "; getline(cin, name);
        cout << "Password: "; getline(cin, pass);
        if (name == "admin" && pass == "123") return new Admin(name);
        else cout << "Invalid credentials.\n";
    } else {
        cout << "Enter your name: ";
        getline(cin, name);
        if (!name.empty()) return new Guest(name);
        else cout << "Name cannot be empty.\n";
    }

    return nullptr;
}

// ----------------- Main -----------------

// Main entry point for the hotel reservation system
int main() {
    cout << "=== HOTEL RESERVATION SYSTEM ===\n";
    try {
        while (true) {
            User* user = nullptr;
            while (!user) user = login(); // Keep prompting until valid login

            user->showMenu(); // Display user-specific menu
            delete user; // Clean up user object
            cout << "\nLogged out.\n";
        }
    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << "\n";
    }
    return 0;
}

//teehee