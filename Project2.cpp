//============================================================================
// Name        : Project2.cpp
// Author      : Sam Blanton
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Project 2
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <time.h>
#include <fstream>
#include <sstream>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 10;


// define a structure to hold bid information
struct Course {
    string courseId; // unique identifier
    string title;
    vector<string> prerequisites;
};

class HashTable {

private:
    // Define structures to hold courses
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a course
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    void Search(string courseId);
    void loadCourses(const std::string& csvPath, HashTable* hashTable);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initialized vector that holds courses to the size of tableSize
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // Initialized vector size to the passed-through variable
    this->tableSize = size;
    nodes.resize(size);
}

/**
 * Destructor
 */
HashTable::~HashTable() {
    // Implemented logic to free storage by erasing nodes at the beginning.
    nodes.erase(nodes.begin());
}

/**
 * Calculate the hash value of a given key.
 * Note that the key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // Return the hash for a given key
    return key % tableSize;
}

/**
 * Insert a course
 *
 * @param course The course to insert
 */
void HashTable::Insert(Course course) {
    // Create the key for the given course
    unsigned key = hash(atoi(course.courseId.c_str()));
    // Retrieve the node using the key
    Node* oldNode = &(nodes.at(key));
    // If no entry found for the key
    if (oldNode == nullptr) {
        Node* newNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*newNode));
    } else {
        // If node is initialized, insert the data
        if (oldNode->key == UINT_MAX) {
            oldNode->key = key;
            oldNode->course = course;
            oldNode->next = nullptr;
        } else {
            // Handle collisions by iterating until the next empty node is found
            while (oldNode->next != nullptr) {
                oldNode = oldNode->next;
            }
            // Assign the new node to the empty next node.
            oldNode->next = new Node(course, key);
        }
    }
}

/**
 * Print all courses
 */
void HashTable::PrintAll() {
    cout << endl;

    // Create a vector to store the courses for sorting
    vector<Course> courses;

    // Collect all courses into the vector
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        if (it->key != UINT_MAX) {
            courses.push_back(it->course);
            Node* node = it->next;
            while (node != nullptr) {
                courses.push_back(node->course);
                node = node->next;
            }
        }
    }

    // Sort the courses alphabetically
    sort(courses.begin(), courses.end(), [](const Course& a, const Course& b) {
        return a.courseId < b.courseId;
    });

    // Print the sorted courses
    for (const Course& course : courses) {
        cout << course.courseId << ", " << course.title << endl;
    }

    cout << endl;
}

/**
 * Search for a course by courseId
 *
 * @param courseId The courseId to search for
 */
void HashTable::Search(string courseId) {
    Course course;

    // Find the node at the vector position
    unsigned key = hash(atoi(courseId.c_str()));
    Node* node = &(nodes.at(key));

    cout << endl;

    // Check if the node at the calculated key matches the given courseId
    if (node != nullptr && node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
        cout << node->course.courseId << ", " << node->course.title << endl;
        cout << "Prerequisites: ";
        for (int i = 0; i < node->course.prerequisites.size(); i++) {
            cout << node->course.prerequisites[i] << ' ';
        }
        cout << '\n' << endl;
        return;
    }

    // Search for the courseId in the linked list of nodes at the same key position
    while (node != nullptr) {
        // Check if the current node matches the given courseId
        if (node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
            cout << node->course.courseId << ", " << node->course.title << endl;
            cout << "Prerequisites: ";
            for (int i = 0; i < node->course.prerequisites.size(); i++) {
                cout << node->course.prerequisites[i] << ' ';
            }
            cout << '\n' << endl;
            return;
        }
        node = node->next;
    }

    // If the course is not found, print a message indicating so
    cout << "Course not found" << '\n' << endl;
}


/**
 * Load courses from a CSV file into the hash table
 *
 * @param csvPath The path to the CSV file
 * @param hashTable The hash table to load the courses into
 */
void loadCourses(const string& csvPath, HashTable* hashTable) {
    // Print the path of the CSV file being loaded
    cout << '\n' <<"Loading CSV file: " << csvPath << endl;

    // Open the CSV file for reading
    ifstream file(csvPath);
    if (!file.is_open()) {
        // Check if the file opening failed
        cerr << "Error opening file: " << csvPath << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        // Read each line of the CSV file

        // Create a stringstream from the line
        stringstream ss(line);
        string token;

        // Create a Course object to store the course information
        Course course;

        // Extract the courseId and title from the stringstream
        getline(ss, course.courseId, ',');
        getline(ss, course.title, ',');

        // Extract the prerequisites from the stringstream and add them to the Course object
        while (getline(ss, token, ',')) {
            if (!token.empty())
                course.prerequisites.push_back(token);
        }

        // Insert the Course object into the hash table
        hashTable->Insert(course);
    }

    // Close the file
    file.close();

    // Print a message indicating successful data loading
    cout << "Data Loaded Successfully" << '\n' << endl;
}


int main() {
    const char* filename = "input.csv";
    int choice;
    string courseId;

    // Define a hash table to hold all the courses
    HashTable* courseTable;

    // Create an instance of the HashTable
    courseTable = new HashTable();

    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: " << endl;
        cin >> choice;

        switch (choice) {
            case 1:
                // Load the course data from the CSV file into the hash table
                loadCourses(filename, courseTable);
                break;
            case 2:
                // Print the list of all courses in the hash table
                courseTable->PrintAll();
                break;
            case 3:
                cout << "What course do you want to see?" << endl;
                cin >> courseId;

                // Search for and print the details of a specific course
                courseTable->Search(courseId);
                break;
            case 9:
                // Exit the program
                break;
            default:
                cout << "Invalid Option" << endl;
                break;
        }
    }

    // Print a closing message
    cout << "Thank you for using the course planner!" << endl;

    return 0;
}

