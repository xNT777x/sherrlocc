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
class MovingObject;
class Position;
class Configuration;
class Map;

class Criminal;
class RobotS;
class RobotW;
class RobotSW;
class RobotC;

class ArrayMovingObject;
class StudyPinkProgram;

class BaseItem;
class BaseBag;
class SherlockBag;
class WatsonBag;

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

    bool isBlocked(int dr, int dc,const Position& pos, MovingObject* mv_obj) const {
        int c_row = pos.getRow();
        int c_col = pos.getCol();
        // check if both ways of diagonal move are blocked
        if (!(abs(dr) == 2 || abs(dc) == 2)) {
            Position next_pos(c_row + dr, c_col);
            if (!isValid(next_pos, mv_obj)) {
                Position next_pos(c_row, c_col + dc);
                if (!isValid(next_pos, mv_obj)) return false;
            }
        }
        //check if there's a wall in front of a skip (2-unit move in same direction)
        else {
            if (dc == -2) { // U
                Position next_pos(c_row, c_col - 1);
                if (!isValid(next_pos, mv_obj)) return false;
            }
            if (dc == 2) { // D
                Position next_pos(c_row, c_col + 1);
                if (!isValid(next_pos, mv_obj)) return false;
            }
            if (dr == -2) { // L
                Position next_pos(c_row - 1, c_col);
                if (!isValid(next_pos, mv_obj)) return false;
            }
            if (dr == 2) { // R
                Position next_pos(c_row + 1, c_col);
                if (!isValid(next_pos, mv_obj)) return false;
            }
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
    static const Position npos;

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

const Position Position::npos = Position(-1, -1);

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
    virtual string str() = 0;
    virtual int getExp()  = 0;
    virtual int getHp() = 0;
    virtual string getName() = 0;
};
//MovingObject as ancestor
class Sherlock : public MovingObject {
private:
    string moving_rule;
    int hp;
    int exp;
    int moveIndex;
public:
    Sherlock(int index, const string& moving_rule, const Position& init_pos, Map* map, int init_hp, int init_exp) : MovingObject(index, init_pos, map, "Sherlock"), moving_rule(moving_rule), hp(max(0, min(init_hp, 500))), exp(max(0, min(init_exp, 900))) {};
    Position getNextPosition() override {
        char direction = moving_rule[moveIndex];
        int current_row = pos.getRow();
        int current_col = pos.getCol();

        switch (direction) {
        case 'U': {
            current_row--;
            break;
        }
        case 'L': {
            current_col--;
            break;
        }
        case 'D': {
            current_row++;
            break;
        }
        case 'R': {
            current_col++;
            break;
        }
        }
        if (current_col < 0) current_col = map->getNumRows() - 1;
        if (current_col > map->getNumRows() - 1) current_col = 0;
        if (current_row < 0) current_row = map->getNumCols() - 1;
        if (current_row > map->getNumCols() - 1) current_row = 0;

        moveIndex = (moveIndex + 1) % moving_rule.size();

        return Position(current_col, current_row);
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

    int getExp() override {
        return exp;
    }
    int getHp() override {
        return hp;
    }
    string getName() override {
        return "Sherlock";
    }
};

class Watson : public MovingObject {
private:
    string moving_rule;
    int hp;
    int exp;
    int moveIndex;

public:
    Watson(int index, const string& moving_rule, const Position& init_pos, Map* map, int init_hp, int init_exp) : MovingObject(index, init_pos, map, "Watson"), moving_rule(moving_rule), hp(max(0, min(init_hp, 500))), exp(max(0, min(init_exp, 900))) {
    };

    Position getNextPosition() override {
        char direction = moving_rule[moveIndex];
        int current_row = pos.getRow();
        int current_col = pos.getCol();

        switch (direction) {
        case 'U': {
            current_row--;
            break;
        }
        case 'L': {
            current_col--;
            break;
        }
        case 'D': {
            current_row++;
            break;
        }
        case 'R': {
            current_col++;
            break;
        }
        }
        if (current_col < 0) current_col = map->getNumRows() - 1;
        if (current_col > map->getNumRows() - 1) current_col = 0;
        if (current_row < 0) current_row = map->getNumCols() - 1;
        if (current_row > map->getNumCols() - 1) current_row = 0;

        moveIndex = (moveIndex + 1) % moving_rule.size();

        return Position(current_col, current_row);
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
    int getExp() override {
        return exp;
    }
    int getHp() override {
        return hp;
    }
    string getName() override {
        return "Watson";
    }
};

class Criminal : public MovingObject {
private:
    Sherlock* sherlock;
    Watson* watson;

public:
    Criminal(int index, const Position& init_pos, Map* map, Sherlock* sherlock, Watson* watson) : MovingObject(index, init_pos, map, "Criminal"), sherlock(sherlock), watson(watson) {};
    Position getNextPosition() override {
        Position next_pos = Position::npos;
        int current_row = pos.getRow();
        int current_col = pos.getCol();

        int s_row = sherlock->getCurrentPosition().getRow();
        int s_col = sherlock->getCurrentPosition().getCol();
        int w_row = watson->getCurrentPosition().getRow();
        int w_col = watson->getCurrentPosition().getCol();

        int max_distance = -1;
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (abs(dr) == abs(dc)) continue; //skips current position and diagonal moves

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
    string getName() override {
        return "Criminal";
    }
    
};

class Character : public MovingObject {
public:
    Character(int index, const Position& pos, Map* map, const string& name) : MovingObject(index, pos, map, name) {};

};

// Character as ancestor
class Sherlock : public Character {
private:
    string moving_rule;
    int hp;
    int exp;
    int moveIndex;
public:
    Sherlock(int index, const string& moving_rule, const Position& init_pos, Map* map, int init_hp, int init_exp) : Character(index, init_pos, map, "Sherlock"), moving_rule(moving_rule), hp(max(0, min(init_hp, 500))), exp(max(0, min(init_exp, 900))) {};

    int setExp(int newexp) {
        exp = newexp;
        return exp;
    }
    int setHp(int newhp) {
        hp = newhp;
        return hp;
    }
};

class Watson : public Character {
private:
    string moving_rule;
    int hp;
    int exp;
    int moveIndex;

public:
    Watson(int index, const string& moving_rule, const Position& init_pos, Map* map, int init_hp, int init_exp) : Character(index, init_pos, map, "Watson"), moving_rule(moving_rule), hp(max(0, min(init_hp, 500))), exp(max(0, min(init_exp, 900))) {
    };

    int setExp(int newexp) {
        exp = newexp;
        return exp;
    }
    int setHp(int newhp) {
        hp = newhp;
        return hp;
    }
};

class Criminal : public Character {
private:
    Sherlock* sherlock;
    Watson* watson;

public:
    Criminal(int index, const Position& init_pos, Map* map, Sherlock* sherlock, Watson* watson) : Character(index, init_pos, map, "Criminal"), sherlock(sherlock), watson(watson) {};
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
        for (int i = 0; i < count; i++) {
            delete arr_mv_objs[i];
        }
        delete[] arr_mv_objs;
    };
    bool isFull() const {
        return count == capacity;
    };
    bool add(MovingObject* mv_obj) {
        if (isFull()) {
            return false;
        }
        else {
            arr_mv_objs[count] = mv_obj;
            count++;
            return true;
        }
    };
    MovingObject* get(int index) const {
        if (index < 0 || index >= count) {
            return nullptr;
        }
        return arr_mv_objs[index];
    };
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
    int max_num_moving_objects;
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

    //extra attributes for printing config
    int sherlock_init_hp;
    int sherlock_init_exp;
    int watson_init_hp;
    int watson_init_exp;

public:
    int countPosition(string config_line) {
        int pos_count = 1;
        int index = config_line.find_first_of(";");
        while (index != string::npos) {
            config_line.erase(index);
            pos_count++;
        }
        return pos_count;
    }
    string printCoordinate(int amount, Position* array) {
        string coords;
        for (int i = 0; i < amount; i++) {
            coords += array[i].str();
            if (i < amount - 1) coords += ";";
        }
        return coords;
    }

    Configuration(const string& filepath) {
        ifstream file(filepath);
        string config_line;

        while (getline(file, config_line)) {
            if (config_line.find("MAP_NUM_ROWS") != string::npos) {
                sscanf(config_line.c_str(), "%d", map_num_rows);
            }
            if (config_line.find("MAP_NUM_COLS") != string::npos) {
                sscanf(config_line.c_str(), "%d", map_num_cols);
            }
            if (config_line.find("MAX_NUM_MOVING_OBJECTS") != string::npos) {
                sscanf(config_line.c_str(), "%d", max_num_moving_objects);
            }
            if (config_line.find("MAX_NUM_MOVING_OBJECTS") != string::npos) {
                sscanf(config_line.c_str(), "%d", max_num_moving_objects);
            }
            if (config_line.find("ARRAY_WALLS") != string::npos) {
                string array_wall_values = config_line.substr(config_line.find("["));
                num_walls = countPosition(array_wall_values);

                for (int i = 0; i < num_walls; i++) {

                    string wall_coordinate = array_wall_values.substr(array_wall_values.find_first_of("("), array_wall_values.find_first_of(")") - array_wall_values.find_first_of("(") + 1);

                    arr_walls[i] = wall_coordinate;
                    array_wall_values.erase(array_wall_values.find(wall_coordinate), wall_coordinate.length());
                }
            }
            if (config_line.find("ARRAY_FAKE_WALLS") != string::npos) {
                string array_fake_wall_values = config_line.substr(config_line.find("["));
                num_fake_walls = countPosition(array_fake_wall_values);

                for (int i = 0; i < num_fake_walls; i++) {
                    string fake_wall_coordinate = array_fake_wall_values.substr(array_fake_wall_values.find_first_of("("), array_fake_wall_values.find_first_of(")") - array_fake_wall_values.find_first_of("(") + 1);

                    arr_fake_walls[i] = fake_wall_coordinate;
                    array_fake_wall_values.erase(array_fake_wall_values.find(fake_wall_coordinate), fake_wall_coordinate.length());
                }
            }
            if (config_line.find("SHERLOCK_MOVING_RULE") != string::npos) {
                sherlock_moving_rule = config_line.substr(config_line.find("=") + 1);
            }
            if (config_line.find("SHERLOCK_INIT_POS") != string::npos) {
                sherlock_init_pos = config_line.substr(config_line.find("=") + 1);
            }
            if (config_line.find("WATSON_MOVING_RULE") != string::npos) {
                watson_moving_rule = config_line.substr(config_line.find("=") + 1);
            }
            if (config_line.find("WATSON_INIT_POS") != string::npos) {
                watson_init_pos = config_line.substr(config_line.find("=") + 1);
            }
            if (config_line.find("CRIMINAL_INIT_POS") != string::npos) {
                criminal_init_pos = config_line.substr(config_line.find("=") + 1);
            }
            if (config_line.find("NUM_STEPS") != string::npos) {
                sscanf(config_line.c_str(), "%d", num_steps);
            }
            //added for str method
            if (config_line.find("SHERLOCK_INIT_HP") != string::npos) {
                sscanf(config_line.c_str(), "%d", sherlock_init_hp);
            }
            if (config_line.find("SHERLOCK_INIT_EXP") != string::npos) {
                sscanf(config_line.c_str(), "%d", sherlock_init_exp);
            }
            if (config_line.find("WATSON_INIT_HP") != string::npos) {
                sscanf(config_line.c_str(), "%d", watson_init_hp);
            }
            if (config_line.find("WATSON_INIT_EXP") != string::npos) {
                sscanf(config_line.c_str(), "%d", watson_init_exp);
            }
        }

    };
    ~Configuration() {
        delete[] arr_walls;
        delete[] arr_fake_walls;
    };
    string str() {
        string str;
        str = "Configuration[";
        str += "\nMAP_NUM_ROWS=" + to_string(map_num_rows);
        str += "\nMAP_NUM_COLS=" + to_string(map_num_cols);
        str += "\nMAX_NUM_MOVING_OBJECT" + to_string(max_num_moving_objects);
        str += "\nNUM_WALLS=" + to_string(num_walls);
        str += "\nARRAY_WALLS=[" + printCoordinate(num_walls, arr_walls);
        str += "]\nNUM_FAKE_WALLS=" + to_string(num_fake_walls);
        str += "\nARRAY_WAKE_WALLS[" + printCoordinate(num_fake_walls, arr_fake_walls);
        str += "]\nSHERLOCK_MOVING_RULE=" + sherlock_moving_rule;
        str += "\nSHERLOCK_INIT_POS=" + sherlock_init_pos;
        str += "\nSHERLOCK_INIT_HP=" + to_string(sherlock_init_hp);
        str += "\nSHERLOCK_INIT_EXP" + to_string(sherlock_init_exp);
        str += "\nWATSON_MOVING_RULE" + watson_moving_rule;
        str += "\nWATSON_INIT_POS" + watson_init_pos;
        str += "\nWATSON_INIT_HP=" + to_string(watson_init_hp);
        str += "\nWATSON_INIT_EXP=" + to_string(watson_init_exp);
        str += "\nCRIMINAL_INIT_POS=" + criminal_init_pos;
        str += "\nNUM_STEPS=" + to_string(num_steps);
        str += "\n]";
        return str;
    };
};

// Robot, BaseItem, BaseBag,...

// Robot Types


// MovingObject as ancestor
class RobotC : public MovingObject {
private:
    RobotType robot_type;
    BaseItem* item;

    Criminal* criminal;
public:
    RobotC(int index, const Position& init_pos, Map* map, Criminal* criminal) : MovingObject(index, init_pos, map, "RobotC"), criminal(criminal), robot_type(C) {};
    Position getNextPosition() override {
        return criminal->getCurrentPosition();
    }
    ~RobotC() {};

    void move() override {
        Position next_pos = getNextPosition();
        if (map->isValid(next_pos, this)) {
            pos = next_pos;
        }
    }

    int getDistance(Sherlock* sherlock) const {
        int r_row = pos.getRow();
        int r_col = pos.getCol();
        int s_row = sherlock->getCurrentPosition().getRow();
        int s_col = sherlock->getCurrentPosition().getCol();

        int distance_to_sherlock = abs(r_row - s_row) + abs(r_col - s_col);
        return distance_to_sherlock;
    }

    int getDistance(Watson* watson) const {
        int r_row = pos.getRow();
        int r_col = pos.getCol();
        int w_row = watson->getCurrentPosition().getRow();
        int w_col = watson->getCurrentPosition().getCol();

        int distance_to_watson = abs(r_row - w_row) + abs(r_col - w_col);
        return distance_to_watson;
    }

    string str() override {
        return "[pos=" + pos.str() + ";type=C;dist=]";
    }
};

class RobotS : public MovingObject {
private:
    RobotType robot_type;
    BaseItem* item;

    Criminal* criminal;
    Sherlock* sherlock;
public:
    RobotS(int index, const Position& init_pos, Map* map, Criminal* criminal, Sherlock* sherlock) : MovingObject(index, init_pos, map, "RobotS"), criminal(criminal), sherlock(sherlock), robot_type(S) {};
    Position getNextPosition() override {
        Position current_pos = criminal->getCurrentPosition();
        Position next_pos = Position::npos;

        int s_row = sherlock->getCurrentPosition().getRow();
        int s_col = sherlock->getCurrentPosition().getCol();
        int current_r_row = current_pos.getRow();
        int current_r_col = current_pos.getCol();

        int min_distance = INT_MAX;
        for (int dr = -2; dr <= 2; dr++) {
            for (int dc = -2; dc <= 2; dc++) {
                if (abs(dr) + abs(dc) != 2) continue; //skips steps that are not 2 units

                Position r_pos(current_r_row + dr, current_r_col + dc);
                int r_row = r_pos.getRow();
                int r_col = r_pos.getCol();

                if (!map->isValid(r_pos, this)) continue; //skips invalid moves

                int distance_to_sherlock = abs(r_row - s_row) + abs(r_col - s_col);

                if (distance_to_sherlock < min_distance) {
                    min_distance = distance_to_sherlock;
                    next_pos = r_pos;
                }
                else if (distance_to_sherlock == min_distance && ((dr == 0 && dc == -1) || (dr == 1 && dc == 0) || (dr == 0 && dc == 1) || (dr == -1 && dc == 0))) {
                    next_pos = r_pos;
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

    int getDistance() const {
        int r_row = pos.getRow();
        int r_col = pos.getCol();
        int s_row = sherlock->getCurrentPosition().getRow();
        int s_col = sherlock->getCurrentPosition().getCol();

        int distance_to_sherlock = abs(r_row - s_row) + abs(r_col - s_col);
        return distance_to_sherlock;
    }
    string str() override {
        return "[pos=" + pos.str() + ";type=S;dist=" + to_string(getDistance()) + "]";
    }
};

class RobotW : public MovingObject {
private:
    RobotType robot_type;
    BaseItem* item;

    Criminal* criminal;
    Watson* watson;
public:
    RobotW(int index, const Position& init_pos, Map* map, Criminal* criminal, Watson* watson) : MovingObject(index, init_pos, map, "RobotW"), criminal(criminal), watson(watson), robot_type(W) {};
    Position getNextPosition() override {
        Position current_pos = criminal->getCurrentPosition();
        Position next_pos = Position::npos;

        int w_row = watson->getCurrentPosition().getRow();
        int w_col = watson->getCurrentPosition().getCol();
        int current_r_row = current_pos.getRow();
        int current_r_col = current_pos.getCol();

        int min_distance = INT_MAX;
        for (int dr = -2; dr <= 2; dr++) {
            for (int dc = -2; dc <= 2; dc++) {
                if (abs(dr) + abs(dc) != 2) continue; //skips steps that are not 2 units

                Position r_pos(current_r_row + dr, current_r_col + dc);
                int r_row = r_pos.getRow();
                int r_col = r_pos.getCol();

                if (!map->isValid(r_pos, this)) continue; //skips invalid moves

                int distance_to_watson = abs(r_row - w_row) + abs(r_col - w_col);

                if (distance_to_watson > min_distance) {
                    min_distance = distance_to_watson;
                    next_pos = r_pos;
                }
                else if (distance_to_watson == min_distance && ((dr == 0 && dc == -1) || (dr == 1 && dc == 0) || (dr == 0 && dc == 1) || (dr == -1 && dc == 0))) { // prioritize in order U R D L
                    next_pos = r_pos;
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

    int getDistance() const {
        int r_row = pos.getRow();
        int r_col = pos.getCol();
        int w_row = watson->getCurrentPosition().getRow();
        int w_col = watson->getCurrentPosition().getCol();

        int distance_to_watson = abs(r_row - w_row) + abs(r_col - w_col);
        return distance_to_watson;
    }
    string str() override {
        return "[pos=" + pos.str() + ";type=W;dist=" + to_string(getDistance()) + "]";
    }
};

class RobotSW : public MovingObject {
private:
    RobotType robot_type;
    BaseItem* item;

    Criminal* criminal;
    Sherlock* sherlock;
    Watson* watson;
public:
    RobotSW(int index, const Position& init_pos, Map* map, Criminal* criminal, Sherlock* sherlock, Watson* watson) : MovingObject(index, init_pos, map, "RobotSW"), criminal(criminal), sherlock(sherlock), watson(watson), robot_type(SW) {};
    Position getNextPosition() override {
        Position current_pos = criminal->getCurrentPosition();
        Position next_pos = Position::npos;

        int s_row = sherlock->getCurrentPosition().getRow();
        int s_col = sherlock->getCurrentPosition().getCol();
        int w_row = watson->getCurrentPosition().getRow();
        int w_col = watson->getCurrentPosition().getCol();
        int current_r_row = current_pos.getRow();
        int current_r_col = current_pos.getCol();

        int min_distance = INT_MAX;
        for (int dr = -2; dr <= 2; dr++) {
            for (int dc = -2; dc <= 2; dc++) {
                if (abs(dr) + abs(dc) != 2) continue; //skips steps that are not 2 units
                if (map->isBlocked(dr, dc, current_pos, this)) continue;

                Position r_pos(current_r_row + dr, current_r_col + dc);
                int r_row = r_pos.getRow();
                int r_col = r_pos.getCol();

                if (!map->isValid(r_pos, this)) continue; //skips invalid moves

                int distance_to_sherlock = abs(r_row - s_row) + abs(r_col - s_col);
                int distance_to_watson = abs(r_row - w_row) + abs(r_col - w_col);
                int total_distance = distance_to_sherlock + distance_to_watson;

                if (total_distance > min_distance) {
                    min_distance = total_distance;
                    next_pos = r_pos;
                }
                else if (total_distance == min_distance && ((dr == 0 && dc == -2) || (dr == 1 && dc == -1) || (dr == 2 && dc == 0) || (dr == 1 && dc == 1) || (dr == 0 && dc == 2) && (dr == -1 && dc == 1) && (dr == -2 && dc == 0) || (dr == -1 && dc == -1))) {
                    next_pos = r_pos;
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

    int getDistance() const {
        int r_row = pos.getRow();
        int r_col = pos.getCol();
        int s_row = sherlock->getCurrentPosition().getRow();
        int s_col = sherlock->getCurrentPosition().getCol();
        int w_row = watson->getCurrentPosition().getRow();
        int w_col = watson->getCurrentPosition().getCol();

        int distance_to_sherlock = abs(r_row - s_row) + abs(r_col - s_col);
        int distance_to_watson = abs(r_row - w_row) + abs(r_col - w_col);
        int total_distance = distance_to_sherlock + distance_to_watson;
        return total_distance;
    }
    string str() override {
        return "[pos=" + pos.str() + ";type=SW;dist=" + to_string(getDistance()) + "]";
    }
};

class Robot : public MovingObject {
public:
    Robot(int index, const Position pos, Map* map, const string& name) : MovingObject(index, pos, map, name) {};
    ~Robot() {};

    virtual BaseItem* newItem(int r_row, int r_col) {
        int p = r_row * r_col;
        int s = p;
        while (s < 10) {
            int digit = s % 10;
            int ten = (s - digit) / 10;
            s = digit + ten;
        }
        switch (s) {
        case 0:
        case 1: {
            return new MagicBook();
        }
        case 2:
        case 3: {
            return new EnergyDrink();
        }
        case 4:
        case 5: {
            return new FirstAid();
        }
        case 6:
        case 7: {
            return new ExcemptionCard();
        }
        case 8:
        case 9: {
            int t = (r_row * 11 + r_col) % 4;
            switch (t) {
            case 0: {
                return new PassingCard("RobotS");
            }
            case 1: {
                return new PassingCard("RobotC");
            }
            case 2: {
                return new PassingCard("RobotSW");
            }
            case 3: {
                return new PassingCard("all");
            }
            }
        }
              return nullptr;
        }
    }
};
// Robot as ancestor
class RobotC : public Robot {

};

class RobotS : public Robot {

};

class RobotW : public Robot {

};

class RobotSW : public Robot {

};

//Items

class BaseItem {
protected:
    Sherlock* sherlock;
    Watson* watson;
public:
    BaseItem() {};
    ~BaseItem() {};
    virtual bool canUse(Character* obj, Robot* robot) = 0;
    virtual void use(Character* obj, Robot* robot) = 0;
};

class MagicBook : public BaseItem {
private:
    ItemType item_type;
public:
    MagicBook() : item_type(MAGIC_BOOK) {};
    ~MagicBook() {};

    bool canUse(Character* obj, Robot* robot) {
        if (obj->getExp() > 350) return false;

        return true;
    }

    void use(Character* obj, Robot* robot) override {
        if (obj->getName() == "Sherlock") {
            int new_exp = sherlock->getExp() * 1.25;
            sherlock->setExp(new_exp);
        }
        if (obj->getName() == "Watson") {
            int new_exp = watson->getExp() * 1.25;
            watson->setExp(new_exp);
        }
    }
};

class EnergyDrink : public BaseItem {
private:
    ItemType item_type;
public:
    EnergyDrink() : item_type(ENERGY_DRINK) {};
    ~EnergyDrink() {};
    bool canUse(Character* obj, Robot* robot) override {
        if (obj->getHp() > 100) return false;

        return true;
    }
    void use(Character* obj, Robot* robot) override {
        if (obj->getName() == "Sherlock") {
            int new_hp = sherlock->getHp() * 1.2;
            sherlock->setHp(new_hp);
        }
        if (obj->getName() == "Watson") {
            int new_hp = watson->getHp() * 1.2;
            watson->setHp(new_hp);
        }
    }
};

class FirstAid : public BaseItem {
private:
    ItemType item_type;
public:
    FirstAid() : item_type(FIRST_AID) {};
    ~FirstAid() {};
    bool canUse(Character* obj, Robot* robot) override {
        if (obj->getHp() > 100 && obj->getExp() > 350) return false;

        return true;
    }
    void use(Character* obj, Robot* robot) override {
        if (obj->getName() == "Sherlock") {
            int new_hp = sherlock->getHp() * 1.5;
            sherlock->setHp(new_hp);
        }
        if (obj->getName() == "Watson") {
            int new_hp = watson->getHp() * 1.5;
            watson->setHp(new_hp);
        }
    }
};
// wip
class ExcemptionCard : public BaseItem {
private:
    ItemType item_type;
public:
    ExcemptionCard() : item_type(EXCEMPTION_CARD) {};
    ~ExcemptionCard() {};
    bool canUse(Character* obj, Robot* robot) override {
        if (obj->getName() != "Sherlock") return false;
        if (obj->getHp() % 2 == 0) return false;

        return true;
    }
    void use(Character* obj, Robot* robot) override {
        /*pass challenge*/
    }
};
// wip
class PassingCard : public BaseItem {
private:
    ItemType item_type;
    string challenge;
public:
    PassingCard(string challenge) : challenge(challenge), item_type(PASSING_CARD) {};
    ~PassingCard() {};
    bool canUse(Character* obj, Robot* robot) override {
        if (obj->getName() != "Watson") return false;
        if (obj->getHp() % 2 != 0) return false;

        return true;
    }
    void use(Character* obj, Robot* robot) override {
        if (challenge == "all") /*pass challenge*/;
        if (challenge != robot->getName()) {
            int new_exp = watson->getExp() - 50;
            if (new_exp < 0) new_exp = 0;
            watson->setExp(new_exp);
        }
    }
};

class BaseBag {
private:
    Character* obj;
    int count;
    int capacity;
    BaseItem** list_items;
public:
    BaseBag(int capacity) : capacity(capacity), count(0) {
        list_items = new BaseItem * [capacity];
    }
    ~BaseBag() {
        delete[] list_items;
    }
    virtual bool insert(BaseItem* item) {
        if (count >= capacity) return false; // check if bag is full

        int index = count - 1;
        //shift all current items to the right
        for (int i = index; i <= 0; i++) {
            list_items[index + 1] = list_items[index];
        }
        list_items[0] = item;
        count++;
        return true;
    };
    virtual BaseItem* get() {};
};

class SherlockBag : public BaseBag {

};

class WatsonBag : public BaseBag {

};

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
    StudyPinkProgram(const string& config_file_path) {};

    bool isStop() const {};

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
