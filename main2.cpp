#include <iostream> 
#include <map> // store and manage rooms 
#include <vector>
#include <string>
#include <fstream> // saving and loading
#include <algorithm> 
#include <cctype>
#include <cstdlib> // random number generation
#include <ctime> // incorporates date and time to seed random number generator
#include <limits> // tells if type is int, signed, set bounds for iteration and inputs

using namespace std;

enum Direction { NORTH, EAST, SOUTH, WEST };

// forward declarations allow classes to reference each other
class Room;
class Object;
class Mobile;

class Room {
    string _name; // room name
    string _description; // room description
    Room* _exits[4]; // ptr to rooms [0=(N)orth,1=(E)ast,2=(S)outh,3=(W)est]
    vector<Object*> objects; // list of objects in room
    vector<Mobile*> mobiles; // list of mobiles in room
public:
    // initialize room object with (name, description) 
    Room(string n, string d) 
        : _name(n), _description(d) { // member initialization list 
        // loops thru directions, initialize exits to nullptr, meaning no connections yet 
        for (int i=0;i<4;i++) _exits[i]=nullptr; 
    }
    
    // Getters for name and description (private variables)
    // const helps to make it unmodifiable
    string name() const {
        return _name;
    }
    string description() const {
        return _description;
    }

    // Setters
    void set_name(const string& n){ // update _name after object is created
    // assign n to _name
        _name = n;
    } // rename room 
    void set_description(const string& d){
        _description = d;
    } // redefine room desc

    // Rooms in each direction
    // retrieves ptr in that direction
    Room* north() const {
        return _exits[NORTH];
    }
    Room* east() const {
        return _exits[EAST];
    }
    Room* south() const {
        return _exits[SOUTH];
    }
    Room* west() const {
        return _exits[WEST];
    }

    // Room connection
    // bool connect:
    // (current room, a ptr to the room that will connect to current, direction other room will connect back to current)
    // false if rooms have existing connections
    bool connect(Direction from, Room* other, Direction to) { 
        // If _exits[from] is not nullptr then there is a connection there already
        // check if current room has connection in 'from' direction
        if (_exits[from]!=nullptr)
        return false;
        // check if other room has connection in 'to' direction
        if (other->_exits[to]!=nullptr)
        return false;
        // If both are free, connect them
        _exits[from] = other; // Connect 'from' direction to 'other'
        other->_exits[to] = this; // Connect 'to' direction back to 'this'
        return true;

        /*Example of above logic: 
        Room* roomA = new Room();
    Room* roomB = new Room();

    bool success = roomA->connect(NORTH, roomB, SOUTH);
    if (success) {
        cout << "Rooms connected successfully!" << endl;
    } else {
        cout << "Failed to connect rooms." << endl;
    }
    */
    }

    // Add Objects to this room
    void addObject(Object* someObject) {
        objects.push_back(someObject); // adds ptr to someObject to end of vector
    } 
    // remove an object from room one room allowing to bring it to others
    void removeObject(Object* someObject) {
        // remove shifts elements equal to someObject
        // erase removes elements from back shifted there 
        // removes repeat occurences of someObject
        // remove happens first rearranging vector
        // remove(moves elemenets not equal to someObject to front, leaves elements equal to someObject at back in undefined state [?,?])
        objects.erase(remove(objects.begin(),objects.end(),someObject),objects.end());
        // remove() puts matching objects at end [book, lamp, key, key]
        // erase() removes everything after the marker
        // could be objects.erase(remove(objects.begin(), objects.end(), key), objects.end());
    }

    // // retrieve list of objects currently in room 
    const vector<Object*>& getObjects() const {
        return objects;
    }

    // Mobiles in rooms
    void addMobile(Mobile* aMob) {
        mobiles.push_back(aMob);
    }
    // point aMob to mobiles vector which represents characters currently in room
    void removeMobile(Mobile* aMob) {
        // similar to removeObjects, removes all occurences of mobiles in room
        mobiles.erase(remove(mobiles.begin(),mobiles.end(),aMob),mobiles.end());
    }
    // retrieve list of characters currently in room 
    // recieve reference to internal mobiles vector 
    const vector<Mobile*>& getMobiles() const {
        return mobiles;
    }
};

// Object Class Implementation
class Object {
public:
    string name;       // object name
    string description;// object description
    bool isMovable;    // can it be picked up
    bool isSticky;     // if not sticky, object returns home when dropped
    Room* homeRoom;    // the room considered its home 

    string lockedMsg; // message if locked and can't pick it up
    string takeMsg;  // message displayed when successfully picked up
    string dropMsg; // message displayed when the object is dropped in a room
    string vanishMsg; // message displayed when the object disappears

