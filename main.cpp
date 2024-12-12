#include "Room.h"
#include <iostream>
#include <vector>

using namespace std;

void menu();
void look(Room* current_room);
void exits(Room* current_room);
void add(Room* current, vector<Room*>& allRooms);
void connect(Room* current, vector<Room*>& allRooms);
void rename(Room* current);
void description(Room* current);
Direction ch2Direct(char c);  // Use the Direction enum defined in Room.h

int main() {
    Room* current;  // keep track of the current room we are in
    vector<Room*> allRooms;

    current = new Room("Kitchen", "A mess of dishes awaits cleaning");
    allRooms.push_back(current);  // add pointer to kitchen to allRooms

    cout << "Current room is " << *current << endl;

    Room* temp = new Room("Den", "A TV is blaring loudly");
    allRooms.push_back(temp);

    current->connect(EAST, temp, WEST);
    // connect temp (Den) to the east exit of current(Kitchen), and Kitchen to the west exit of temp(Den)
    cout << "Just connected " << *(current->east()) << " to the east of Kitchen" << endl;

    // move into the Den
    current = current->east();
    cout << "Current room is " << *current << endl;

    // create the bedroom
    temp = new Room("Bedroom", "The sun shines thru a picture window");
    allRooms.push_back(temp);

    // connect the Den to the Bedroom
    current->connect(SOUTH, temp, NORTH);

    // move to the Bedroom
    current = current->south();
    cout << "Current room is " << *current << endl;

    // create the patio
    temp = new Room("Patio", "A plastic table filled with barbecue food");
    allRooms.push_back(temp);

    // connect the bedroom to the Patio
    current->connect(WEST, temp, EAST);
    // move current to the Patio
    current = current->west();
    cout << "Current room is " << *current << endl;

    // connect patio to kitchen using allRooms
    current->connect(NORTH, allRooms[0], SOUTH);

    cout << "ALL ROOMS" << endl;
    for (size_t k = 0; k < allRooms.size(); ++k)
        cout << k << " " << allRooms[k]->name() << endl;

    current = current->north();
    cout << "Current room is " << *current << endl;

    cout << "Available Exits:" << endl;
    cout << " East: " << (current->east() ? current->east()->name() : "Nothing") << endl;
    cout << " South: " << (current->south() ? current->south()->name() : "Nothing") << endl;

    cout << "Which exit (E or S) do you wish to go through?" << endl;
    char input;
    cin >> input;
    switch (input) {
        case 'E': case 'e':
            if (current->east()) current = current->east();
            else cout << "You cannot go that way" << endl;
            break;
        case 'S': case 's':
            if (current->south()) current = current->south();
            else cout << "You cannot go that way" << endl;
            break;
    }

    cout << "Current room is " << *current << endl;

    do {
        cout << "Enter a command (? to list):                       <<<< ";
        cin >> input;
        switch (input) {
            case '?':
                menu();
                break;
            case 'a':
            case 'A':
                add(current, allRooms);
                break;
            case 'n':
            case 'N':
                if (current->north() != nullptr)
                    current = current->north();
                else
                    cout << "You cannot go that way" << endl;
                break;
            case 'r':
            case 'R':
                rename(current);
                break;
            case 'd':
            case 'D':
                description(current);
                break;
            case 's':
            case 'S':
                if (current->south() != nullptr)
                    current = current->south();
                else
                    cout << "You cannot go that way" << endl;
                break;
            case 'c':
            case 'C':
                connect(current, allRooms);
                break;
            case 'e':
            case 'E':
                if (current->east() != nullptr)
                    current = current->east();
                else
                    cout << "You cannot go that way" << endl;
                break;
            case 'w':
            case 'W':
                if (current->west() != nullptr)
                    current = current->west();
                else
                    cout << "You cannot go that way" << endl;
                break;
            case 'l':
            case 'L':
                look(current);
                break;
            case 'x':
            case 'X':
                exits(current);
                break;
            case 'q':
            case 'Q':
                cout << "Quitting..." << endl;
                break;
            default:
                cout << "Invalid command" << endl;
        }
    } while (input != 'q' && input != 'Q');

    for (Room* room : allRooms) {
        delete room;
    }

    return 0;
}

// Show the name and description of a room
void look(Room* r) {
    cout << "You are in " << r->name() << endl;
    cout << r->description() << endl;
}

