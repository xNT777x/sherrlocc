/*
* Ho Chi Minh City University of Technology
* Faculty of Computer Science and Engineering
* Initial code for Assignment 1
* Programming Fundamentals Spring 2023
* Author: Vu Van Tien
* Date: 02.02.2023
*/

//The library here is concretely set, students are not allowed to include any other libraries.
#ifndef _H_STUDY_IN_PINK_2_H_
#define _H_STUDY_IN_PINK_2_H_

#include "main.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Forward declaration
// class MovingObject;
// class Position;
// class Configuration;
// class Map;

// class Criminal;
// class RobotS;
// class RobotW;
// class RobotSW;
// class RobotC;

// class ArrayMovingObject;
// class StudyPinkProgram;

// class BaseItem;
// class BaseBag;
// class SherlockBag;
// class WatsonBag;

class TestStudyInPink;

enum ItemType { MAGIC_BOOK, ENERGY_DRINK, FIRST_AID, EXCEMPTION_CARD, PASSING_CARD };
enum ElementType { PATH, WALL, FAKE_WALL };
enum RobotType { C=0, S, W, SW };

class MapElement {
protected:
    ElementType type;
public:
    MapElement(ElementType in_type) : type(in_type) {};
    virtual ~MapElement() {};
    virtual ElementType getType() const {
        return type;
    }
};

class Path : public MapElement {
public:
    Path() : MapElement(PATH) {};
};

class Wall : public MapElement {
public:
    Wall() : MapElement(WALL) {};
};

class FakeWall : public MapElement {
private:
    int req_exp;

public:
    FakeWall(int in_req_exp) : MapElement(FAKE_WALL), req_exp(in_req_exp) {};
    int getReqExp() const {
        return req_exp;
    }
};

class Map {
private:
    int num_rows, num_cols;
    MapElement*** map;

public:
    Map(int num_rows, int num_cols, int num_walls, Position* array_walls, int num_fake_walls, Position* array_fake_walls) : num_rows(num_rows), num_cols(num_cols) {
        map = new MapElement**[num_rows];

        for (int i = 0; i < num_rows; i++) {
            map[i] = new MapElement * [num_cols];
            for (int j = 0; j < num_cols; j++) {
                map[i][j] = new Path();
            }
        }

        for (int i = 0; i < num_walls; i++) {
            int wall_row = array_walls[i].getRow();
            int wall_col = array_walls[i].getCol();

            delete map[wall_row][wall_col];
            map[wall_row][wall_col] = new Wall();
        }

        for (int i = 0; i < num_fake_walls; i++) {
            int fw_row = array_fake_walls[i].getRow();
            int fw_col = array_fake_walls[i].getCol();
            int in_req_exp = (fw_row * 257 + fw_col * 139 + 89) % 900 + 1;

            delete map[fw_row][fw_col];
            map[fw_row][fw_col] = new FakeWall(in_req_exp);
        }
    };
    ~Map() {
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                delete map[i][j];
            }
            delete[] map[i];
        }
        delete[] map;
    }
    bool isValid(const Position& pos, MovingObject* mv_obj) const {
        int pos_row = pos.getRow();
        int pos_col = pos.getCol();

        MapElement* element = map[pos_row][pos_col];

        if (pos_row < 0 || pos_col < 0 || pos_row >= num_rows || pos_col >= num_cols) {
            return false;
        }
        if (element->getType() == WALL) {
            return false;
        }
        if (element->getType() == FAKE_WALL) {
            FakeWall* fake_wall = dynamic_cast<FakeWall*>(element);
            if (fake_wall && mv_obj->getName() == "Watson" && mv_obj->getExp() < fake_wall->getReqExp()) return false;
        }
        return true;
    }

    int getNumRows() const {
        return num_rows;
    }
    int getNumCols() const {
        return num_cols;
    }
};

class Position {
private:
    int r, c;
public:
    static const Position npos = Position(-1,-1);

    Position(int r = 0, int c = 0) : r(r), c(c) {
    };

    Position(const string& str_pos) {
        sscanf(str_pos.c_str(), "(%d,%d)", &r, &c);
    };

    int getRow() const {
        return r;
    }
    int getCol() const {
        return c;
    }
    void setRow(int r) {
        this->r = r;
    }
    void setCol(int c) {
        this->c = c;
    }

