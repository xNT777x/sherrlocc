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

    // Calculate in_req_exp
    int calReqExp(int r, int c) {
        return (r * 257 + c * 139 + 89) % 900 + 1;
    }

    }
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

            map[wall_row][wall_col] = new Wall();
        }

        for (int i = 0; i < num_fake_walls; i++) {
            int fake_wall_row = array_fake_walls[i].getRow();
            int fake_wall_col = array_fake_walls[i].getCol();
            int in_req_exp = FakeWall::calReqExp(fake_wall_row, fake_wall_col);

            map[fake_wall_row][fake_wall_col] = new FakeWall(in_req_exp);
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

        if (r < 0 || c < 0 || r > num_rows || c || num_cols) {
            return false;
        }
        if (element->getType() == WALL) {
            return false;
        }
        if (element->getType() == FAKE_WALL) {
            FakeWall* fake_wall = dynamic_cast<FakeWall*>(element);

            //Come back here once MovingObject is done
            if(fake_wall && mv_obj.string == "Watson" &&)
        }

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
        sscanf(string.c_str(), "(%d,%d)", &r, &c);
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
    virtual ~MovingObject();
    virtual Position getNextPosition() = 0;
    Position getCurrentPosition() const {
        return pos;
    }
    virtual void move() = 0;
    virtual string str() const = 0;
    virtual int getExp() = 0;
    virtual int getHp() = 0;
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
        case 'L': current_row--;
        case 'R': current_row++;
        case 'U': current_col++;
        case 'D': current_col--;
        }
        //wrap around, might remove
        if (current_row < 0) current_row = map->getNumRows() - 1;
        if (current_row > map->getNumRows() - 1) current_row = 0;
        if (current_col < 0) current_col = map->getNumCols() - 1;
        if (current_col > map->getNumCols() - 1) current_col = 0;

        moveIndex = (moveIndex + 1) % moving_rule.size();
        
        return pos(current_row, current_col);
    } 
    void move() override {
    }
    string str() override {

    }

};

class Watson : public MovingObject {
private:
    

public:
    Watson(int index, const string & moving_rule, const Position & init_pos, Map * map, int init_hp, int init_exp);
    Position getNextPosition() {};
    void move() {
        this->getNextPosition();
    }
    string str() {
        return "Watson[index=" + this.index + ",pos=" + this.pos + ",moving_rule" + ;
    }
};

class Criminal : public MovingObject {
private:
    // TODO

public:
    Criminal(int index, const Position & init_pos, Map * map, Sherlock * sherlock, Watson * watson);
    // getNextPosition
    // move
    string str() {
        return "Watson[index=" + this.index + ",pos=" + this.pos + "]";
     }
    // ...
};

class ArrayMovingObject {
private:
    int count = 0;
    int capacity;
    MovingObject* arr_mv_objs[capacity];

public:
    ArrayMovingObject(int capacity) {
        this->capacity = capacity;

        for (int i = 0; i < this.capacity; i++) {
            *arr_mv_objs
        }
    };

    ~ArrayMovingObject() ;
    bool isFull() const;
    bool add(MovingObject * mv_obj);
    MovingObject * get(int index) const;
    int size() const; // return current number of elements in the array
    string str() const;
};

class Configuration {
    friend class StudyPinkProgram;

private:
    // TODO

public:
    Configuration(const string & filepath);
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