// Add a new room connected to the current room
void add(Room* current, vector<Room*>& allRooms) {
    string name, description;
    Direction dir;

    cout << "Connect new room in which direction? (N,S,E,W)     <<<< ";
    cin.ignore();
    getline(cin, name);

    cout << "Which exit of the new room leads to this room?     <<<< ";
    getline(cin, description);
    cout << "Room connection ok" << endl;

    cout << "Enter the direction to connect (N/S/E/W): ";
    char dirChar;
    cin >> dirChar;
    dir = ch2Direct(dirChar);

    if ((dir == NORTH && current->north() != nullptr) ||
        (dir == SOUTH && current->south() != nullptr) ||
        (dir == EAST && current->east() != nullptr) ||
        (dir == WEST && current->west() != nullptr)) {
        cout << "The direction is already occupied. Choose another direction." << endl;
        return;
    }

    Room* newRoom = new Room(name, description);
    allRooms.push_back(newRoom);
    current->connect(dir, newRoom, static_cast<Direction>((dir + 2) % 4));

    cout << "New room added and connected!" << endl;
}

// Connect current room to an existing room
void connect(Room* current, vector<Room*>& allRooms) {
    int roomIndex;
    Direction dir;
    cout << "Enter the index of the room to connect to: ";
    cin >> roomIndex;
    if (roomIndex < 0 || roomIndex >= static_cast<int>(allRooms.size())) {
        cout << "Invalid room index." << endl;
        return;
    }
    cout << "Enter the direction to connect (N/S/E/W): ";
    char dirChar;
    cin >> dirChar;
    dir = ch2Direct(dirChar);

    if ((dir == NORTH && current->north() != nullptr) ||
        (dir == SOUTH && current->south() != nullptr) ||
        (dir == EAST && current->east() != nullptr) ||
        (dir == WEST && current->west() != nullptr)) {
        cout << "The direction is already occupied. Choose another direction." << endl;
        return;
    }

    Room* otherRoom = allRooms[roomIndex];
    current->connect(dir, otherRoom, static_cast<Direction>((dir + 2) % 4));

    cout << "Rooms connected!" << endl;
}

// Change the name of a room
void rename(Room* current) {
    string newName;
    cout << "Enter a new name for the room: ";
    cin.ignore();
    getline(cin, newName);
    current->set_name(newName);
    cout << "Room renamed!" << endl;
}

// Change the description of a room
void description(Room* current) {
    string text;
    cout << "Enter a new description for this room and hit <enter>" << endl;
    cin.ignore();
    getline(cin, text);
    current->set_description(text);
    cout << "Room description updated!" << endl;
}

// Show exits from the current room
void exits(Room* r) {
    cout << "Exits:" << endl;
    if (r->north() != nullptr)
        cout << "North: " << r->north()->name() << endl;
    else
        cout << "North: Nothing" << endl;

    if (r->south() != nullptr)
        cout << "South: " << r->south()->name() << endl;
    else
        cout << "South: Nothing" << endl;

    if (r->east() != nullptr)
        cout << "East: " << r->east()->name() << endl;
    else
        cout << "East: Nothing" << endl;

    if (r->west() != nullptr)
        cout << "West: " << r->west()->name() << endl;
    else
        cout << "West: Nothing" << endl;
}

// Display the menu
void menu() { 
    cout << "Please choose from the following: " << endl; 
    cout << " a for add --- add a new room" << endl;
    cout << " n for north --- move north (if possible)" << endl; 
    cout << " e for east --- move east (if possible)" << endl; 
    cout << " s for south --- move south (if possible)" << endl; 
    cout << " w for west --- move west (if possible)" << endl; 
    cout << " l for look --- look at room" << endl; 
    cout << " x for exits --- show room exits" << endl;
    cout << " r for rename --- rename the current room" << endl;
    cout << " d for description --- change the room description" << endl;
    cout << " c for connect --- connect this room to another room" << endl;
    cout << " q for quit --- quit the program" << endl;
}

Direction ch2Direct(char c) {
    switch (c) {
        case 'N': case 'n': return NORTH;
        case 'S': case 's': return SOUTH;
        case 'E': case 'e': return EAST;
        case 'W': case 'w': return WEST;
        default:
            cout << "Invalid direction!" << endl;
            return NORTH;  // Default fallback
    }
}
