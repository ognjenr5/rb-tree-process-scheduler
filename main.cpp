#include <iostream>
#include <fstream>

#include "core/Process.h"
#include "core/TreeNode.h"

using namespace std;

void load_tree_from_file(TreeNode* tree, const string& filename, string* msg) {

    if (tree->get_root()->n_count() != 0) {
        *msg = "Tree already created.\n";
        return;
    }

    ifstream file;
    file.open(filename);
    if (!file.is_open()) {
        *msg = "File couldn't be opened.\n";
        return;
    }

    string name;
    int t1, t2;

    while (file >> name >> t1 >> t2) {
        Process* p = new Process(name, t1, t2);
        tree->insert(p);
    }

    *msg = "Tree successfully loaded from file.\n";

    file.close();
}

void load_tree_from_cin(TreeNode* tree, string* msg) {

    if (tree->get_root()->n_count() != 0) {
        *msg = "Tree already created.\n";
        return;
    }

    string name;
    int t1, t2;

    cout << "Enter the process name, the time required for execution, and the maximum waiting time in that order. Enter -1 to stop.\n";

    while (true) {
        cin >> name;
        if (name == "-1") break;
        cin >> t1 >> t2;
        if (t1 <= 0 || t2 <= 0) continue;
        Process* p = new Process(name, t1, t2);
        tree->insert(p);
    }

    *msg = "Tree successfully loaded.\n";
}

#include <cstdlib>

void clear_console() {
#ifdef _WIN32
    std::system("cls");  // Windows
#else
    std::system("clear"); // Linux/macOS
#endif
}

void run() {
    TreeNode* tree = nullptr;
    char option;
    string msg = "\n";

    while (true){
        cout << msg;
        cout << "-------------------------------\n"
            "0. Create tree\n"
            "1. Destroy tree\n"
            "2. Insert process into the tree\n"
            "3. Load tree from file\n"
            "4. Load tree from standard input\n"
            "5. Detailed information about a specific process\n"
            "6. Delete process from the tree\n"
            "7. Print tree\n"
            "8. Simulate one step\n"
            "9. Full simulation\n"
            "e. Exit program\n"
            "-------------------------------\n"
            "Choose an option:\t";

        cin >> option;

        clear_console();

        switch (option) {
            case '0':
                tree = new TreeNode();
                msg =  "Tree successfully created.\n";
                break;

            case '1':
                if (!tree) msg =  "Tree has not been created.\n";
                else {
                    delete tree;
                    msg =  "Tree successfully deleted.\n";
                    tree = nullptr;
                }
                break;

            case '2':
                if (!tree) msg =  "Tree has not been created.\n";
                else {
                    string name;
                    int t1, t2;

                    cout << "Enter process name, execution time, and maximum waiting time respectively.\n";

                    while (true) {
                        cin >> name >> t1 >> t2;

                        if (t1 > 0 && t2 > 2) {
                            tree->insert(new Process(name, t1, t2));
                            break;
                        }

                        cout << "Invalid input (times must be natural numbers).\n";
                    }

                    msg = "Process successfully inserted.\n";
                }
                break;

            case '3':
                if (!tree) msg = "Tree has not been created.\n";
                else {
                    cout << "Enter file name: ";
                    string filename;
                    cin >> filename;
                    load_tree_from_file(tree, filename, &msg);
                }
                break;

            case '4':
                if (!tree) msg =  "Tree has not been created.\n";
                else {
                    load_tree_from_cin(tree, &msg);
                }
                break;

            case '5':
                if (!tree) msg = "Tree has not been created.\n";
                else {
                    cout << "Enter the current waiting time and current execution time of the requested process.\n";

                    int wait_time, exec_time;
                    cin >> wait_time >> exec_time;

                    Process* p = tree->find(wait_time, exec_time);

                    if (!p) msg = "Requested process not found.\n";
                    else {
                        msg = "Process with the given times was found.\n";
                        msg += p->detailed_print();
                    }
                }
                break;

            case '6':
                if (!tree) msg = "Tree has not been created.\n";
                else {
                    cout << "Enter the current waiting time and current execution time of the requested process.\n";

                    int wait_time, exec_time;
                    cin >> wait_time >> exec_time;

                    tree->remove(wait_time, exec_time, &msg);
                }
                break;

            case '7':
                if (!tree) msg = "Tree has not been created.\n";
                else {
                    cout << "Print tree as:\n"
                            "1. 2-3-4 tree\n"
                            "2. Red-Black tree\n";

                    char suboption;
                    cin >> suboption;

                    if (suboption != '1' && suboption != '2') {
                        cout << "Invalid option.\n";
                        break;
                    }

                    if (suboption == '1')
                        tree->print(cout);
                    else
                        tree->print_rb(cout);

                    msg = "";
                }
                break;

            case '8':
                if (!tree) cout << "Tree has not been created.\n";
                else {
                    int time_slice;

                    cout << "Enter time increment (natural number): ";
                    cin >> time_slice;

                    if (time_slice <= 0) {
                        cout << "Time increment must be a natural number.\n";
                        break;
                    }

                    tree->simulation_step(time_slice, cout);
                }
                break;

            case '9':
                if (!tree) cout << "Tree has not been created.\n";
                else {
                    int time_slice;

                    cout << "Enter time increment (natural number): ";
                    cin >> time_slice;

                    if (time_slice <= 0) {
                        cout << "Time increment must be a natural number.\n";
                        break;
                    }

                    string filename;
                    cout << "Enter file name for step output: ";
                    cin >> filename;

                    ofstream os(filename);

                    if (!os.is_open()) {
                        msg =  "Failed to create file.\n";
                    }
                    else
                        tree->simulate(time_slice, os);

                    os.close();
                }
                break;

            case 'e':
                exit(0);

            default:
                cout << "Enter a valid option.\n";
                break;
        }
    }
}

int main () {
    run();

    return 0;

}
