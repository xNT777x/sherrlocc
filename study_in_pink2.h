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
class Sherlock;
class Watson;

class Criminal;
class RobotS;
class RobotW;
class RobotSW;
class RobotC;

class ArrayMovingObject;
class Configuration;
class StudyPinkProgram;

class Character;
class Robot;

class BaseItem;

class MagicBook;
class EnergyDrink;
class FirstAid;
class ExcemptionCard;
class PassingCard;

class BaseBag;
class SherlockBag;
class WatsonBag;

class TestStudyInPink;

enum ItemType { MAGIC_BOOK, ENERGY_DRINK, FIRST_AID, EXCEMPTION_CARD, PASSING_CARD };
enum ElementType { PATH, WALL, FAKE_WALL };
enum RobotType { C=0, S, W, SW };

class Position {
private:
    int r, c;
public:
    static const Position npos() {
        return Position(-1, -1);
    };

    Position(int r = 0, int c = 0) : r(r), c(c) {};
    ~Position() {};

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

    bool isEqual(Position pos) const {
        int in_r = pos.getRow();
        int in_c = pos.getCol();
        return (in_r == this->r && in_c == this->c);
    }
    bool isEqual(int in_r, int in_c) const {
        return (in_r == this->r && in_c == this->c);
    }
    friend class TestStudyInPink;
};

class MapElement {
protected:
    ElementType type;
public:
    MapElement(ElementType in_type) : type(in_type) {};
    virtual ~MapElement() {};
    virtual ElementType getType() const {
        return type;
    }
    friend class TestStudyInPink;
};

class Path : public MapElement {
public:
    Path() : MapElement(PATH) {};
    friend class TestStudyInPink;
};

class Wall : public MapElement {
public:
    Wall() : MapElement(WALL) {};
    friend class TestStudyInPink;
};

class FakeWall : public MapElement {
private:
    int req_exp;

public:
    FakeWall(int in_req_exp) : MapElement(FAKE_WALL), req_exp(in_req_exp) {};
    ~FakeWall() {};
    int getReqExp() const {
        return req_exp;
    }
    friend class TestStudyInPink;
};

class MovingObject {
protected:
    int index;
    Position pos;
    Map* map;
    string name;

public:
    MovingObject(int index, const Position pos, Map* map, const string& name = "") : index(index), pos(pos), map(map), name(name) {};
    virtual ~MovingObject() {};
    virtual Position getNextPosition() = 0;
    Position getCurrentPosition() const {
        return pos;
    }
    virtual void setPosition(int new_r, int new_c) {
        pos.setCol(new_c);
        pos.setRow(new_r);
    }

    virtual void move() = 0;
    virtual string str() = 0;
    virtual int getExp() = 0;
    virtual int getHp() = 0;
    virtual string getName() = 0;

    friend class TestStudyInPink;
};

class Character : public MovingObject {
public:
    Character(int index, const Position& pos, Map* map, const string& name) : MovingObject(index, pos, map, name) {};

    Position getNextPosition() override {
        return Position();
    }
    void move() override {
    }
    string str() override {
        return "";
    }
    int getExp() override {
        return 0;
    }
    int getHp() override {
        return 0;
    }
    string getName() override {
        return name;
    }
    friend class TestStudyInPink;
};

