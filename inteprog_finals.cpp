#include <iostream>
#include <string>
using namespace std;

void showRoomTypes() {
    cout << "We have the following room types available:" << endl;
    cout << "Please input the number that corresponds to the room type you want to book:" << endl;
    cout << "1. Single Room - $100 per night" << endl;
    cout << "2. Double Room - $150 per night" << endl;
    cout << "3. Suite - $250 per night" << endl;
}

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
    cout << "1. Admin" << endl;
    cout << "2. Guest" << endl;
    cout << "3. Exit" << endl;
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
                cout << "Welcome Admin " << adminName << "!" << endl;
                // Admin functionalities to be added here
            } else {
                cout << "Invalid credentials! Exiting..." << endl;
            }
            break;

        case 2: {
            cout << "Welcome, Guest!" << endl;
            showRoomTypes();
            int roomType;
            cout << "Please select a room type (1-3): ";
            cin >> roomType;

            if (roomType < 1 || roomType > 3) {
                cout << "Invalid room type selected!" << endl;
            } else {
                cout << "You have selected room type " << roomType << "." << endl;
                // Future: add booking details, dates, bill, etc.
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
