// Board origin is bottom left square. It is black.
// The opponent sits across from you. ie. pawn rows are horizontal.
// The opponent is white

enum Name : byte { Pawn = 1, Rook = 5, Knight = 3, Bishop = 4, Queen = 9, King = 0 };

enum Color : int { Black = 1, White = -1};

// This is gross but will not be needed in final version.
// Blank spaces exist so chars line up with Name enum values
char display_chars_white[10] { 'K', 'P', ' ', 'H', 'B', 'R', ' ', ' ', ' ', 'Q' };
char display_chars_black[10] { 'k', 'p', ' ', 'h', 'b', 'r', ' ', ' ', ' ', 'q' };

// Map 0-15 to the pin names of arduino
// [0,7] stores x pins, [8,15] stores y pins
// A6 and A7 are not used because they cannot function as outputs
byte btn_indices[16] = { 2, 3, 4, 5, 6, 7, 8, 9, A0, A1, A2, A3, A4, A5, 12, 13 };

// struct that stores a move
// Array is in from { curr_x, curr_y, new_x, new_y }
typedef struct{
    byte pos[4];
}Move;


// ****** ARDUINO METHODS ****** //

// Waits until the user presses two buttons to select a piece
// and two buttons to move the piece
// 1500ms delay is added between pairs of buttons to allow for debouncing
// Buttons don't have to be pressed at the same time
// TODO This should be changed so it checks the falling edge rather than delay
Move get_move(){

    set_PULLUP();

    Move m;
    
    // Init to 255. Will be set to between 0-7 below
    for(byte i = 0; i < 4; i++)
        m.pos[i] = 255;

    // First loop selects piece
    // Second selects where piece will move
    // i offsets m.pos index
    for(byte i = 0; i < 3; i += 2){ 
        do{
            // X
            for(byte j = 0; j < 8; j++){
                if(digitalRead(btn_indices[j]) == LOW)
                    m.pos[0 + i] = j;
            }

            // Y
            // TODO change the 15 to a 16 once the LED on the nano is desoldered
            // from pin 13 as it messes with this
            for(byte j = 8; j < 15; j++){
                if(digitalRead(btn_indices[j]) == LOW)
                    m.pos[1 + i] = j - 8;   
            }
            
        }while(m.pos[0 + i] == 255 || m.pos[1 + i] == 255); // Repeat until two buttons pressed

        // Once the first buttons have been pressed, delay to debounce them
        delay(1500);
    } 

    Serial.print("Got move: ");
    for(byte i = 0; i < 4; i++)
        Serial.print(m.pos[i]);
    Serial.println();

    return m;
}

// Blinks led for specifed time
void blink_LED_pair(byte pin1, byte pin2, int on, int off){
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    delay(on);
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    delay(off);
    pinMode(pin1, INPUT_PULLUP);
    pinMode(pin2, INPUT_PULLUP);
}

// Lights LEDs to select a piece, and show where to move it
void do_move_LEDs(Move m){

    // Due to the wiring, unless the LED is being lit,
    // it must be set to INPUT_PULLUP
    set_PULLUP();
    int on = 3000;
    int off = 0;

    blink_LED_pair(btn_indices[m.pos[0]], btn_indices[m.pos[1] + 8], on, off);
    blink_LED_pair(btn_indices[m.pos[2]], btn_indices[m.pos[3] + 8], on, off);
}

// Sets the pinMode of every button IO pin to INPUT_PULLUP
void set_PULLUP(){
    for(byte i = 0; i < 16; i++)
      pinMode(btn_indices[i], INPUT_PULLUP);
}


// ****** CHESSBOARD METHODS ****** //

class Piece {
    public:
    Piece() {};
    Piece(byte pos_x, byte pos_y, Name n, Color c) {
        this->name = n;
        this->X = pos_x;
        this->Y = pos_y;
        this->color = c;
    }

    Name name;
    Color color;
    byte X;
    byte Y;
};

// We wrap the array in a struct so we can declare, then initialize later
typedef struct{
    Piece pieces[32];
}pieces_array;

class Board {
    public:

    pieces_array pieces; 

