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
        this-> unmoved = true;
    }

    Name name;
    Color color;
    byte X;
    byte Y;
    bool unmoved;

    

    void move(byte new_x, byte new_y){
        this->X = new_x;
        this->Y = new_y;
    }
};

// We wrap the array in a struct so we can declare, then initialize later
typedef struct{
    Piece pieces[32];
}pieces_array;

class Board {
    public:
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
            Piece(4, 0, King, Black),
            Piece(3, 0, Queen, Black),

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
            Piece(4, 7, King, White),
            Piece(3, 7, Queen, White),
        };
    }

    // We wrap the array in a struct so we can declare
    // here and initialize in the constructor.
    pieces_array pieces; 
 
    void print_board(){
        // 64 + null terminator, all set to _
        char board_string[65]; 
        for (byte i = 0; i < 64; i++) {
            board_string[i] = '_';    
        }
        board_string[64] = '\0';
        
        // Replace the empty squares _ with pieces display chars
        for (byte i = 0; i < 32; i++) {

            // Pointer to Piece
            Piece *p = &this->pieces.pieces[i];

            // Some math to map X,Y to board_string index
            if(p->color == Black)
                board_string[(p->Y * 8) + p->X] = display_chars_black[p->name];
            else
                board_string[(p->Y * 8) + p->X] = display_chars_white[p->name];
        }
         
        for (byte i = 0; i < 64; i++) {
            
            Serial.print(' ');
            Serial.print(board_string[63-i]);
           
            // Every 8th char add a new line
            if((i + 1) % 8 == 0)
                Serial.print("\n");
        }
        
    }

    // Returns true if the move is valid. Does not make the move.
    // Could technically pass a pointer here, but in the context of
    // the Arduino it makes more sense to do an X and Y
    bool valid_move(byte curr_x, byte curr_y, byte new_x, byte new_y){
        
        Piece *p = &pieces.pieces[0];

        // Point p at the piece that will be moved.
        while(p->X != curr_x && p->X != curr_y) // This is uhh a little dangerous
            p++;
        
        // Each Piece has a set of possible moves.
        switch(p->name){
            case(Pawn):
                break;
                
            case(Rook):
                break;

            case(Knight):
                break;

            case(Bishop):
                break;

            case(Queen):
                break;

            case(King):
                break;
        }
    }
    
    // Use move, to check every single move to the king of color c.
    // If any move is valid, this returns true
    bool check(Color c){
        return false;
    }

    bool occupied(byte x, byte y){
        return false;
    }
};

Board game_board;

void setup(){
    Serial.begin(9600);
}

void loop(){
    delay(1000); // Allow for serial comms to begin.
    game_board.print_board();
    while(true){}
}