class Map {
private:
    int num_rows, num_cols;
    MapElement*** map;

public:
    Map(int num_rows, int num_cols, int num_walls, Position* array_walls, int num_fake_walls, Position* array_fake_walls) : num_rows(num_rows), num_cols(num_cols) {
        map = new MapElement * *[num_rows];

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

    bool isBlocked(int dr, int dc, const Position& pos, MovingObject* mv_obj) const {
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
    friend class TestStudyInPink;
};

class Sherlock : public Character {
private:
    string moving_rule;
    int hp;
    int exp;
    int moveIndex;
public:
    Sherlock(int index, const string& moving_rule, const Position& init_pos, Map* map, int init_hp, int init_exp) : Character(index, init_pos, map, "Sherlock"), moving_rule(moving_rule), hp(max(0, min(init_hp, 500))), exp(max(0, min(init_exp, 900))), moveIndex(0) {};
    ~Sherlock() {};
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

        return Position(current_row, current_col);
    }
    void move() override {
        Position next_pos = getNextPosition();
        if (map->isValid(next_pos, this)) {
            pos = next_pos;
        }
    }

    string str() override {
        return "Sherlock[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule=" + moving_rule + "]";
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

    int setExp(int newexp) {
        exp = newexp;
        return exp;
    }
    int setHp(int newhp) {
        hp = newhp;
        return hp;
    }
    friend class TestStudyInPink;
};

class Watson : public Character {
private:
    string moving_rule;
    int hp;
    int exp;
    int moveIndex;

public:
    Watson(int index, const string& moving_rule, const Position& init_pos, Map* map, int init_hp, int init_exp) : Character(index, init_pos, map, "Watson"), moving_rule(moving_rule), hp(max(0, min(init_hp, 500))), exp(max(0, min(init_exp, 900))), moveIndex(0) {};
    ~Watson() {};
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

        return Position(current_row, current_col);
    }

    void move() override {
        Position next_pos = getNextPosition();
        if (map->isValid(next_pos, this)) {
            pos = next_pos;
        }
    }
    string str() override {
        return "Watson[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule=" + moving_rule + "]";
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
    int setExp(int newexp) {
        exp = newexp;
        return exp;
    }
    int setHp(int newhp) {
        hp = newhp;
        return hp;
    }
    friend class TestStudyInPink;
};

class Criminal : public Character {
private:
    Sherlock* sherlock;
    Watson* watson;

public:
    Criminal(int index, const Position& init_pos, Map* map, Sherlock* sherlock, Watson* watson) : Character(index, init_pos, map, "Criminal"), sherlock(sherlock), watson(watson) {};
    ~Criminal() {};
    Position getNextPosition() override {
        Position next_pos = Position::npos();
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
        return "Criminal[index=" + to_string(index) + ";pos=" + pos.str() + "]";
    }
    string getName() override {
        return "Criminal";
    }
    int getExp() override {
        return 0;
    }
    int getHp() override {
        return 0;
    }
    friend class TestStudyInPink;
};

class ArrayMovingObject {
public:
    int count;
    int capacity;
    MovingObject** arr_mv_objs;
    ArrayMovingObject(int capacity) : capacity(capacity), count(0) {
        arr_mv_objs = new MovingObject * [capacity];
    };

    ~ArrayMovingObject() {
        for (int i = 0; i < count; i++) {
            delete arr_mv_objs[i];
        }
        delete[] arr_mv_objs;
    };
    bool isFull() const {
        return count >= capacity;
    };
    bool add(MovingObject* mv_obj) {
        if (isFull()) {
            return false;
        }
        else {
            MovingObject* obj = mv_obj;
            arr_mv_objs[count] = obj;
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
    }
    string str() const {
        string str = "ArrayMovingObject[count=" + to_string(count) + ";capacity=" + to_string(capacity);
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
    Position sherlock_init_pos;
    string watson_moving_rule;
    Position watson_init_pos;
    Position criminal_init_pos;
    int num_steps;

    //extra attributes for printing config
    int sherlock_init_hp;
    int sherlock_init_exp;
    int watson_init_hp;
    int watson_init_exp;

public:
    int countPosition(string config_line) {
        int pos_count = 0;
        size_t index = config_line.find_first_of("(");
        while (index != string::npos) {
            pos_count++;
            index = config_line.find_first_of("(", index + 1);
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
                size_t equal_pos = config_line.find("=");

                sscanf(config_line.c_str() + equal_pos + 1, "%d", &map_num_rows);
            }
            if (config_line.find("MAP_NUM_COLS") != string::npos) {
                size_t equal_pos = config_line.find("=");

                sscanf(config_line.c_str() + equal_pos + 1, "%d", &map_num_cols);
            }
            if (config_line.find("MAX_NUM_MOVING_OBJECTS") != string::npos) {
                size_t equal_pos = config_line.find("=");

                sscanf(config_line.c_str() + equal_pos + 1, "%d", &max_num_moving_objects);
            }
            if (config_line.find("ARRAY_WALLS") != string::npos) {
                string array_wall_values = config_line.substr(config_line.find("["));
                num_walls = countPosition(array_wall_values);
                if (num_walls > 0) {
                    arr_walls = new Position[num_walls];

                    for (int i = 0; i < num_walls; i++) {
                        string wall_coordinate;
                        size_t start_pos = array_wall_values.find_first_of("(");
                        size_t end_pos = array_wall_values.find_first_of(")");
                        int r, c;
                        if (start_pos != string::npos && end_pos != string::npos && end_pos > start_pos) {
                            wall_coordinate = array_wall_values.substr(start_pos, end_pos - start_pos + 1);
                            sscanf(wall_coordinate.c_str(), "(%d,%d)", &r, &c);
                        }

                        arr_walls[i] = Position(r, c);
                        array_wall_values.erase(array_wall_values.find(wall_coordinate), wall_coordinate.length());
                    }
                }
                else arr_walls = nullptr;
            }
            if (config_line.find("ARRAY_FAKE_WALLS") != string::npos) {
                string array_fake_wall_values = config_line.substr(config_line.find("["));
                num_fake_walls = countPosition(array_fake_wall_values);
                if (num_fake_walls > 0) {
                    arr_fake_walls = new Position[num_fake_walls];

                    for (int i = 0; i < num_fake_walls; i++) {
                        string fake_wall_coordinate;
                        size_t start_pos = array_fake_wall_values.find_first_of("(");
                        size_t end_pos = array_fake_wall_values.find_first_of(")");
                        int r, c;
                        if (start_pos != string::npos && end_pos != string::npos && end_pos > start_pos) {
                            string fake_wall_coordinate = array_fake_wall_values.substr(start_pos, end_pos - start_pos + 1);
                            sscanf(fake_wall_coordinate.c_str(), "(%d,%d)", &r, &c);
                        }

                        arr_fake_walls[i] = Position(r,c);
                        array_fake_wall_values.erase(array_fake_wall_values.find(fake_wall_coordinate), fake_wall_coordinate.length());
                    }
                }
                else arr_fake_walls = nullptr;
                
            }
            if (config_line.find("SHERLOCK_MOVING_RULE") != string::npos) {
                sherlock_moving_rule = config_line.substr(config_line.find("=") + 1);
            }
            if (config_line.find("SHERLOCK_INIT_POS") != string::npos) {
                sherlock_init_pos = Position(config_line.substr(config_line.find("=") + 1));
            }
            if (config_line.find("WATSON_MOVING_RULE") != string::npos) {
                watson_moving_rule = config_line.substr(config_line.find("=") + 1);
            }
            if (config_line.find("WATSON_INIT_POS") != string::npos) {
                watson_init_pos = Position(config_line.substr(config_line.find("=") + 1));
            }
            if (config_line.find("CRIMINAL_INIT_POS") != string::npos) {
                criminal_init_pos = Position(config_line.substr(config_line.find("=") + 1));
            }
            if (config_line.find("NUM_STEPS") != string::npos) {
                size_t equal_pos = config_line.find("=");

                sscanf(config_line.c_str() + equal_pos + 1, "%d", &num_steps);
            }
            //added for str method
            if (config_line.find("SHERLOCK_INIT_HP") != string::npos) {
                size_t equal_pos = config_line.find("=");

                sscanf(config_line.c_str() + equal_pos + 1, "%d", &sherlock_init_hp);
            }
            if (config_line.find("SHERLOCK_INIT_EXP") != string::npos) {
                size_t equal_pos = config_line.find("=");

                sscanf(config_line.c_str() + equal_pos + 1, "%d", &sherlock_init_exp);
            }
            if (config_line.find("WATSON_INIT_HP") != string::npos) {
                size_t equal_pos = config_line.find("=");

                sscanf(config_line.c_str() + equal_pos + 1, "%d", &watson_init_hp);
            }
            if (config_line.find("WATSON_INIT_EXP") != string::npos) {
                size_t equal_pos = config_line.find("=");

                sscanf(config_line.c_str() + equal_pos + 1, "%d", &watson_init_exp);
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
        str += "\nSHERLOCK_INIT_POS=" + sherlock_init_pos.str();
        str += "\nSHERLOCK_INIT_HP=" + to_string(sherlock_init_hp);
        str += "\nSHERLOCK_INIT_EXP" + to_string(sherlock_init_exp);
        str += "\nWATSON_MOVING_RULE" + watson_moving_rule;
        str += "\nWATSON_INIT_POS" + watson_init_pos.str();
        str += "\nWATSON_INIT_HP=" + to_string(watson_init_hp);
        str += "\nWATSON_INIT_EXP=" + to_string(watson_init_exp);
        str += "\nCRIMINAL_INIT_POS=" + criminal_init_pos.str();
        str += "\nNUM_STEPS=" + to_string(num_steps);
        str += "\n]";
        return str;
    };
    friend class TestStudyInPink;
};

// Robot, BaseItem, BaseBag,...

class Robot : public MovingObject {
protected:
    RobotType robot_type;
    BaseItem* item;
public:
    Robot(int index, const Position pos, Map* map, const string& name, RobotType robot_type) : MovingObject(index, pos, map, name), robot_type(robot_type), item(nullptr) {}
    ~Robot() {};

    void setItem(BaseItem* new_item) {
        item = new_item;
    };

    virtual BaseItem* getItem() {
        return item;
    }
    Position getNextPosition() override {
        return Position();
    }
    void move() override {
    }
    string str() override {
        return "";
    }
    int getExp() override {
        return 0;
    }
    int getHp() override {
        return 0;
    }
    string getName() override {
        return name;
    }
    virtual RobotType getRobotType() {
        return robot_type;
    }
    friend class TestStudyInPink;
};

class RobotC : public Robot {
private:
    RobotType robot_type;
    BaseItem* item;

    Criminal* criminal;
public:
    RobotC(int index, const Position& init_pos, Map* map, Criminal* criminal) : Robot(index, init_pos, map, "RobotC", C), criminal(criminal) {};
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
    friend class TestStudyInPink;
};

class RobotS : public Robot {
private:
    RobotType robot_type;
    BaseItem* item;

    Criminal* criminal;
    Sherlock* sherlock;
public:
    RobotS(int index, const Position& init_pos, Map* map, Criminal* criminal, Sherlock* sherlock) : Robot(index, init_pos, map, "RobotS", S), criminal(criminal), sherlock(sherlock) {};
    ~RobotS() {};

    Position getNextPosition() override {
        Position current_pos = criminal->getCurrentPosition();
        Position next_pos = Position::npos();

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
    friend class TestStudyInPink;
};

class RobotW : public Robot {
private:
    RobotType robot_type;
    BaseItem* item;

    Criminal* criminal;
    Watson* watson;
public:
    RobotW(int index, const Position& init_pos, Map* map, Criminal* criminal, Watson* watson) : Robot(index, init_pos, map, "RobotW", W), criminal(criminal), watson(watson) {};
    ~RobotW() {};
    friend class TestStudyInPink;

    Position getNextPosition() override {
        Position current_pos = criminal->getCurrentPosition();
        Position next_pos = Position::npos();

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

class RobotSW : public Robot {
private:
    RobotType robot_type;
    BaseItem* item;

    Criminal* criminal;
    Sherlock* sherlock;
    Watson* watson;
public:
    RobotSW(int index, const Position& init_pos, Map* map, Criminal* criminal, Sherlock* sherlock, Watson* watson) : Robot(index, init_pos, map, "RobotSW", SW), criminal(criminal), sherlock(sherlock), watson(watson) {};
    ~RobotSW() {};
    friend class TestStudyInPink;

    Position getNextPosition() override {
        Position current_pos = criminal->getCurrentPosition();
        Position next_pos = Position::npos();

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

class BaseItem {
protected:
    Sherlock* sherlock;
    Watson* watson;
    ItemType item_type;
public:
    BaseItem(ItemType item_type) : item_type(item_type) {};
    virtual ~BaseItem() {};
    friend class TestStudyInPink;

    virtual bool canUse(Character* obj, Robot* robot) = 0;
    virtual void use(Character* obj, Robot* robot) = 0;
    virtual ItemType getItemType() {
        return item_type;
    }
};

class MagicBook : public BaseItem {
public:
    MagicBook() : BaseItem(MAGIC_BOOK) {};
    ~MagicBook() {};
    friend class TestStudyInPink;

    bool canUse(Character* obj, Robot* robot = NULL) {
        if (obj->getExp() > 350) return false;

        return true;
    }

    void use(Character* obj, Robot* robot = NULL) override {
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
public:
    EnergyDrink() : BaseItem(ENERGY_DRINK) {};
    ~EnergyDrink() {};
    friend class TestStudyInPink;

    bool canUse(Character* obj, Robot* robot = NULL) override {
        if (obj->getHp() > 100) return false;

        return true;
    }
    void use(Character* obj, Robot* robot = NULL) override {
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
public:
    FirstAid() : BaseItem(FIRST_AID) {};
    ~FirstAid() {};
    friend class TestStudyInPink;

    bool canUse(Character* obj, Robot* robot = NULL) override {
        if (obj->getHp() > 100 && obj->getExp() > 350) return false;

        return true;
    }
    void use(Character* obj, Robot* robot = NULL) override {
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

class ExcemptionCard : public BaseItem {
public:
    ExcemptionCard() : BaseItem(EXCEMPTION_CARD) {};
    ~ExcemptionCard() {};
    friend class TestStudyInPink;

    bool canUse(Character* obj, Robot* robot) override {
        if (obj->getName() != "Sherlock") return false;
        if (obj->getHp() % 2 == 0) return false;
        if (!robot) return false;

        return true;
    }
    void use(Character* obj, Robot* robot) override {
        /*pass challenge*/
    }
};

class PassingCard : public BaseItem {
private:
    string challenge;
public:
    PassingCard(string challenge) : challenge(challenge), BaseItem(PASSING_CARD) {};
    ~PassingCard() {};
    friend class TestStudyInPink;

    bool canUse(Character* obj, Robot* robot) override {
        if (obj->getName() != "Watson") return false;
        if (obj->getHp() % 2 != 0) return false;
        if (!robot) return false;

        return true;
    }
    void use(Character* obj, Robot* robot) override {
        if (challenge != robot->getName()) {
            int new_exp = watson->getExp() - 50;
            if (new_exp < 0) new_exp = 0;
            watson->setExp(new_exp);
        }
    }
};

// singly linked list
struct Node {
    BaseItem* item;
    Node* next;
    Node(BaseItem* item, Node* next = nullptr) : item(item), next(next) {};
};

class BaseBag {
protected:
    Character* obj;
    int count;
    int capacity;
    Node* head;
public:
    BaseBag(Character* obj, int capacity) : obj(obj), count(0), capacity(capacity), head(nullptr) {};
    virtual ~BaseBag() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp->item;
            delete temp;
        }
    }
    friend class TestStudyInPink;

    virtual bool insert(BaseItem* item) {
        head = new Node(item, head);
        count++;
        return true;
        return false;
    }

    virtual bool isFull() {
        return count >= capacity;
    }

    virtual BaseItem* get() {
        if (head == nullptr) return NULL;

        Node* previous_item = nullptr;
        Node* current_item = head;
        while (current_item != nullptr && !current_item->item->canUse(obj, nullptr)) {
            previous_item = current_item;
            current_item = current_item->next;
        }
        if (current_item == nullptr) return NULL;
        if (previous_item == nullptr) {
            head = current_item->next;
        }
        else {
            previous_item->next = current_item->next;
            current_item->next = head;
            head = current_item;
        }

        BaseItem* item = head->item;
        delete head;
        head = current_item->next;
        count--;
        return item;
    }

    virtual BaseItem* get(ItemType itemType) {
        if (head == nullptr) return NULL;

        Node* previous_item = nullptr;
        Node* current_item = head;

        while (current_item != nullptr && !(current_item->item->getItemType() == itemType && current_item->item->canUse(obj, nullptr))) {
            previous_item = current_item;
            current_item = current_item->next;
        }
        if (current_item == nullptr) return NULL;
        if (previous_item == nullptr) {
            head = current_item->next;
        }
        else {
            previous_item->next = current_item->next;
            current_item->next = head;
            head = current_item;
        }

        BaseItem* item = head->item;
        delete head;
        head = current_item->next;
        count--;
        return item;
    }

    virtual string str() {
        if (count == 0) return "Bag[count=0]";

        string strg = "Bag[count=" + to_string(count) + ";";
        Node* current_item = head;
        while (current_item != nullptr) {
            switch (current_item->item->getItemType()) {
            case MAGIC_BOOK: {
                strg += "MagicBook,";
                break;
            }
            case ENERGY_DRINK: {
                strg += "EnergyDrink,";
                break;
            }
            case FIRST_AID: {
                strg += "FirstAid,";
                break;
            }
            case EXCEMPTION_CARD: {
                strg += "ExcemptionCard,";
                break;
            }
            case PASSING_CARD: {
                strg += "PassingCard,";
                break;
            }
            }
        }
        *(strg.end() - 1) = ']';
        return strg;
    }
    
    virtual int getCount() {
        return count;
    }
};

class SherlockBag : public BaseBag {
private:
    Sherlock* sherlock;
public:
    SherlockBag(Sherlock* sherlock) : BaseBag(sherlock, 13) {};
    ~SherlockBag() {};
    friend class TestStudyInPink;
};

class WatsonBag : public BaseBag {
private:
    Watson* watson;
public:
    WatsonBag(Watson* watson) : BaseBag(watson, 15) {};
    ~WatsonBag() {};
    friend class TestStudyInPink;
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
    StudyPinkProgram(const string& config_file_path) : config() {
        config = new Configuration(config_file_path);

        map = new Map(config->map_num_rows, config->map_num_cols, config->num_walls, config->arr_walls, config->num_fake_walls, config->arr_fake_walls);

        sherlock = new Sherlock(1, config->sherlock_moving_rule, config->sherlock_init_pos, map, config->sherlock_init_hp, config->sherlock_init_exp);
        watson = new Watson(2, config->watson_moving_rule, config->watson_init_pos, map, config->watson_init_hp, config->watson_init_exp);
        criminal = new Criminal(0, config->criminal_init_pos, map, sherlock, watson);

        arr_mv_objs = new ArrayMovingObject(config->max_num_moving_objects);
        arr_mv_objs->add(criminal);
        arr_mv_objs->add(sherlock);
        arr_mv_objs->add(watson);
    }
    friend class TestStudyInPink;

    bool isStop() const {
        if (sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition())) return false;
        if (sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition())) return false;
        if (sherlock->getHp() == 0) return false;
        if (watson->getHp() == 0) return false;
        return true;
    }

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

        SherlockBag* sherlock_bag = new SherlockBag(sherlock);
        WatsonBag* watson_bag = new WatsonBag(watson);
        for (int istep = 0; istep < config->num_steps; ++istep) {
            for (int i = 0; i < arr_mv_objs->size(); ++i) {
                arr_mv_objs->get(i)->move();

                Character* character = dynamic_cast<Character*>(arr_mv_objs->get(i));
                if (arr_mv_objs->get(i)->getName() == "Sherlock" || arr_mv_objs->get(i)->getName() == "Watson") {
                    for (int j = 0; i < arr_mv_objs->size(); j++) {
                        Robot* robot = dynamic_cast<Robot*>(arr_mv_objs->get(j));
                        if (robot->getName() == "Sherlock" || robot->getName() == "Watson") {
                            continue;
                        }

                        if (character->getCurrentPosition().isEqual(robot->getCurrentPosition())) {
                            if (character->getName() == "Sherlock") {
                                BaseItem* excemption_card = sherlock_bag->get(EXCEMPTION_CARD);
                                switch (robot->getRobotType()) {
                                case S: {
                                    if (sherlock->getExp() > 400) {
                                        BaseItem* item = robot->getItem();
                                        if(!sherlock_bag->isFull()) sherlock_bag->insert(item);
                                    }
                                    else if (excemption_card) {
                                        if (excemption_card->canUse(sherlock, robot)) {
                                            BaseItem* item = robot->getItem();
                                            if (!sherlock_bag->isFull()) sherlock_bag->insert(item);
                                        }
                                    }
                                    else sherlock->setExp(sherlock->getExp() * 0.9);
                                    break;
                                }
                                case W: {
                                    BaseItem* item = robot->getItem();
                                    if (!sherlock_bag->isFull()) sherlock_bag->insert(item);
                                    break;
                                }
                                case SW:{
                                    if (sherlock->getExp() > 300 && sherlock->getHp() > 335) {
                                        BaseItem* item = robot->getItem();
                                        if (!sherlock_bag->isFull()) sherlock_bag->insert(item);
                                    }
                                    else if (excemption_card) {
                                        if (excemption_card->canUse(sherlock, robot)) {
                                            BaseItem* item = robot->getItem();
                                            if (!sherlock_bag->isFull()) sherlock_bag->insert(item);
                                        }
                                    }
                                    else {
                                        sherlock->setExp(sherlock->getExp() * 0.85);
                                        sherlock->setHp(sherlock->getHp() * 0.85);
                                    }
                                    break;
                                }
                                case C: {
                                    if (sherlock->getExp() > 500) {
                                        sherlock->setPosition(criminal->getCurrentPosition().getRow(), criminal->getCurrentPosition().getCol());
                                    }
                                    else if (excemption_card) {
                                        if (excemption_card->canUse(sherlock, robot)) {
                                            sherlock->setPosition(criminal->getCurrentPosition().getRow(), criminal->getCurrentPosition().getCol());
                                        }
                                    }
                                    else {
                                        BaseItem* item = robot->getItem();
                                        if (!sherlock_bag->isFull()) sherlock_bag->insert(item);
                                    }
                                    break;
                                }
                                }
                                if (excemption_card) delete excemption_card;
                            }
                            if (character->getName() == "Watson") {
                                BaseItem* passing_card = watson_bag->get(PASSING_CARD);
                                switch (robot->getRobotType()) {
                                case S: {
                                    break;
                                }
                                case SW: {
                                    if (watson->getExp() > 600 && watson->getHp() > 165) {
                                        BaseItem* item = robot->getItem();
                                        if (!watson_bag->isFull()) watson_bag->insert(item);
                                    }
                                    else if (passing_card) {
                                        if (passing_card->canUse(watson, robot)) {
                                            passing_card->use(watson, robot);
                                        }
                                    }
                                    else {
                                        watson->setExp(watson->getExp() * 0.85);
                                        watson->setHp(watson->getHp() * 0.85);
                                    }
                                    break;
                                }
                                case W: {
                                    if (watson->getHp() > 350) {
                                        BaseItem* item = robot->getItem();
                                        if (!watson_bag->isFull()) watson_bag->insert(item);
                                    }
                                    else if (passing_card) {
                                        BaseItem* item = robot->getItem();
                                        if (!watson_bag->isFull()) watson_bag->insert(item);
                                    }
                                    else {
                                        watson->setHp(watson->getHp() * 0.95);
                                    }
                                    break;
                                }
                                case C: {
                                    BaseItem* item = robot->getItem();
                                    if (!watson_bag->isFull()) watson_bag->insert(item);
                                    break;
                                }
                                }
                                if (passing_card) delete passing_card;
                            }
                        }
                    }
                }
                if (isStop()) {
                    printStep(istep);
                    break;
                }
                if (verbose) {
                    printStep(istep);
                }
            }
            if (sherlock->getCurrentPosition().isEqual(watson->getCurrentPosition())) {
                BaseItem* item = nullptr;
                while (sherlock_bag->get(PASSING_CARD) && (!watson_bag->isFull())) {
                    watson_bag->insert(item);
                    item = nullptr;
                }
                while (watson_bag->get(EXCEMPTION_CARD) && (!sherlock_bag->isFull())) {
                    sherlock_bag->insert(item);
                    item = nullptr;
                }
            }
            if ((istep + 1) % 3 == 0) {
                Robot* new_robot = nullptr;
                BaseItem* new_item = nullptr;
                if (istep + 1 == 3) {
                    new_robot = new RobotC(3, criminal->getCurrentPosition(), map, criminal);
                    int r_row = criminal->getCurrentPosition().getRow();
                    int r_col = criminal->getCurrentPosition().getCol();
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
                        new_item = new MagicBook();
                        break;
                    }
                    case 2:
                    case 3: {
                        new_item = new EnergyDrink();
                        break;
                    }
                    case 4:
                    case 5: {
                        new_item = new FirstAid();
                        break;
                    }
                    case 6:
                    case 7: {
                        new_item = new ExcemptionCard();
                        break;
                    }
                    case 8:
                    case 9: {
                        int t = (r_row * 11 + r_col) % 4;
                        switch (t) {
                        case 0: {
                            new_item = new PassingCard("RobotS");
                            break;
                        }
                        case 1: {
                            new_item = new PassingCard("RobotC");
                            break;
                        }
                        case 2: {
                            new_item = new PassingCard("RobotSW");
                            break;
                        }
                        case 3: {
                            new_item = new PassingCard("all");
                            break;
                        }
                        }
                        break;
                    }
                    }
                    new_robot->setItem(new_item);
                    arr_mv_objs->add(new_robot);
                }
                else {
                    int index = (istep + 1) / 3 + 2;
                    int distance_to_sherlock = abs(sherlock->getCurrentPosition().getRow() - criminal->getCurrentPosition().getRow()) + abs(sherlock->getCurrentPosition().getCol() - criminal->getCurrentPosition().getCol());
                    int distance_to_watson = abs(watson->getCurrentPosition().getRow() - criminal->getCurrentPosition().getRow()) + abs(watson->getCurrentPosition().getCol() - criminal->getCurrentPosition().getCol());
                    if (distance_to_sherlock > distance_to_watson) {
                        new_robot = new RobotS(index, criminal->getCurrentPosition(), map, criminal, sherlock);
                        int r_row = criminal->getCurrentPosition().getRow();
                        int r_col = criminal->getCurrentPosition().getCol();
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
                            new_item = new MagicBook();
                            break;
                        }
                        case 2:
                        case 3: {
                            new_item = new EnergyDrink();
                            break;
                        }
                        case 4:
                        case 5: {
                            new_item = new FirstAid();
                            break;
                        }
                        case 6:
                        case 7: {
                            new_item = new ExcemptionCard();
                            break;
                        }
                        case 8:
                        case 9: {
                            int t = (r_row * 11 + r_col) % 4;
                            switch (t) {
                            case 0: {
                                new_item = new PassingCard("RobotS");
                                break;
                            }
                            case 1: {
                                new_item = new PassingCard("RobotC");
                                break;
                            }
                            case 2: {
                                new_item = new PassingCard("RobotSW");
                                break;
                            }
                            case 3: {
                                new_item = new PassingCard("all");
                                break;
                            }
                            }
                            break;
                        }
                        }
                        new_robot->setItem(new_item);
                        arr_mv_objs->add(new_robot);
                    }
                    else if (distance_to_sherlock < distance_to_watson) {
                        new_robot = new RobotW(index, criminal->getCurrentPosition(), map, criminal, watson);
                        int r_row = criminal->getCurrentPosition().getRow();
                        int r_col = criminal->getCurrentPosition().getCol();
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
                            new_item = new MagicBook();
                            break;
                        }
                        case 2:
                        case 3: {
                            new_item = new EnergyDrink();
                            break;
                        }
                        case 4:
                        case 5: {
                            new_item = new FirstAid();
                            break;
                        }
                        case 6:
                        case 7: {
                            new_item = new ExcemptionCard();
                            break;
                        }
                        case 8:
                        case 9: {
                            int t = (r_row * 11 + r_col) % 4;
                            switch (t) {
                            case 0: {
                                new_item = new PassingCard("RobotS");
                                break;
                            }
                            case 1: {
                                new_item = new PassingCard("RobotC");
                                break;
                            }
                            case 2: {
                                new_item = new PassingCard("RobotSW");
                                break;
                            }
                            case 3: {
                                new_item = new PassingCard("all");
                                break;
                            }
                            }
                            break;
                        }
                        }
                        new_robot->setItem(new_item);
                        arr_mv_objs->add(new_robot);
                    }
                    else {
                        new_robot = new RobotSW(index, criminal->getCurrentPosition(), map, criminal, sherlock, watson);
                        int r_row = criminal->getCurrentPosition().getRow();
                        int r_col = criminal->getCurrentPosition().getCol();
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
                            new_item = new MagicBook();
                            break;
                        }
                        case 2:
                        case 3: {
                            new_item = new EnergyDrink();
                            break;
                        }
                        case 4:
                        case 5: {
                            new_item = new FirstAid();
                            break;
                        }
                        case 6:
                        case 7: {
                            new_item = new ExcemptionCard();
                            break;
                        }
                        case 8:
                        case 9: {
                            int t = (r_row * 11 + r_col) % 4;
                            switch (t) {
                            case 0: {
                                new_item = new PassingCard("RobotS");
                                break;
                            }
                            case 1: {
                                new_item = new PassingCard("RobotC");
                                break;
                            }
                            case 2: {
                                new_item = new PassingCard("RobotSW");
                                break;
                            }
                            case 3: {
                                new_item = new PassingCard("all");
                                break;
                            }
                            }
                            break;
                        }
                        }
                        new_robot->setItem(new_item);
                        arr_mv_objs->add(new_robot);
                    }
                    delete new_robot;
                }
            }
            BaseItem* sherlock_heal = sherlock_bag->get();
            if (sherlock_heal) {
                sherlock_heal->use(sherlock, NULL);
                sherlock_heal = nullptr;
            }
            BaseItem* watson_heal = watson_bag->get();
            if (watson_heal) {
                watson_heal->use(watson, NULL);
                watson_heal = nullptr;
            }
        }
        delete sherlock_bag;
        delete watson_bag;
        printResult();
    }

    ~StudyPinkProgram() {
        delete config;
        delete sherlock;
        delete watson;
        delete criminal;
        delete map;
        delete arr_mv_objs;
    }
};

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
#endif // _H_STUDY_IN_PINK_2_H_