    Board() {
        pieces = {
            
            // Black
            Piece(0, 1, Pawn, Black),
            Piece(1, 1, Pawn, Black),
            Piece(2, 1, Pawn, Black),
            Piece(3, 1, Pawn, Black),
            Piece(4, 1, Pawn, Black),
            Piece(5, 1, Pawn, Black),
            Piece(6, 1, Pawn, Black),
            Piece(7, 1, Pawn, Black),
            Piece(0, 0, Rook, Black),
            Piece(7, 0, Rook, Black),
            Piece(1, 0, Knight, Black),
            Piece(6, 0, Knight, Black),
            Piece(2, 0, Bishop, Black),
            Piece(5, 0, Bishop, Black),
            Piece(3, 0, King, Black),
            Piece(4, 0, Queen, Black),

            // White
            Piece(0, 6, Pawn, White),
            Piece(1, 6, Pawn, White),
            Piece(2, 6, Pawn, White),
            Piece(3, 6, Pawn, White),
            Piece(4, 6, Pawn, White),
            Piece(5, 6, Pawn, White),
            Piece(6, 6, Pawn, White),
            Piece(7, 6, Pawn, White),
            Piece(0, 7, Rook, White),
            Piece(7, 7, Rook, White),
            Piece(1, 7, Knight, White),
            Piece(6, 7, Knight, White),
            Piece(2, 7, Bishop, White),
            Piece(5, 7, Bishop, White),
            Piece(3, 7, King, White),
            Piece(4, 7, Queen, White),
        };
    }

    // Prints the board (For debugging)
    void print_board(){
        // 64 + null terminator, all set to -
        char board_string[65]; 
        for (byte i = 0; i < 64; i++)
            board_string[i] = '-';    
        board_string[64] = '\0';
        
        // Replace the empty squares _ with pieces display chars
        for (byte i = 0; i < 32; i++) {

            // Pointer to Piece
            Piece *p = &this->pieces.pieces[i];

            // Some math to map X,Y to board_string index
            if(p->X != 100) { // Piece is not off the board.
                if(p->color == Black)
                    board_string[(p->Y * 8) + p->X] = display_chars_black[p->name];
                else
                    board_string[(p->Y * 8) + p->X] = display_chars_white[p->name];
            }
        }
        
        // Spooky formatting so board prints in correct orientation
        byte row = 0;
        for (byte i = 0; i < 64; i++) {

            // Every 8th char add a new line
            if(i % 8 == 0){
                Serial.print("\n");
                row++;
            }

            if(i % 8 == 0){
                Serial.print(String(8 - row) + "  ");
            }

            Serial.print(' ');
            Serial.print(board_string[64 - (row*8) + (i%8)]);
        }
        Serial.println("\n\n    0 1 2 3 4 5 6 7");
        Serial.println("\n\n\n");
    }

