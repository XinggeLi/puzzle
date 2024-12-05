// Project Identifier: A8A3A33EF075ACEF9B08F5B9845569ECCB423725
#include <vector>
#include <deque>
#include <getopt.h>
#include <iostream>
using namespace std;

struct Coordinates {
    char color;
    uint32_t row;
    uint32_t col;
};

uint32_t charToRoom(char ch) {
    if (ch == '^') {
        return 0; 
    }
    else {
        return static_cast<uint32_t>(ch - 'a') + 1;
    }
}

char roomToChar(uint32_t in) {
    if (in == 0) {
        return '^'; 
    }
    else {
        return static_cast<char>('a' + in - 1);
    }
}

class Puzzle {
 private:
    int layers;
    uint32_t rows, columns;
    vector<vector<vector<char>>> map;
    vector<vector<char>> puzz;
    deque<Coordinates> container; 
    uint32_t tRow, tCol, sRow, sCol;
    char policy;
    char Color = '^';
    char format = 'm';

    bool searchTarget() {
        while (!container.empty()) {
            Coordinates sear;
            if (policy == 'q') {
                sear = container.front();
                container.pop_front();
            }
            else {
                sear = container.back();
                container.pop_back();
            }
            Color = sear.color;
            char color = puzz[sear.row][sear.col];

            if (color >= '^' && color != Color) {
                checkLoca(color, sear.row, sear.col, Color);
            }
            else {
                if (sear.row > 0) {
                    checkLoca(Color, sear.row - 1, sear.col, '2');
                    if (sear.row - 1 == tRow && sear.col == tCol) {
                        return true;
                    }
                }

                if (sear.col < columns - 1 && columns > 1) {
                    checkLoca(Color, sear.row, sear.col + 1, '3');
                    if (sear.row == tRow && sear.col + 1 == tCol) {
                        return true;
                    }
                }

                if (sear.row < rows - 1 && rows > 1) {
                    checkLoca(Color, sear.row + 1, sear.col, '4');
                    if (sear.row + 1 == tRow && sear.col == tCol) {
                        return true;
                    }
                }

                if (sear.col > 0) {
                    checkLoca(Color, sear.row, sear.col - 1, '5');
                    if (sear.row == tRow && sear.col - 1 == tCol) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void checkLoca(char l, uint32_t r, uint32_t c, char d) {
        if ((puzz[r][c] == '?' || puzz[r][c] == '@' || puzz[r][c] == '.' || puzz[r][c] >= '^' 
                        || l - puzz[r][c] == 32) && map[charToRoom(l)][r][c] == '0') {
            Coordinates coor;
            coor.color = l;
            coor.row = r;
            coor.col = c;
            container.push_back(coor);
            map[charToRoom(l)][r][c] = d;
        }
    }  
    
    bool checkLoca2(uint32_t r, uint32_t c) {
        for (uint32_t i = 0; i <= static_cast<uint32_t>(layers); i++) {
            if (map[i][r][c] != '0') {
                return false;
            }
        }
        return true;
    }
    
    void traceMap(Coordinates &last, char &trace) {
        uint32_t co = charToRoom(last.color);
        if (trace == '2') {
            last.row++;
            trace = map[co][last.row][last.col];
            map[co][last.row][last.col] = '+';
        }
        else if (trace == '3') {
            last.col--;
            trace = map[co][last.row][last.col];
            map[co][last.row][last.col] = '+';
        }
        else if (trace == '4') {
            last.row--;
            trace = map[co][last.row][last.col];
            map[co][last.row][last.col] = '+';
        }
        else if (trace == '5') {
            last.col++;
            trace = map[co][last.row][last.col];
            map[co][last.row][last.col] = '+';
        }
        else {
            last.color = trace;
            trace = map[charToRoom(trace)][last.row][last.col];
            map[co][last.row][last.col] = '@';
            map[charToRoom(last.color)][last.row][last.col] = '%';
        }
    }

    void traceList(Coordinates &last) {
        uint32_t co = charToRoom(last.color);
        if (map[co][last.row][last.col] == '2') {
            last.row++;
        }
        else if (map[co][last.row][last.col] == '3') {
            last.col--;
        }
        else if (map[co][last.row][last.col] == '4') {
            last.row--;
        }
        else if (map[co][last.row][last.col] == '5') {
            last.col++;
        }
        else {
            last.color = map[co][last.row][last.col];
        }
    }

    bool checkChar(const char &c) {
        if (c != '@' && c != '#' && c != '.' && c != '^' && c != '?') {
            for (uint32_t i = 0; i <= static_cast<uint32_t>(layers); i++) {
                if (c == roomToChar(i) || roomToChar(i) - c == 32) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

 public:
    
    Puzzle() {}

    void get_opt(int argc, char** argv) {
        option longOpts[] = {{"help", no_argument, nullptr, 'h'},
                                    {"queue", no_argument, nullptr, 'q'},
                                    {"stack", no_argument, nullptr, 's'},
                                    {"output", required_argument, nullptr, 'o'},
                                    { nullptr, 0, nullptr, '\0' }};
        int choice, option_index;
        bool qs = false;
        string arg;
        while ((choice = getopt_long(argc, argv, "hqso:", longOpts, &option_index)) != -1) {
            
            switch (choice) {
                case 'q':
                    if (qs) {
                        cerr << "both q and s\n";
                        exit(1);
                    }
                    policy = 'q';
                    qs = true;
                    break;
                case 's':
                    if (qs) {
                        cerr << "both q and s\n";
                        exit(1);
                    }
                    policy = 's';
                    qs = true;
                    break;
                case 'o':
                    arg = optarg;
                    if (arg == "map") {
                        format = 'm';
                    }
                    else if (arg == "list") {
                        format = 'l';
                    }
                    else {
                        cerr << "invalid output choice\n";
                        exit(1);
                    }
                    break;
                case 'h':
                    cout << "This program reads a CVS file that contains a puzzle,\n"
                         << "then tries to solve the puzzle either by using a queue\n"
                         << "or stack(specified in the command line arguments).\n"
                         << "if a solution exist, it will output the solution in\n"
                         << "either map or list(specidied in the command line).\n"
                         << "Otherwise, it will output no solution.\n"
                         <<  "Usage: \'./project1\n\t[--quene | -q]\n"
                         <<                      "\t[--stack | -s]\n"
                         <<                      "\t[--output | -o] <map or list>\n"
                         <<                      "\t[--help | -h]\n"
                         <<                      "\t< <CSV Puzzle File>\'" << endl;
                    exit(0);
                default:
                    cerr << "invalid command line argument\n";
                    exit(1);
            }
        }
        if (qs == false) {
            cerr << "did not provide mode\n";
            exit(1);
        }
    }

    void readMap(){
        cin >> layers >> rows >> columns;
        if (layers < 0 || layers > 26 || rows < 1 || columns < 1) {
            exit(1);
        }

        puzz.resize(rows, vector<char>(columns, '0'));
        map.resize(layers + 1, vector<vector<char>>(rows, vector<char>(columns, '0')));
        char c;
        uint32_t i = 0;
        while(cin >> c) {
            if (c == '/') {
                string new_line;
                getline(cin, new_line);
            }
            else if (checkChar(c)) {
                cerr << "invalid character\n";
                exit(1);
            }
            else {
                puzz[i/columns][i%columns] = c;
                int start = 0;
                int tar = 0;
                if (c == '@') {
                    if (start != 0) {
                        cerr << "double start\n";
                        exit(1);
                    }
                    start++;
                    sRow = i/(int)columns;
                    sCol = i%(int)columns;
                    checkLoca('^', sRow, sCol, '1');
                }
                else if (c == '?') {
                    if (tar != 0) {
                        cerr << "double target\n";
                        exit(1);
                    }
                    tar++;
                    tRow = i/columns;
                    tCol = i%columns;
                }
                
                ++i;
            }
        }
    }

    

    void output() {
        if (searchTarget()) {
            Coordinates last;
            last.color = Color;
            last.row = tRow;
            last.col = tCol;
            
            if (format == 'l') {
                deque<Coordinates> backTrace;
                backTrace.push_back(last);
                while(last.row != sRow || last.col != sCol || last.color != '^') {
                    traceList(last);
                    backTrace.push_back(last);
                }
                while(!backTrace.empty()) {
                    last = backTrace.back();
                    backTrace.pop_back();
                    cout << "(" << last.color << ", (" << last.row 
                         << ", " << last.col << "))\n";
                }
            }
            else {
                char trace = map[charToRoom(Color)][last.row][last.col];
                while(trace != '1') {
                    traceMap(last, trace);
                }
                map[0][sRow][sCol] = '@';
                for (int i = 0; i <= layers; i++) {
                    for (uint32_t j = 0; j < rows; j++) {
                        for (uint32_t k = 0; k < columns; k++) {
                            if (map[i][j][k] != '+' && map[i][j][k] != '@' && map[i][j][k] != '%') {
                                map[i][j][k] = puzz[j][k];
                                if (puzz[j][k] == roomToChar(i) || roomToChar(i) - puzz[j][k] == 32 
                                                              || (j == sRow && k == sCol && i > 0)) {
                                    map[i][j][k] = '.';
                                }
                            }
                        }
                    }
                }
                for (int i = 0; i <= layers; i++) {
                    cout << "// color " << roomToChar(i) << "\n";
                    for (uint32_t j = 0; j < rows; j++) {
                        for (uint32_t k = 0; k < columns; k++) {
                            cout << map[i][j][k];
                        }
                        cout << "\n";
                    }
                }
            }
        }
        else {
            cout << "No solution.\n" << "Discovered:\n";
            for (uint32_t i = 0; i < rows; i++) {
                for (uint32_t j = 0; j < columns; j++) {
                    if (checkLoca2(i, j)) {
                        puzz[i][j] = '#';
                    }
                    cout << puzz[i][j];
                }
                cout << "\n";
            }
        }
    }
};


int main(int arg, char **argv) {
    ios_base::sync_with_stdio(false);
    Puzzle P1;
    P1.get_opt(arg, argv);
    P1.readMap();
    P1.output();
}