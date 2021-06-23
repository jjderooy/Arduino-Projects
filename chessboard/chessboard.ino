// Board origin is bottom left square. It is black.
// The opponent sits across from you. ie. pawn rows are horizontal.
// The opponent is white

enum Name : byte { Pawn, Rook, Knight, Bishop, Queen, King };

enum Color : int { Black = 1, White = -1};

char display_chars_white[6] { 'P', 'R', 'N', 'B', 'Q', 'K' };
char display_chars_black[6] { 'p', 'r', 'n', 'b', 'q', 'k' };

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
        this->pieces = {
            
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
        // 64 + null terminator, all set to _
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
        Serial.println();
        for (byte i = 0; i < 64; i++) {

            // Every 8th char add a new line
            if(i % 8 == 0){
                Serial.print("\n");
                row++;
            }

            if(i % 8 == 0){
                Serial.print(String(8 - row) + "   ");
            }

            Serial.print(' ');
            Serial.print(board_string[64 - (row*8) + (i%8)]);
        }
        Serial.println("\n\n 0 1 2 3 4 5 6 7\n\n\n\n");
    }

    // Returns true if the move is valid. Does not make the move.
    // Assumes the move is on the chessboard.
    bool valid_move(byte curr_x, byte curr_y, byte new_x, byte new_y){
        
        // Point p at the piece that will be moved.
        Piece *p = occupied(curr_x, curr_y);
        
        // Get our increments for movment. Used by no_collisions()
        // Increments will be normalized to + or -1 depending on direction
        // If no movement occurs, inc = 0
        int inc_x = new_x - curr_x;
        int inc_y = new_y - curr_y;

        // Check if the movement is a proper diagonal,
        // horizontal, or vertical movement. Knights are exempt
        if(((abs(inc_x) > abs(inc_y) && inc_y != 0) || 
            (abs(inc_y) > abs(inc_x) && inc_x != 0)) &&
            p->name != Knight){
               Serial.println("Invalid slope");
               return false;
           }

        // Normalize increments to magnitude of 1
        // if inc is 0, set inc to 0 (avoiding div by 0)
        // Woo ternary operators
        inc_x = inc_x ? inc_x/abs(inc_x) : 0;
        inc_y = inc_y ? inc_y/abs(inc_y) : 0;

        // Not moving is invalid
        if(!inc_x && !inc_y){
            Serial.println("Piece must move at least 1 square");
            return false;
        }

        bool no_col = no_collisions(curr_x, curr_y, new_x, new_y, inc_x, inc_y);

        // Each Piece has a set of possible moves.
        switch(p->name){
            case(Pawn):
                Serial.println("Entered pawn case");
                
                // Moving one square forward. Recall Black = 1, White = -1
                if(inc_x == 0 && new_y - curr_y == p->color && no_col){
                    Serial.println("One square forward movement");
                    break;
                }

                // Pawn can move 2 squares forward if:
                    // Black && curr_y = 1
                    // White && curr_y = 6
                if(inc_x == 0 && (new_y - curr_y) == 2*p->color && 
                   curr_y == (p->color == Black ? 1 : 6) && no_col){
                    Serial.println("Two square forward movement");
                    break;
                }

                // Pawn can move diagonal if:
                    // Diagonal square is occupied
                    // (Next section will catch the color)
                if(abs(inc_x) == 1 && inc_y == p->color &&
                   occupied(new_x, new_y)){
                    Serial.println("Diagonal take movement");
                    break;
                }

                Serial.println("Invalid pawn movement");
                return false;
                
            case(Rook):
                Serial.println("Entered rook case");

                // If x and y inc are both non zero, it's invalid (diagonal)
                if((inc_x && inc_y) || !no_col) {
                    Serial.println("Invalid rook movement");
                    return false;
                }
                break;

            case(Knight):
                Serial.println("Entered knight case");

                if(!valid_knight(curr_x, curr_y, new_x, new_y)){
                    Serial.println("Invalid knight movment");
                    return false;
                }
                break;

            case(Bishop):
                Serial.println("Entered bishop case");

                // Invalid if x and y inc are diff (not diagonal)
                if(abs(inc_x) - abs(inc_y) || !no_col) {
                    Serial.println("Invalid bishop movement");
                    return false;
                }
                break;

            case(Queen):
                Serial.println("Entered queen case");

                // Queen can move anywhere as long as nothing is in the way
                if(!no_col) {
                    Serial.println("Invalid queen movement");
                    return false;
                }
                break;

            case(King):
                Serial.println("Entered king case");

                // King can move anywhere as long as nothing is in the way
                // and movement is not more than 1 square
                // Don't need to check collisions because only moving 1
                if(abs(new_x - curr_x) > 1 && abs(new_y - curr_y) > 1){
                    Serial.println("Invalid king movement");
                    return false;
                }
                break;
        }
        
        // Check if the move the put mover in check
        if(check(p->color)){
            Serial.println("Move puts mover in check");
            return false;
        }

        // Check if a piece was taken
        Piece *occ = occupied(new_x, new_y);

        // Nothing there? It's a valid move
        if(occ == nullptr){
            Serial.println("Valid move");
            return true;
        }

        // Can't take your own color
        else if(occ->color == p->color){
            Serial.println("Cannot take own color");
            return false;
        }
        // "Take" the piece and move it off the board
        else{
            Serial.println("Took opponents piece at: " + String(occ->X) + String(occ->Y));
            occ->X = 100;
            occ->Y = 100;
            return true;
        }
    }

    // Moves the piece
    void move(byte curr_x, byte curr_y, byte new_x, byte new_y){
        
        Piece *p = occupied(curr_x, curr_y);

        p->X = new_x;
        p->Y = new_y;
    }
    

    private:

    bool valid_knight(byte curr_x, byte curr_y, byte new_x, byte new_y){
        
        // Knights must move at least 3 squares (component wise)
        if(abs(curr_x - new_x) + abs(curr_y - new_y) != 3){
            Serial.println("Invalid knight movement");
            return false;
        }

        return true;
    }

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
                Serial.println("Piece in the way at: " + String(curr_x) + String(curr_y));
                return false;
            }
        }

        Serial.println("Linear move has no collisions");
        return true;
    }

    // Use valid_move, to check every single move to the king of color c.
    // If any move is valid, this returns true
    bool check(Color c){

        Piece *p = &pieces.pieces[0];
        Piece *k = p;

        // Point k at the king of color c
        while(k->name != King || k->color != c)
            k++;

        Serial.println("Found king at: " + String(k->X) + String(k->Y)); 

        for(int i = 0; i < 32; i++){
            if(p->color == c && valid_move(p->X, p->Y, k->X, k->Y)){
                Serial.println("Check");
                return true;
            }
        }
        return false;
    }
    
    // Returns a pointer to any piece that occupies the specified square
    // Else return nullptr
    Piece* occupied(byte x, byte y){

        Piece *p = &this->pieces.pieces[0];
        
        for(int i = 0; i < 32; i++){
            if(p->X == x && p->Y == y)
                return p;
            p++;
        }
        return nullptr;
    }
};

Board b;

void setup(){
    Serial.begin(9600);
}

void loop(){
    delay(1000);
    b.print_board();

    String s; // for incoming serial data

    // Make moves by keyboard CTRL + S + P to send move
    while(true){
        if (Serial.available() > 0) {
            // read the incoming bytes:
            s = Serial.readString();

            int int_string[4];
            for (int i = 0; i < 4; i++){
                int_string[i] = s[i] - '0';
                Serial.print(int_string[i]);
            }
            Serial.println();
            
            if(b.valid_move(int_string[0],int_string[1],int_string[2],int_string[3])){
               b.move(int_string[0],int_string[1],int_string[2],int_string[3]);
            }
            b.print_board();
        }
    }
}