    //Constructor
    Object(string n, string d, bool movable, bool sticky, Room* home)
        // defined in class description above
        :name(n),description(d),isMovable(movable),isSticky(sticky),homeRoom(home) {
        // Custom messaging
        // string concatenation because not in i/o stream
        lockedMsg="Further inspection reveals the " + n + " is locked.";
        takeMsg="You take the " + n + ".";
        dropMsg="You drop the " + n + " on the floor.";
        vanishMsg="The "+ n +" disappears without a trace..";
    }
};

class Mobile {
public:
    string name;         // mobile's name
    string description;  // mobile's description
    Room* currentRoom;   // current room of the mobile
    bool canFollow;      // can it follow another mobile
    Mobile* following;   // who is it following
    bool canPickupObjects;// can this mobile pick up objects
    vector<Object*> mobInventory; // objects this mobile holds
    
    // Room* startRoom ptr to room where mobile start
    // canPick set to false meaning if parameter is not provided, it will automatically be false
    Mobile(string n, string d, Room* startRoom, bool follow, bool canPick=false)
        // currentRoom(startRoom) assigns startRoom ptr to currentRoom
        :name(n),description(d),currentRoom(startRoom),canFollow(follow),following(nullptr),canPickupObjects(canPick){
        }

    // moves mobile to a random available exit
    void moveRandomly() {
        // Get exits of current room
        // Vector of ptrs to rooms connected to currentRoom 
        vector<Room*> exits = {currentRoom->north(),currentRoom->east(),currentRoom->south(),currentRoom->west()};
        vector<int> possibleMoves; // creates empty vector to store valid exits
        // loop through exits iterate exits vector
        // if exits[i] is not null then room in direction
        // if valid exit is possible, adds index i to possibleMoves vector 
        // iterate through exits
        for (int i=0;i<4;i++){
            if (exits[i]) possibleMoves.push_back(i); // adds valid direction if connected room 
        }

        // If no possible moves exit function and return
        if (possibleMoves.empty()) 
        return;
        // Choose a random direction from possible ones
        //                         [remainding num calculated by dividing possibleMoves.size by rand() generated]
        int choice = possibleMoves[rand() % possibleMoves.size()]; // rand() generates rand num, %possible ensures result is in range 
        // if possibleMoves = {0, 2} the choices are 0(north) and 2(south)
        currentRoom -> removeMobile(this); // remove mobile from currentRoom
        currentRoom = exits[choice]; // update to target room
        currentRoom -> addMobile(this); // add mobile to new room
    }

    // Mobile follow implementation
    void followTarget() {
        // if mobile can follow and target is in different room, move there
        // condition 1: checks if following is nullptr
        // condition 2: ptr to room object(Room* currentRoom) from following to indicate current location
        // condition 3: checks if target is in different room from the follower 
        if (following && following->currentRoom && following->currentRoom!=currentRoom) {
            currentRoom->removeMobile(this);
            currentRoom=following->currentRoom; // move follower to same room as target
            currentRoom->addMobile(this); // adds follower to ensure follower is now listed in same room as target
        }
    }

    // Mobile can pick up or drop objects with some random chance
    void tryPickupOrDrop() {
        if(!canPickupObjects) 
        return; // exit function if mobile cant pickup 
        // 50% chance pick or drop
        int action = rand()%2; // random num generator 0 or 1 so mobile doesnt always pick or drop objects
        // object pickup 
        if (action==0) { // pickup will be 0
            // pick up attempt
            const auto& objs=currentRoom->getObjects(); // calls getObjects on currentRoom to retriece reference to vector of objects in room
            // check if room contains objects
            if(!objs.empty()) { 
                // loop through each object in objs collection
                // loop body
                // for (declaration : containter)
                for (auto* someObject: objs) { // auto* - deduces type // objs is a vector of object*, someObject deduces as Object*
                    // only pick movable
                    if(someObject->isMovable) { // check if someObject can be pickedup, skipped if not
                        currentRoom->removeObject(someObject); // remove someObject from room
                        mobInventory.push_back(someObject); // add picked up object someObject to mobiles inventory
                        cout<<name<<" picks up the "<<someObject->name<<".\n";
                        return;
                    }
                }
            }
        } else {
            // drop object handling
            if(!mobInventory.empty()) {
                Object* someObj=mobInventory.back(); // retrieves ptr to last object in mobInventory vector using back() method so someObj points to last object
                mobInventory.pop_back(); // remove last object from mobInventory vector, no longer needed in inventory
                if(someObj->isSticky) { // sticky objects remain in room where dropped
                    currentRoom->addObject(someObj); // add sticky objects to current room
                    cout<<name<<" drops "<<someObj->name<<".\n"; // notify drop
                } else {
                    // if not sticky, returns home
                    someObj->homeRoom->addObject(someObj); // adds non sticky back to homeRoom
                    cout<<name<<" drops "<<someObj->name<<" and it is gone.\n";
                }
            }
        }
    }
};