    // Returns true if the move is valid. Does not make the move.
    // Assumes the move is on the chessboard.
    // Always pass layers = 0 as it is a recursive variable
    // pass test_check = true to test if a move puts the mover in check
    bool valid_move(Piece *p, byte new_x, byte new_y, byte layers, bool test_check){

        float init = micros();

        byte curr_x = p->X;
        byte curr_y = p->Y;

        // Get our increments for movement. Used by no_collisions()
        // Increments will be normalized to + or -1 depending on direction
        // If no movement occurs, inc = 0
        int inc_x = new_x - curr_x;
        int inc_y = new_y - curr_y;

        // Check if the movement is a proper diagonal,
        // horizontal, or vertical movement. Knights are exempt
        if(((abs(inc_x) > abs(inc_y) && inc_y != 0)  || 
            (abs(inc_x) < abs(inc_y) && inc_x != 0)) &&
            p->name != Knight){
               //Serial.println("Invalid slope");
               return false;
        }

        // Normalize increments to magnitude of 1
        // if inc is 0, set inc to 0 (avoiding div by 0)
        // Woo ternary operators
        inc_x = inc_x ? inc_x/abs(inc_x) : 0;
        inc_y = inc_y ? inc_y/abs(inc_y) : 0;

        // Not moving is invalid
        if(!inc_x && !inc_y){
            //Serial.println("Piece must move at least 1 square");
            return false;
        }

        bool no_col;
        if(p->name != Knight)
            no_col = no_collisions(curr_x, curr_y, new_x, new_y, inc_x, inc_y);
        else
            no_col = false;


        // Each Piece has a set of possible moves.
        switch(p->name){
            case(Pawn):
                //Serial.println("Entered pawn case");
                
                // no_collisions only checks closest square so we need to
                // check if new is occupied by anything since pawns can
                // only take diagonally

                // Moving one square forward. Recall Black = 1, White = -1
                if(inc_x == 0 && new_y - curr_y == p->color &&
                   !occupied(new_x, new_y)){
                    //Serial.println("One square forward movement");
                    break;
                }

                // Pawn can move 2 squares forward if:
                    // Black && curr_y = 1
                    // White && curr_y = 6
                if(inc_x == 0 && (new_y - curr_y) == 2*p->color && 
                   curr_y == (p->color == Black ? 1 : 6) &&
                   !occupied(new_x, new_y) && no_col){
                    //Serial.println("Two square forward movement");
                    break;
                }

                // Pawn can move one square diagonal if:
                    // Diagonal square is occupied
                    // (Next section will catch the color)
                if(abs(inc_x) == 1 && inc_y == p->color &&
                   occupied(new_x, new_y) &&
                   (abs(new_x - curr_x) + abs(new_y - curr_y)) == 2){
                    //Serial.println("Diagonal take movement");
                    break;
                }

                //Serial.println("Invalid pawn movement");
                return false;
                
            case(Rook):
                //Serial.println("Entered rook case");

                // If x and y inc are both non zero, it's invalid (diagonal)
                if((inc_x && inc_y) || !no_col) {
                    //Serial.println("Invalid rook movement");
                    return false;
                }
                break;

            case(Knight):
                //Serial.println("Entered knight case");

                // Knights must move at least 3 squares (component wise)
                // and neither component can be 0 (catches movement
                // 3 squares horizontal or vertical)
                if(abs(curr_x - new_x) + abs(curr_y - new_y) != 3 || 
                   curr_x == new_x || curr_y == new_y){
                    //Serial.println("Invalid knight movement");
                    return false;
                }
                break;

            case(Bishop):
                //Serial.println("Entered bishop case");

                // Invalid if x and y inc are diff (not diagonal)
                if(abs(inc_x) - abs(inc_y) || !no_col) {
                    //Serial.println("Invalid bishop movement");
                    return false;
                }
                break;

            case(Queen):
                //Serial.println("Entered queen case");

                // Queen can move anywhere as long as nothing is in the way
                if(!no_col) {
                    //Serial.println("Invalid queen movement");
                    return false;
                }
                break;

            case(King):
                //Serial.println("Entered king case");

                // King can move anywhere as long as movement is not 
                // more than 1 square
                // Checking collisions is handled outside switch block
                if(abs(new_x - curr_x) > 1 || abs(new_y - curr_y) > 1){
                    //Serial.println("Invalid king movement");
                    return false;
                }
                break;
        }

        // Check if a piece was taken
        Piece *tmp = occupied(new_x, new_y);

        // Can't take your own color
        if(tmp->color == p->color){
            //Serial.println("Cannot take own color");
            return false;
        }

        // For performance reasons, testing check is optional
        // The AI will only test check on moves that score better
        // than the current best move as this function is extremely
        // expensive at about 1ms of computer time
        if(test_check){
            // Check if the move put the mover in check
            tmp = get_king(p->color);

            // Move, test check, then unmove
            Piece *taken = move(p, new_x, new_y);

            if(layers < 1 && check(tmp->X, tmp->Y, layers)){
                //Serial.println("Move puts mover in check");

                // Illegal move. Move the piece(s) back
                move(p, curr_x, curr_y);
                move(taken, new_x, new_y);
                return false;
            }

            move(p, curr_x, curr_y);
        }

        //Serial.println("Move validation took " + String((float(micros()) - init) / 1000) + "ms");

        // If we made it here, the move is valid!
        return true;
    }

    // Moves the piece and take opponent's piece if specifed,
    // and returns a pointer to the take piece (so it can be unmoved)

    // take param only exists for when checking check as we don't
    // actually want to take pieces.
    Piece* move(Piece *p, byte new_x, byte new_y){
        
        // Occasionally when testing check in valid_move()
        // a nullptr will be passed here. In that case do nothing
        if(p == nullptr)
            return nullptr;

        // Check for piece in the target square
        Piece *occ = occupied(new_x, new_y);

        // "Take" the piece if it exists and move it off the board
        if(occ){
            //Serial.println("Took opponents piece at: " + String(occ->X) + String(occ->Y));
            occ->X = 100;
            occ->Y = 100;
        }

        // Move the piece to the new location
        p->X = new_x;
        p->Y = new_y;

        // Return the pointer to the taken piece. This lets the user "unmove"
        // the piece if needed. This is esp important in the AI where many
        // moves are made and need to be reverted
        return occ;
    }
    