    string str() const {
        return "(" + to_string(r) + "," + to_string(c) + ")";
    }

    bool isEqual(int in_r, int in_c) const {
        return (in_r == this->r && in_c == this->c);
    }
};

class MovingObject {
protected:
    int index;
    Position pos;
    Map * map;
    string name;

public:
    MovingObject(int index, const Position pos, Map* map, const string& name = "") : index(index), pos(pos), map(map), name(name) {};
    virtual ~MovingObject() {};
    virtual Position getNextPosition() = 0;
    Position getCurrentPosition() const {
        return pos;
    }

    //bunch of pure virtual methods
    virtual void move() = 0;
    virtual string str() const = 0;
    virtual int getExp() const = 0;
    virtual string getName() const = 0;
};

class Sherlock : public MovingObject {
private:
    string moving_rule;
    int hp;
    int exp;
    int moveIndex;
public:
    Sherlock(int index, const string& moving_rule, const Position& init_pos, Map* map, int init_hp, int init_exp) : MovingObject(index, init_pos, map, "Sherlock"), moving_rule(moving_rule), hp(init_hp), exp(init_exp) {
        hp = max(0, min(hp, 500));
        exp = max(0, min(exp, 900));
    };
    Position getNextPosition() {
        char direction = moving_rule[moveIndex];
        int current_row = pos.getRow();
        int current_col = pos.getCol();

        switch (direction) {
        case 'U': {
            current_col--;
            break;
        }
        case 'L': {
            current_row--;
            break;
        }
        case 'D': {
            current_col++;
            break;
        }
        case 'R': {
            current_row++;
            break;
        }
        }
        //wrap around, might remove
        if (current_row < 0) current_row = map->getNumRows() - 1;
        if (current_row > map->getNumRows() - 1) current_row = 0;
        if (current_col < 0) current_col = map->getNumCols() - 1;
        if (current_col > map->getNumCols() - 1) current_col = 0;

        moveIndex = (moveIndex + 1) % moving_rule.size();
        
        return Position(current_row, current_col);
    } 
    void move() override {
        Position next_pos = getNextPosition();
        if (map->isValid(next_pos, this)) {
            pos = next_pos;
        }
    }

    string str() override {
        return "Sherlock[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule" + moving_rule + "]";
    }

};

class Watson : public MovingObject {
private:
    string moving_rule;
    int hp;
    int exp;
    int moveIndex;

public:
    Watson(int index, const string& moving_rule, const Position& init_pos, Map* map, int init_hp, int init_exp) : MovingObject(index, init_pos, map, "Watson"), moving_rule(moving_rule), hp(init_hp), exp(init_exp) {
        hp = max(0, min(hp, 500));
        exp = max(0, min(exp, 900));
    };
    
    Position getNextPosition() {
        char direction = moving_rule[moveIndex];
        int current_row = pos.getRow();
        int current_col = pos.getCol();
        switch (direction) {
        case 'U': {
            current_col--;
            break;
        }
        case 'L': {
            current_row--;
            break;
        }
        case 'D': {
            current_col++;
            break;
        }
        case 'R': {
            current_row++;
            break;
        }
        }
        //wrap around, might remove
        if (current_row < 0) current_row = map->getNumRows() - 1;
        if (current_row > map->getNumRows() - 1) current_row = 0;
        if (current_col < 0) current_col = map->getNumCols() - 1;
        if (current_col > map->getNumCols() - 1) current_col = 0;

        moveIndex = (moveIndex + 1) % moving_rule.size();

        return Position(current_row, current_col);
    }
    void move() override {
        Position next_pos = getNextPosition();
        if (map->isValid(next_pos, this)) {
            pos = next_pos;
        }
    }
    string str() override {
        return "Watson[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule" + moving_rule + "]";
    }
    int getExp() const override {
        return exp;
    }
    string getName() const override {
        return name;
    }
};

