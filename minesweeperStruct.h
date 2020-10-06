typedef struct {
    bool isVisible;
    bool isMine;
    int numMinesAround;
    bool flag;
} tile;

typedef struct {
    bool flag;
    int row;
    int col;
} turnInput;