    // Returns a pointer to any piece that occupies the specified square
    // Else return nullptr
    Piece *occupied(byte x, byte y){

        Piece *p = &this->pieces.pieces[0];
        
        for(int i = 0; i < 32; i++){
            if(p->X == x && p->Y == y)
                return p;
            p++;
        }
        return nullptr;
    }

    // Add up the values of all the pieces Black > 0, White < 0
    // score > 0 means black is winning
    // score < 0 means black is losing
    // score * Color gives relative score
    int score_board(){
        float init = micros();
        int score = 0;
        Piece *p = &pieces.pieces[0];

        for(byte i = 0; i < 32; i++){

            // Check if piece is on the board
            if(p->X < 32)
                score += int(p->name) * p->color;
            p++;
        }
        //Serial.println("Board scoring took " + String((float(micros()) - init) / 1000) + "ms");
        return score;
    }

    // Use valid_move, to check every single move to the kings square
    // If any move is valid, this returns true
    // This ends up being recursive so we need to pass the recursive depth
    bool check(byte king_x, byte king_y, byte layers){

        Piece *p = &pieces.pieces[0];

        for(int i = 0; i < 32; i++){
            if(valid_move(p, king_x, king_y, layers + 1, true)){ 
                //Serial.println("Check");
                return true;
            }
            p++;
        }
        return false;
    }

    // Returns a pointer to king of color c
    Piece *get_king(Color c){
        // Point k at the king of color c
        Piece *k = &pieces.pieces[0];

        while(k->name != King || k->color != c)
            k++;
        //Serial.println("Found king at: " + String(k->X) + String(k->Y)); 

        return k;
    }

    private:

    // Move from curr to new using the inc provided
    // If any pieces are encountered in the way, return false
    // Assumes piece moves at least one square
    bool no_collisions(byte curr_x, byte curr_y, 
                           byte new_x, byte new_y, 
                           int inc_x, int inc_y){

        // Subtract inc so we only move to the square before new
        while(curr_x != new_x - inc_x || curr_y != new_y - inc_y){
            
            // Increment first so piece doesn't collide with itself 
            // Only increment if we aren't already there
            curr_x += inc_x*(curr_x != new_x);
            curr_y += inc_y*(curr_y != new_y);

            if(occupied(curr_x, curr_y)){
                //Serial.println("Piece in the way at: " + String(curr_x) + String(curr_y));
                return false;
            }
        }

        //Serial.println("Linear move has no collisions");
        return true;
    }

};

class AI {
    public:

    Color c;

    AI(Color color){
        c = color;
    }

    // brd  : node (initially the state of the live board)
    // depth: maximum number of turns before returning a best move
    // clr  : color of player (AI) making the move
    // nodes: keeps track of how many moves are made by the AI
    int negamax(Board brd, byte depth, Color clr, long *nodes){
        /*
            https://en.wikipedia.org/wiki/Negamax
            function negamax(node, depth, color) is
                if depth = 0 or node is a terminal node then
                    return color × the heuristic value of node
                value := −∞
                for each child of node do
                    value := max(value, negamax(child, depth − 1, −color))
                return −value
        */

        // Also need to check here for stalemate / checkmate
        if(depth == 0)
            return clr*brd.score_board();

        int value = INT16_MIN;

        // Each child node is every possible legal Board that can be made
        // by moving a single piece of Color clr
        // We loop through every piece of Color clr and move it to every square
        // on the board, checking each time if it is legal 

        // Board's pieces array, [0,15] = Black, [16,31] = White
        // clr == Black -> i =  0, till i = 15
        // clr == White -> i = 16, till i = 31
        byte end_index = 16 + 16*(clr == White); 
        for(byte i = 16*(clr == White); i < end_index; i++){
            
            // Pointer to the start of the Pieces of Color clr in Board brd
            Piece *p = &brd.pieces.pieces[i];
            
            for(byte x = 0; x < 8; x++){
                for(byte y = 0; y < 8; y++){
                    (*nodes)++;
                    if(brd.valid_move(p, x, y, 0, false)){
                        byte orig_x = p->X;
                        byte orig_y = p->Y;

                        // Pointer to any piece taken when moving p
                        Piece *taken = brd.move(p, x, y);
                        
                        // The algo doesn't call for the multiplication by
                        // clr here but it doesn't make sense without it.
                        // Possibly an implementation error, but it works
                        int ngm = clr*negamax(brd, depth - 1, Color(-clr), nodes);

                        // To save on compute time, only test check if
                        // the move is better than the current move
                        // This is because testing check is very expensive
                        Piece *king = brd.get_king(clr);
                        if(ngm > value && !brd.check(king->X, king->Y, 0))
                            value = ngm;

                        // Unmove p
                        brd.move(p, orig_x, orig_y);

                        // Move any Piece p took back to where it came
                        brd.move(taken, x, y);
                    }
                }
            }
        }

        return -value;
    }