// Globals
bool builderMode=true;
vector<Object*> playerInventory; // what player carries
vector<Object*> allObjects; // All objects in game
vector<Mobile*> allMobiles; // All mobiles in game
map<int,Room*> rooms; // store rooms by ID, ptr to room object to store details
int currentRoomID=0;   // ID of current room player in
Room* currentRoom=nullptr;
int roomID=0; // ID counter for new rooms

void displayHelp() {
    cout<<"Please choose from the following: \n"
        <<"     n for north --- move north\n"
        <<"      e for east --- move east\n"
        <<"     s for south --- move south\n"
        <<"      w for west --- move west\n"
        <<"      l for look --- look at room\n"
        <<"     x for exits --- show room exits\n"
        <<"   c for connect --- connect this room to another (already made) room\n"
        <<"    r for rename --- rename this room\n"
        <<"      d for desc --- change description for this room\n"
        <<"       a for add --- add new room\n"
        <<"  jump for jump --- teleport to a specific room\n"
        <<"  save for save --- save the current game\n"
        <<"  load for load --- load a saved game\n"
        <<"inventory for inventory --- view your current inventory\n"
        <<"get (object) --- pick up an object in the room\n"
        <<"drop (object) --- drop an object from your inventory\n"
        <<"look (object/mobile) --- look at a specific object or mobile\n"
        <<"      q for quit --- quit game\n"
        <<"              ? --- show commands\n";
}

// Display exits 
// takes ptr to a Room object (Room* room)
void displayExits(Room* room) {
    // check if (room is connected from that direction, returns pointer of room if it exists else nothing)
    cout<<"  North -- " << (room->north() ? room->north()->name():"Nothing.") << "\n";
    cout<<"  East -- " << (room->east() ? room->east()->name():"Nothing.") << "\n";
    cout<<"  South -- " << (room->south() ? room->south()->name():"Nothing.") << "\n";
    cout<<"  West -- " << (room->west() ? room->west()->name():"Nothing.") << "\n";
}

// Show details of current room including objects and mobiles
void lookRoom(Room* room) { // take ptr to a room object 
    // call room->name to get rooms name and print it
    // call room->description and print it
    cout<<room->name()<<"\n"<<room->description()<<"\n"; 
    // get and loop through objects in a room
    const auto& objs = room->getObjects(); // iterate through objs vector , room->getObj invokes member func of room class
    // retrieves objects in room
    for (auto* o: objs) { // o is ptr to type stored in objs
    // 1. iteration o points to first object* in obj
    // 2. points to second
    // 3. etc. until all elements processed and printed
        cout<<"There is a "<<o->name<<" here.\n";
    }
    // get and loop through mobiles in a room
    const auto& mobs = room->getMobiles();
    for (auto* m: mobs) {
        cout<<m->name<<" is here.\n";
    }
}

// Look at object or mobile by name
// (ptr to currentRoom user is in, name of target to inspect)
void lookTarget(Room* currentRoom,const string& target) {
    // copies input target into local variable  
    string t = target;
    // search case sensitive ::to lower gets lowercase
    transform(t.begin(), t.end(), t.begin(), ::tolower);
    // check objects in room
    // loop through all objects in current room
    for (auto* obj: currentRoom->getObjects()) {
        string oname = obj->name; // retrieve name of current object
        //oname.begin points to first char in string
        //oname.begin in 3rd position stores result
        //name comparison referenced from cppreference.com
        // case sensitive comparison for user input
        transform (oname.begin(), oname.end(), oname.begin(), ::tolower);
        // provide info when player requests to look for specified target
        if(oname==t) {
            cout<<obj->description<<"\n";
        return;
        }
    }

    // check player inventory
    for (auto* obj: playerInventory) { // loop through objects in players inventory
        // Get name of current object
        string oname = obj->name;
        // convert objects name to lowercase for case comparison
        transform(oname.begin(), oname.end(), oname.begin(), ::tolower);
        // if objects name matches. target
        if(oname == t){ 
            cout << obj->description<<"\n"; // print description of object
            return; 
        }
    }
    // check mobiles in room
    for (auto* mob: currentRoom->getMobiles()) { // loop through all mobiles in room
        // convert mobiles name to lowercase for case comparison
        string mname = mob->name; // extract name of mobile object
        // case sensitivity
        transform(mname.begin(), mname.end(), mname.begin(), ::tolower);
        // mobile name == target name
        if(mname == t) {
            cout<<mob->description<<"\n"; // print description of mobile
            if(!mob->mobInventory.empty()) { // if mobile has items in inventory then print 
                cout<<mob->name<<" is carrying:\n";
                for (auto* o: mob->mobInventory) { // loop through all items in the mobiles inventory //auto o is ptr to type so vector<object>, auto becomes object* o
                    cout <<"  " << o->name << "\n"; // print the name of each item in the inventory, indented for readability
                }
            }
            return;
        }
    }
    cout<<"You don't see that here.\n";
}