class Criminal : public MovingObject {
private:
    Sherlock* sherlock;
    Watson* watson;

public:
    Criminal(int index, const Position& init_pos, Map* map, Sherlock* sherlock, Watson* watson) : MovingObject(index, init_pos, map), sherlock(sherlock), watson(watson) {};
    Position getNextPosition() override {
        int current_row = pos.getRow();
        int current_col = pos.getCol();
        Position next_pos = pos;

        int s_row = sherlock->getCurrentPosition().getRow();
        int s_col = sherlock->getCurrentPosition().getCol();
        int w_row = watson->getCurrentPosition().getRow();
        int w_col = watson->getCurrentPosition().getCol();

        int max_distance = -1;
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (abs(dr) == abs(dc)) continue; //skips current position & diagonal moves

                Position c_pos(current_row + dr, current_col + dc);
                int c_row = c_pos.getRow();
                int c_col = c_pos.getCol();

                if (!map->isValid(c_pos, this)) continue; //skips invalid moves

                int distance_to_sherlock = abs(c_row - s_row) + abs(c_col - s_col);
                int distance_to_watson = abs(c_row - w_row) + abs(c_col - w_col);
                int total_distance = distance_to_sherlock + distance_to_watson;

                if (total_distance > max_distance) {
                    max_distance = total_distance;
                    next_pos = c_pos;
                }
            }
        }
        return next_pos;
    }
    void move() override {
        Position next_pos = getNextPosition();
        if (map->isValid(next_pos, this)) {
            pos = next_pos;
        }
    }
    string str() override {
        return "Criminal=[index=" + to_string(index) + ";pos=" + pos.str() + "]";
     }
};

class ArrayMovingObject {
private:
    int count;
    int capacity;
    MovingObject** arr_mv_objs;

public:
    ArrayMovingObject(int capacity) : capacity(capacity), count(0) {
        arr_mv_objs = new MovingObject*[capacity];
    };

    ~ArrayMovingObject() {
        for (int i = 0; i < capacity; i++) {
            delete arr_mv_objs[i];
        }
        delete[] arr_mv_objs;
    };
    bool isFull() const {
        return count == capacity;
    };
    bool add(MovingObject* mv_obj) {
        if (isFull) {
            return false;
        }
        else {
            arr_mv_objs[count] = mv_obj;
            count++;
            return true;
        }
    };
    MovingObject* get(int index) const {};
    int size() const {
        return count;
    }; // return current number of elements in the array
    string str() const {
        string str = "[ArrayMovingObject[count=" + to_string(count) + ";capacity=" + to_string(capacity);
        for (int i = 0; i < count; i++) {
            str += ";" + arr_mv_objs[i]->str();
        }
        str += "]";
        return str;
    };
};

class Configuration {
    friend class StudyPinkProgram;

private:
    int map_num_rows;
    int map_num_cols;
    int num_walls;
    Position* arr_walls;
    int num_fake_walls;
    Position* arr_fake_walls;
    string sherlock_moving_rule;
    string sherlock_init_pos;
    string watson_moving_rule;
    string watson_init_pos;
    string criminal_init_pos;
    int num_steps;

public:
    Configuration(const string& filepath) {
        ifstream file(filepath);
        string config_line;

    };
    ~Configuration();
    string str() const;
};

// Robot, BaseItem, BaseBag,...

class StudyPinkProgram {
private:
    // Sample attributes
    Configuration * config;
    
    Sherlock * sherlock;
    Watson * watson;
    Criminal * criminal;
    
    Map * map;
    ArrayMovingObject * arr_mv_objs;


public:
    StudyPinkProgram(const string & config_file_path);

    bool isStop() const;

    void printResult() const {
        if (sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
            cout << "Sherlock caught the criminal" << endl;
        }
        else if (watson->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
            cout << "Watson caught the criminal" << endl;
        }
        else {
            cout << "The criminal escaped" << endl;
        }
    }

    void printStep(int si) const {
        cout << "Step: " << setw(4) << setfill('0') << si
            << "--"
            << sherlock->str() << "--|--" << watson->str() << "--|--" << criminal->str() << endl;
    }

    void run(bool verbose) {
        // Note: This is a sample code. You can change the implementation as you like.
        // TODO
        for (int istep = 0; istep < config->num_steps; ++istep) {
            for (int i = 0; i < arr_mv_objs->size(); ++i) {
                arr_mv_objs->get(i)->move();
                if (isStop()) {
                    printStep(istep);
                    break;
                }
                if (verbose) {
                    printStep(istep);
                }
            }
        }
        printResult();
    }

    ~StudyPinkProgram();
};

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
#endif /* _H_STUDY_IN_PINK_2_H_ */