    // Returns the best Move using the negamax algo given a depth
    Move get_best_move(Board brd, byte depth){
         
        // This is basically the same as negamax(), but instead of finding
        // best values, we finding the best Move using negamax()

        // We evaluate negamax() on each piece of Color c, and
        // pick the piece with the highest return value

        float init = micros();

        // Passed to negamax as pointer. Each move is one node of the
        // tree that negamax searches
        long nodes = 0;

        int value = INT16_MIN;
        Move best;

        byte end_index = 16 + 16*(c == White); 
        for(byte i = 16*(c == White); i < end_index; i++){

            Piece *p = &brd.pieces.pieces[i];
            
            for(byte x = 0; x < 8; x++){
                for(byte y = 0; y < 8; y++){
                    nodes++;
                    if(brd.valid_move(p, x, y, 0, false)){

                        byte orig_x = p->X;
                        byte orig_y = p->Y;

                        // Pointer to any piece taken when moving p
                        Piece *taken = brd.move(p, x, y);

                        // Call negamax to get the max value of this initial
                        // starting with this initial move
                        int ngm = negamax(brd, depth, Color(-c), &nodes);

                        // Update best if a equal or better move was found
                        // Here we use millis to add some "randomness"
                        // if two moves of the same value are found
                        //Serial.println("Initial move: ");
                        //brd.print_board();
                        if(ngm > value || (ngm == value && millis() % 7 == 0)){
                            value = ngm;
                            best.pos[0] = orig_x;
                            best.pos[1] = orig_y;
                            best.pos[2] = x;
                            best.pos[3] = y;

                            // Blink the top left LED so player knows AI is "thinking"
                            blink_LED_pair(btn_indices[15], btn_indices[15], 50, 0);

                            // Debugging
                            Serial.print("New best move: ");
                            Serial.println("value: " + String(value) + " ngm: " + String(ngm));
                            for(byte i = 0; i < 4; i++)
                                Serial.print(String(best.pos[i]) + " ");
                            Serial.println();
                        }
                        // Unmove p
                        brd.move(p, orig_x, orig_y);

                        // Move any Piece p took back to where it came
                        brd.move(taken, x, y);
                    }
                }
            }
        }

        Serial.println("Beth made " + String(nodes) + " moves" + " in " +
                        String((float(micros()) - init) / 1000) + "ms");

        return best;
    }
};  

Board b;
AI beth(White);


void setup(){
    Serial.begin(115200);
    b.print_board();
}

void loop(){

    set_PULLUP();

    // Get a legal move from the player
    Move m;
    Piece *p;
    do{
        m = get_move();
        p = b.occupied(m.pos[0], m.pos[1]);
    }while(!b.valid_move(p, m.pos[2], m.pos[3], 0, true));

    // Make the player's move
    b.move(p, m.pos[2], m.pos[3]);
    do_move_LEDs(m);

    Serial.println("\n\nMaking Beths move...");
    Move beths_move = beth.get_best_move(b, 2);
    p = b.occupied(beths_move.pos[0], beths_move.pos[1]);
    b.move(p, beths_move.pos[2], beths_move.pos[3]);
    do_move_LEDs(beths_move);

    b.print_board();

    //String s; // For incoming serial data

    //// Make moves by keyboard CTRL + S + P to send move
    //while(true){
        //if (Serial.available() > 0) {

            //// Read the incoming bytes:
            //s = Serial.readString();
            
            //int int_string[4];

            //// Print the move
            //for (int i = 0; i < 4; i++){
                //int_string[i] = s[i] - '0';
                //Serial.print(int_string[i]);
            //}
            //Serial.println();
            
            //Piece *p = b.occupied(int_string[0],int_string[1]);

            //// Make the move
            //if(b.valid_move(p,int_string[2],int_string[3], 0))
               //b.move(p, int_string[2],int_string[3], true);
            
            //b.print_board();

            //Serial.println("\n\nMaking Beths move...");

            //Move beths_move = beth.get_best_move(b, 2);
            //p = b.occupied(beths_move.pos[0], beths_move.pos[1]);
            //b.move(p, beths_move.pos[2], beths_move.pos[3], true);

            //b.print_board();
        //}
    //}
}