// Pickup objects from room floor
void attemptGet(Room* currentRoom,const string& objectName) {
    // convert target objects name to lowercase for case comparison
    string t = objectName;
    transform(t.begin(), t.end(), t.begin(), ::tolower);
    
    // loop through all objects currently in the room
    for (auto* obj: currentRoom->getObjects()) {
        // convert the current objects name to lowercase for case comparison
        string oname=obj->name;transform(oname.begin(), oname.end(), oname.begin(), ::tolower);
        if(oname == t) {
            // if object is not movable, print the locked message and exit
            if(!obj->isMovable){cout<<obj->lockedMsg<<"\n"; // provide feedback that the object cannot be moved
            return; // exit the function since the object cant be picked up
            }
            else {
                // remove from room and add to player inventory
                // 1. Remove object from room
                currentRoom->removeObject(obj);
                // 2. Add the object to players inventory
                playerInventory.push_back(obj);
                // 3. print message confirming object was picked up
                cout<<obj->takeMsg<<"\n";
                return;
            }
        }
    }
    cout<<"You don't see that here.\n";
}

// Drop objects from inventory
void attemptDrop(Room* currentRoom,const string& objectName) {
    // copy targets objects name into local variable
    string t = objectName;
    // convert object name to lowercase for comparison
    transform(t.begin(), t.end(), t.begin(), ::tolower);
    // loop through all objects in players industry
    for (size_t i=0;i<playerInventory.size();i++){
        // get object at index i in players inventory
        Object* anObject=playerInventory[i];
        // copy objects name into local variable
        string oname = anObject->name;
        transform(oname.begin(), oname.end(), oname.begin(), ::tolower);
        // check if current objects name matches the target name
        if(oname == t){
            // remove object from the players inventory
            playerInventory.erase(playerInventory.begin()+i);
            // check if object is STICKY 
            if(anObject->isSticky) {
                // add object to current room
                currentRoom -> addObject(anObject);
                cout<<anObject->dropMsg<<"\n";
            } else {
                // if object is not sticky it disappears 
                // add back to home room
                anObject->homeRoom->addObject(anObject);
                cout<<anObject->vanishMsg<<"\n";
            }
            return;
        }
    }
    // No object in room
    cout<<"That is not here.\n";
}

// Saving and loading features
// used cplusplus.com as reference
// tutorial for saving and loading in c++ from youtube
// save game 
// (map stores unique Ids to, Room* being accessed by poiner to store memory in room)
// reference means any change modified rooms 
void saveGame(map<int, Room*>& rooms, 
              Room* currentRoom,const string& filename,//contains room function operates on, filename contains data for room state
              const vector<Object*>& allObjects, // list of all objects placed in room
              const vector<Mobile*>& allMobiles, // list of mobiles in specific room
              int currentRoomID) { // id used to identify and interact with specific room
    // open file to save game data 
    // ofstream creates output stream object named outFile associated with fileName
    // opens file in write mode 
    ofstream outFile(filename); //output file(object)
    if(!outFile) { // if no file (false) basically
        cout<<"Error saving the game.\n"; 
        return;
    }

    // saving room data
    // [id the key from rooms map(an int), rm the value from rooms map(room*)]
    for (auto& [id,rm]: rooms) { // loop through all rooms in the map, references rooms ptr
        // write the room ID, name, description to the file 
        //rm->name() accesses room object and calls name
        outFile<<"ROOM|"<<id<<"|"<<rm->name()<<"|"<<rm->description()<<"\n";
    }
    outFile<<"END_ROOMS\n"; // mark the end of the room data in the file

    // save connections between rooms 
    // range based for loop referenced from geekforgeeks.com
    for (auto& [id, rm] : rooms) { // loop through all rooms in map
        // initialize directions with -1 meaning no connection because 0=North, etc..
        int n = -1, e = -1, s = -1, w = -1;  
        // handles north->room102
        for (auto&[oid, oroom] : rooms) { // check each room to find connections
            // check if rm's north/east/south/west matches oroom
            //rm->north() checks what room is connected to north of current rm 
            if (rm->north()==oroom) //oid - other room id, oroom - reference to room object
            n=oid; // If the current room's north points to oroom, set n to oroom's ID
            if (rm->east()==oroom) // calls east to return ptr or null if none
            e=oid;
            if (rm->south()==oroom) // would match oroom with ID 101
            s=oid; // s variable updated s = 101
            if (rm->west()==oroom) //compare room ptrs to see if rooms connected
            w=oid;
        }
        // write connection line to the file after loops get final value
        // no connetion print -1
        outFile << "CONN|" << id << "|" << n << "|" << e << "|" << s << "|" << w << "\n";
    }
    // mark the end of connection data in the file
    outFile << "END_CONNS\n";
    // save the Id of current room
    outFile << "CURRENT_ROOM|" << currentRoomID << "\n";
    // test confirmation
    cout<<"Game saved.\n";
}

// load game
void loadGame(map<int,Room*>& rooms,Room*& currentRoom,const string& filename,
              vector<Object*>& allObjects,vector<Mobile*>& allMobiles,int& currentRoomID) {
    cout<<"Game loaded successfully.\n";
}

int main() {
    // Use current time for random seed each time program runs
    // w3 schools c++ rand() function document as reference
    srand((unsigned)time(NULL));

    cout<<"Enable builder mode? (y/n):                                        <<<< ";
    {
        string answer;
        if(!getline(cin, answer)){ // exit program if error
            return 0;
        }
        
        // case comparison 
        transform(answer.begin(), answer.end(), answer.begin(), ::tolower); 
        if(answer=="n") 
        builderMode=false; // Builder mode off if answer is n
    }

    //Starting Room
    // create new room object using "new" keyword
    // (name, description) constructor outline
    currentRoom = new Room("The void","There is nothing but blackness in every direction.");
    // adds new created room pointed by currentRoom to the room maps
    // uses current value of roomID as the key in map
    // increments roomID after assigning the room to ensure the next room has a unique ID
    /*Example
    Initial state:
    roomID = 0;
    rooms = {};
    After 
     rooms[roomID++] = currentRoom;
     {
    0: <Pointer to the new Room ("The void")>
        }   */
    rooms[roomID++] = currentRoom;

    // Objects for pickup
    // adding a sample object and mobile so player can test pick up
    // (name, description) constructor outline
    // true: indicates object is movable
    // true: indicates object is sticky
    // currentRoom: the home room for object
    Object* sampleObj = new Object("Hammer","A tool...",true,true,currentRoom);
    // add created object to global list of all objects in game
    allObjects.push_back(sampleObj);
    currentRoom->addObject(sampleObj); // place object in current room

    // create new mobile
    Mobile* sampleMob = new Mobile("Isaiah","A bright...",currentRoom,false,true);
    // (Mobile name, description, starting room where mobile located, false cannot follow another mobile, true mobile can pickup objects)
    allMobiles.push_back(sampleMob); // add created mobile to global lists of all mobiles in game
    currentRoom->addMobile(sampleMob); // place mobile in current room 

    // Main game loop
    // stops when 'q' is entered
    while (true) {
        cout<<"\nEnter a command (? to list):                                       <<<< ";
        string line;
        if(!getline(cin,line)) 
        break;
        if(line.empty()) 
        continue; // program continues even if <enter> and empty

        // Extract first word as command
        string command;
        {
            // finds position of first space character in input string 'line'
            size_t pos = line.find(' ');
            if(pos == string::npos) { // check if no space found in string
                command = line; // if no space exists, assign entire line is treated as command assign second part to line variable
                line = ""; // set 'line' to empty string
            } else {
                // if space found in string
                command = line.substr(0,pos);
                // extract substring from start of 'line' to first space
                // assign substring to 'command' (first word in input)
                line = line.substr(pos+1); // extract substring from character after first space
            }   // example go north would be north
        }
        
        // copy of command and cmdLower holds lowercase
        string cmdLower = command;
        // case comparison for lowercase
        // transform allows operations for elements in a range
        // syntax (start, end, result, operation)
        // example: go north
        // .begin points to g .end to to
        // referenced tolower, toupper documentation w3 schools
        transform(cmdLower.begin(), cmdLower.end(), cmdLower.begin(), ::tolower);
        
        if(cmdLower=="q") {
            cout<<"Quitting...\n";
            break;
        } else if (cmdLower=="?") {
            displayHelp();
        } else if (cmdLower=="l") {
            lookRoom(currentRoom); // ptr reference to object (currentRoom) display info on currentRoom
        } else if (cmdLower=="r") {
            if(!builderMode) { // ! if builerMode is false
                cout<<"Builder mode is OFF.\n";
            }
            else {
                cout<<"Enter a new name for this room and hit <enter>:                    <<<< ";
                string newName;
                if(!getline(cin,newName)){ // ! if error reading input //read from file
                    cout<<"Error reading name.\n";
                }else {
                    currentRoom->set_name(newName);
                }
            }
        } else if (cmdLower=="d") {
            if(!builderMode){
                cout<<"Builder mode is OFF.\n";
            }
            else {
                cout<<"Enter a new description for this room and hit <enter>:       <<<< ";
                string descInput; 
                if(!getline(cin,descInput)){
                    cout<<"Error.\n";
                }
                else
                {   
                    // updates description of currentRoom
                    /*Example: if 
                    descInput = "spooky, dark cave"
                    the rooms description now reflects this value*/
                    currentRoom->set_description(descInput);
                }
            }
        } else if (cmdLower=="a") {
            if(!builderMode) {
                cout<<"Builder mode is OFF.\n";
            }
            else {
                cout<<"Connect new room in which direction? (N,S,E,W):                    <<<< ";
                string directionChoice; 
                if(!getline(cin,directionChoice)){
                    cout<<"Error.\n";
                    continue;
                }
                // uppercase comparison for case sensitivity
                transform(directionChoice.begin(), directionChoice.end(), directionChoice.begin(), ::toupper);
                
                // exitDit of type Direction from enum
                // stores direction chosen by user mapped to enum value
                Direction exitDir;
                // asign to enum values
                if(directionChoice == "N") exitDir = NORTH;
                else if(directionChoice == "E") exitDir = EAST;
                else if(directionChoice == "S") exitDir = SOUTH;
                else if(directionChoice == "W") exitDir = WEST;
                else {
                    cout<<"Invalid direction.\n";
                    continue; // skip loop and prompt user again
                }
                
                // check if exit in direction
                // check if chosen direction is north and if current room has north then exit
                if((exitDir==NORTH&&currentRoom->north())||
                   (exitDir==EAST&&currentRoom->east())||
                   (exitDir==SOUTH&&currentRoom->south())||
                   (exitDir==WEST&&currentRoom->west())) 
                   {
                       // if these are true then connection exists
                    cout<<"Exit already exists!\n";
                    continue;
                }

                cout<<"Which exit of the new room leads to this room? (N,S,E,W):          <<<< ";
                string toDirectionChoice; 
                if(!getline(cin,toDirectionChoice)){
                    cout<<"Error.\n";
                    continue;
                }
                
                // case comparison
                transform(toDirectionChoice.begin(), toDirectionChoice.end(), toDirectionChoice.begin(), ::toupper);

                Direction toExitDir;
                //determine room to connect back to
                // if user input is "N" assign North enum value to exitDir
                if (toDirectionChoice=="N") toExitDir = NORTH;
                else if(toDirectionChoice=="E") toExitDir=EAST;
                else if(toDirectionChoice=="S") toExitDir=SOUTH;
                else if(toDirectionChoice=="W") toExitDir=WEST;
                else {
                    cout<<"Invalid direction.\n";
                    continue;
                }
                
                // create new room with (name, description)
                // newRoom ptr holds address of this room
                Room* newRoom = new Room("NewRoom","This room is just waiting for shape and form");
                // connects currentRoom to newRoom in specified directions
                // exitDir direction from currentRoom to newRoom
                // toExitDir direction from newRoom back to currentRoom
                if(currentRoom->connect(exitDir,newRoom,toExitDir)) {
                    // adds newly created room to 'rooms' map, using current 'roomID'
                    // roomID is incremented after assignment to ensure uniqueness for next room
                    // referenced post incrementing from geeksforgeeks.com
                    // if roomID starts at 5 then first value is 5 goes to 6
                    rooms[roomID++] = newRoom;
                    cout<<"Room connection ok\n";
                } else {
                    cout<<"Room connection failed.\n";
                    delete newRoom; // free memory allocated for the new room to avoid a memory leak
                }
            }
        } else if (cmdLower=="c") {
            // handle the "connect" command to link two rooms together
            if(!builderMode){
                cout<<"Builder mode is OFF.\n";
            }
            else {
                cout<<"Connect new room in which direction? (N,S,E,W):              <<<< ";
                string directionChoice;
                if(!getline(cin,directionChoice)){
                    cout<<"Error.\n";
                    continue;
                    // if input fails, skip next iteration
                }
                // case comparison convert lowercase to uppercase
                transform(directionChoice.begin(), directionChoice.end(), directionChoice.begin(), ::toupper);

                // map input to directions
                // match user input (n,s, etc. to enum direction value)
                Direction exitDir;
                if(directionChoice=="N") exitDir=NORTH;
                else if(directionChoice=="E") exitDir=EAST;
                else if(directionChoice=="S") exitDir=SOUTH;
                else if(directionChoice=="W") exitDir=WEST;
                else {
                    cout<<"Invalid direction.\n";
                    continue;
                    // handle invalid direction input and return to command loop
                }

                if((exitDir==NORTH&&currentRoom->north())||
                   (exitDir==EAST&&currentRoom->east())||
                   (exitDir==SOUTH&&currentRoom->south())||
                   (exitDir==WEST&&currentRoom->west())) {
                    cout<<"Exit already exists!\n";
                    continue;
                    // prevent duplicate exits in same direction
                }
                
                // display all existing rooms with their IDS and names 
                cout<<"Connect this room to which room? Choose a #: \n";
                // iterate through the rooms map and display each rooms ID and name
                for (auto& [id,rm] : rooms) {
                    cout << id << ": " << rm->name() << "\n";
                }
                // input failure
                string userChoice;
                if(!getline(cin,userChoice)){
                    cout<<"Error.\n";
                    continue;
                }
                // convert user input to an integer for room selection
                // string to an integer ('rc') representing room ID
                int rc = atoi(userChoice.c_str());
                if(rooms.find(rc)==rooms.end()){
                    cout<<"Invalid room.\n";
                    continue; 
                }
                // get room associated with entered room ID
                Room* target=rooms[rc];

                cout<<"Which exit of the new room leads to this room? (N,S,E,W):    <<<< ";
                if(!getline(cin,userChoice)){
                    cout<<"Error.\n";
                    continue;
                }
                // conver input direction to uppercase for consistency
                transform(userChoice.begin(), userChoice.end(), userChoice.begin(), ::toupper);
                // map input direction to 'direction' enum value
                Direction toExitDir;
                if(userChoice=="N") toExitDir=NORTH;
                else if(userChoice=="E") toExitDir=EAST;
                else if(userChoice=="S") toExitDir=SOUTH;
                else if(userChoice=="W") toExitDir=WEST;
                else {
                    cout<<"Invalid direction.\n";
                    continue;
                }

                if(currentRoom->connect(exitDir,target,toExitDir)) {
                    cout<<"Room connection ok\n";
                } else {
                    cout<<"Room connection failed.\n";
                }
            }
            // jump implementation
        } else if (cmdLower=="jump") {
            cout<<"Enter the room ID to jump to:                                    <<<< ";
            string jumpIDInput; // store user ID
            if(!getline(cin,jumpIDInput)){
                cout<<"Error.\n";
                continue;
            }
            // convert the users input (string to int) represent room ID
            int j=atoi(jumpIDInput.c_str());
            // check if room ID exists in 'rooms' map
            if(rooms.find(j) != rooms.end()) {
                // currentRoom updates ptr to room corresponding to entered room ID
                currentRoom=rooms[j];
                // update currentRoomID to match new rooms ID
                currentRoomID=j;
                cout<<"You have jumped to: "<<currentRoom->name()<<"\n";
            } else {
                cout<<"Invalid room ID.\n";
            }
        } else if (cmdLower=="save") {
            // Save game state
            // call 'saveGame' function to save the current game state to a file named "game_save.txt"
            saveGame(rooms,currentRoom,"game_save.txt",allObjects,allMobiles,currentRoomID);
        } else if (cmdLower=="load") {
            // Load game state
            // Call loadGame function to load game state from the file 
            loadGame(rooms,currentRoom,"game_save.txt",allObjects,allMobiles,currentRoomID);
        } else if (cmdLower=="inventory") {
            // Show what's in player's inventory
            if(playerInventory.empty()) 
            cout << "You are not carrying anything.\n";
            else {
                cout<<"You are carrying:\n";
                // Iterate through players inventory and print name of each object
                for (auto* obj: playerInventory) {
                    cout<<"  "<<obj->name<<"\n";
                }
            }
            
        // Display exits
        } else if (cmdLower=="x") {
            // displayExits function to show available exits 
            displayExits(currentRoom);
        } else if (cmdLower=="get") {
            // get <object>
            // remove any leading spaces from the remaining input string 'line'
            if(!line.empty() && line.front()==' ') line.erase(line.begin());
            if(line.empty()) {
                cout<<"Get what?\n";
            }
            else
            {
                // call attemptGet to pickup object from currentRoom
                attemptGet(currentRoom,line);
            }
        } else if (cmdLower=="drop") {
            // drop <object>
            if(!line.empty() && line.front()==' ') line.erase(line.begin());
            if(line.empty()){
                cout<<"Drop what?\n";
            }
            else
            {
                // attemptDrop function to drop the specified object in currentRoom
                attemptDrop(currentRoom,line);
            }
        } else if (cmdLower=="look") {
            // look <object/mobile>
            if(!line.empty() && line.front()==' ') line.erase(line.begin());
            if(line.empty()) {cout<<"Look at what?\n";} 
            else {
                // lookTarget function to display details about target
                lookTarget(currentRoom,line);
            }
        } else if (cmdLower=="n"||cmdLower=="e"||cmdLower=="s"||cmdLower=="w") {
            // Move in that direction
            // declare ptr 'next' to hold the room in specified direction
            Room* next=nullptr;
            // determine which direction the user wants to move and set `next` to the room in that direction
            if(cmdLower=="n") next=currentRoom->north();
            else if(cmdLower=="e") next=currentRoom->east();
            else if(cmdLower=="s") next=currentRoom->south();
            else if(cmdLower=="w") next=currentRoom->west();

            // If a valid room exists in the chosen direction, update the `currentRoom` pointer to that room
            if(next){
                currentRoom=next;
                // update currentRoomID to reflect new rooms ID
                for (auto&[id,rm]:rooms){
                    if (rm==currentRoom){
                        currentRoomID=id;
                        break;
                    }
                }
                lookRoom(currentRoom);
            } else {
                cout<<"You can't go that way.\n";
            }
        } else {
            cout<<"Invalid command. Type '?' for help.\n";
        }

        // After command, mobiles move/follow
        // iterate through all mobiles someMob in the game using allMobiles vector
        // someMob is a ptr to each mobile object 
        for (auto* someMob: allMobiles) {
            // store the current room of the mobile before it potentially moves
            // oldRoom keeps track of where the mobile was
            Room* oldRoom = someMob->currentRoom;
            // check if mobile is set to follow another mobile folowing and if it is allowed to follow canFollow
            if (someMob->following && someMob->canFollow) {
                // store the mobiles current room before following to check if it changes
                Room* old=someMob->currentRoom;
                // followTarget method to move the mobile to the room it is following
                someMob->followTarget();
                // check if the mobiles room has changed after following its target
                if(someMob->currentRoom!=old){
                    // if the mobile has entered the players current room
                    if(someMob->currentRoom==currentRoom) {
                        // print message that mobile has arrived in players current room, following target
                        cout<<someMob->name<<" arrives, following "<<someMob->following->name<<".\n";
                    } else if (old==currentRoom) { // if mobile leaves current players current room while following
                        // print message that mobile is leaving to follow its target
                        cout<<someMob->name<<" chases after "<<someMob->following->name<<".\n";
                    }
                }
            // Rand action logic
            // if mobile is not following another entity, it decides to either pick/drop an obj or move rand
            } else {
                // 20% chance mob tries to pick/drop objects, else moves randomly
                // check if the mobile is capable of picking up objects canPickupObjects and if it randomly chooses to do so (20% chance)
                if(someMob->canPickupObjects && (rand()%100<20)) {
                    // call tryPickupOrDrop method to handle picking up or dropping objects
                    someMob->tryPickupOrDrop();
                } else { // move randomly to connected room 
                    // store the current room of the mobile before it moves randomly
                    Room* old=someMob->currentRoom;
                    // call moveRandomly method to move the mobile to a random connected room 
                    someMob->moveRandomly();
                    
                    // check if mobiles room has changed after moving randomly
                    if(someMob->currentRoom!=old){
                        // if mobile leaves players current room
                        if(old == currentRoom){
                            cout<<someMob->name<<" leaves the room.\n";
                            // if mobile enters players current room
                        } else if (someMob->currentRoom==currentRoom) {
                            cout<<someMob->name<<" arrives.\n";
                        }
                    }
                }
            }
        }
    }

    // Cleanup deletes all rooms and objects and mobiles 
    for (auto& [id, rm] : rooms) delete rm; // delete all room objects stored in the 'rooms' map
    for (auto* o: allObjects) delete o; // delete all objects instances in the 'allObjects' vector
    for (auto* m: allMobiles) delete m; // delete all mobile instances stored in the allMobiles vector
    return 0; 
}
